/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2016-2023 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "submodels/CloudFunctionObjects/FacePostProcessing/FacePostProcessing.H"
#include "db/IOstreams/Pstreams/Pstream.H"
#include "containers/Lists/ListListOps/ListListOps.H"
#include "parallel/globalIndex/globalIndex.H"
#include "writers/common/surfaceWriter.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class CloudType>
void Foam::FacePostProcessing<CloudType>::makeLogFile
(
    const word& zoneName,
    const label zoneI,
    const label nFaces,
    const scalar totArea
)
{
    // Create the output file if not already created
    if (logToFile_)
    {
        DebugInfo << "Creating output file." << endl;

        if (Pstream::master())
        {
            // Create directory if does not exist
            mkDir(this->writeTimeDir());

            // Open new file at start up
            outputFilePtr_.set
            (
                zoneI,
                new OFstream
                (
                    this->writeTimeDir()/(type() + '_' + zoneName + ".dat")
                )
            );

            outputFilePtr_[zoneI]
                << "# Source    : " << type() << nl
                << "# Face zone : " << zoneName << nl
                << "# Faces     : " << nFaces << nl
                << "# Area      : " << totArea << nl
                << "# Time" << tab << "mass" << tab << "massFlowRate" << endl;
        }
    }
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class CloudType>
void Foam::FacePostProcessing<CloudType>::write()
{
    const fvMesh& mesh = this->owner().mesh();
    const Time& time = mesh.time();
    const faceZoneMesh& fzm = mesh.faceZones();
    scalar timeNew = time.value();
    scalar timeElapsed = timeNew - timeOld_;

    totalTime_ += timeElapsed;

    const scalar alpha = (totalTime_ - timeElapsed)/totalTime_;
    const scalar beta = timeElapsed/totalTime_;

    forAll(faceZoneIDs_, zoneI)
    {
        massFlowRate_[zoneI] =
            alpha*massFlowRate_[zoneI] + beta*mass_[zoneI]/timeElapsed;
        massTotal_[zoneI] += mass_[zoneI];
    }

    const label proci = Pstream::myProcNo();

    Log_<< type() << " output:" << nl;

    List<scalarField> zoneMassTotal(mass_.size());
    List<scalarField> zoneMassFlowRate(massFlowRate_.size());
    forAll(faceZoneIDs_, zoneI)
    {
        const word& zoneName = fzm[faceZoneIDs_[zoneI]].name();

        scalarListList allProcMass(Pstream::nProcs());
        allProcMass[proci] = massTotal_[zoneI];
        Pstream::gatherList(allProcMass);
        zoneMassTotal[zoneI] =
            ListListOps::combine<scalarList>
            (
                allProcMass, accessOp<scalarList>()
            );
        const scalar sumMassTotal = sum(zoneMassTotal[zoneI]);

        scalarListList allProcMassFlowRate(Pstream::nProcs());
        allProcMassFlowRate[proci] = massFlowRate_[zoneI];
        Pstream::gatherList(allProcMassFlowRate);
        zoneMassFlowRate[zoneI] =
            ListListOps::combine<scalarList>
            (
                allProcMassFlowRate, accessOp<scalarList>()
            );
        const scalar sumMassFlowRate = sum(zoneMassFlowRate[zoneI]);

        Log_<< "    " << zoneName
            << ": total mass = " << sumMassTotal
            << "; average mass flow rate = " << sumMassFlowRate
            << nl;

        if (outputFilePtr_.set(zoneI))
        {
            OFstream& os = outputFilePtr_[zoneI];
            os  << time.timeName() << token::TAB << sumMassTotal << token::TAB
                << sumMassFlowRate<< endl;
        }
    }

    Log_<< endl;


    if (surfaceFormat_ != "none")
    {
        forAll(faceZoneIDs_, zoneI)
        {
            const faceZone& fZone = fzm[faceZoneIDs_[zoneI]];

            labelList pointToGlobal;
            labelList uniqueMeshPointLabels;
            autoPtr<globalIndex> globalPointsPtr =
                mesh.globalData().mergePoints
                (
                    fZone().meshPoints(),
                    fZone().meshPointMap(),
                    pointToGlobal,
                    uniqueMeshPointLabels
                );

            pointField uniquePoints(mesh.points(), uniqueMeshPointLabels);
            List<pointField> allProcPoints(Pstream::nProcs());
            allProcPoints[proci] = uniquePoints;
            Pstream::gatherList(allProcPoints);

            faceList faces(fZone().localFaces());
            forAll(faces, i)
            {
                inplaceRenumber(pointToGlobal, faces[i]);
            }
            List<faceList> allProcFaces(Pstream::nProcs());
            allProcFaces[proci] = faces;
            Pstream::gatherList(allProcFaces);

            if (Pstream::master())
            {
                pointField allPoints
                (
                    ListListOps::combine<pointField>
                    (
                        allProcPoints, accessOp<pointField>()
                    )
                );

                faceList allFaces
                (
                    ListListOps::combine<faceList>
                    (
                        allProcFaces, accessOp<faceList>()
                    )
                );

                auto writer = surfaceWriter::New
                (
                    surfaceFormat_,
                    surfaceWriter::formatOptions
                    (
                        this->coeffDict(),
                        surfaceFormat_
                    )
                );

                if (debug)
                {
                    writer->verbose(true);
                }

                writer->open
                (
                    allPoints,
                    allFaces,
                    (this->writeTimeDir() / fZone.name()),
                    false  // serial - already merged
                );

                writer->nFields(2); // Legacy VTK
                writer->write("massTotal", zoneMassTotal[zoneI]);
                writer->write("massFlowRate", zoneMassFlowRate[zoneI]);
            }
        }
    }


    if (resetOnWrite_)
    {
        forAll(faceZoneIDs_, zoneI)
        {
            massFlowRate_[zoneI] = 0.0;
        }
        timeOld_ = timeNew;
        totalTime_ = 0.0;
    }

    forAll(mass_, zoneI)
    {
        mass_[zoneI] = 0.0;
    }

    // writeProperties();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::FacePostProcessing<CloudType>::FacePostProcessing
(
    const dictionary& dict,
    CloudType& owner,
    const word& modelName
)
:
    CloudFunctionObject<CloudType>(dict, owner, modelName, typeName),
    faceZoneIDs_(),
    surfaceFormat_(this->coeffDict().getWord("surfaceFormat")),
    resetOnWrite_(this->coeffDict().getBool("resetOnWrite")),
    logToFile_(this->coeffDict().getBool("log")),
    totalTime_(0.0),
    mass_(),
    massTotal_(),
    massFlowRate_(),
    outputFilePtr_(),
    timeOld_(owner.mesh().time().value())
{
    wordList faceZoneNames(this->coeffDict().lookup("faceZones"));
    mass_.setSize(faceZoneNames.size());
    massTotal_.setSize(faceZoneNames.size());
    massFlowRate_.setSize(faceZoneNames.size());

    outputFilePtr_.setSize(faceZoneNames.size());

    DynamicList<label> zoneIDs;
    const faceZoneMesh& fzm = owner.mesh().faceZones();
    const surfaceScalarField& magSf = owner.mesh().magSf();
    const polyBoundaryMesh& pbm = owner.mesh().boundaryMesh();

    forAll(faceZoneNames, i)
    {
        const word& zoneName = faceZoneNames[i];
        label zoneI = fzm.findZoneID(zoneName);

        if (zoneI != -1)
        {
            zoneIDs.append(zoneI);
            const faceZone& fz = fzm[zoneI];
            mass_[i].setSize(fz.size(), 0.0);
            massTotal_[i].setSize(fz.size(), 0.0);
            massFlowRate_[i].setSize(fz.size(), 0.0);

            label nFaces = returnReduce(fz.size(), sumOp<label>());
            Info<< "        " << zoneName << " faces: " << nFaces << nl;

            scalar totArea = 0.0;
            for (const label facei : fz)
            {
                if (facei < owner.mesh().nInternalFaces())
                {
                    totArea += magSf[facei];
                }
                else
                {
                    const label patchi = pbm.patchID(facei);
                    const polyPatch& pp = pbm[patchi];

                    if
                    (
                        !magSf.boundaryField()[patchi].coupled()
                     || refCast<const coupledPolyPatch>(pp).owner()
                    )
                    {
                        label localFacei = pp.whichFace(facei);
                        totArea += magSf.boundaryField()[patchi][localFacei];
                    }
                }
            }
            totArea = returnReduce(totArea, sumOp<scalar>());

            makeLogFile(zoneName, i, nFaces, totArea);
        }
    }

    faceZoneIDs_.transfer(zoneIDs);

    // readProperties(); AND initialise mass... fields
}


template<class CloudType>
Foam::FacePostProcessing<CloudType>::FacePostProcessing
(
    const FacePostProcessing<CloudType>& pff
)
:
    CloudFunctionObject<CloudType>(pff),
    faceZoneIDs_(pff.faceZoneIDs_),
    surfaceFormat_(pff.surfaceFormat_),
    resetOnWrite_(pff.resetOnWrite_),
    logToFile_(pff.logToFile_),
    totalTime_(pff.totalTime_),
    mass_(pff.mass_),
    massTotal_(pff.massTotal_),
    massFlowRate_(pff.massFlowRate_),
    outputFilePtr_(),
    timeOld_(0.0)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
bool Foam::FacePostProcessing<CloudType>::postFace
(
    const parcelType& p,
    const typename parcelType::trackingData& td
)
{
    if
    (
        this->owner().solution().output()
     || this->owner().solution().transient()
    )
    {
        const faceZoneMesh& fzm = this->owner().mesh().faceZones();

        forAll(faceZoneIDs_, i)
        {
            const faceZone& fz = fzm[faceZoneIDs_[i]];

            label faceId = -1;
            forAll(fz, j)
            {
                if (fz[j] == p.face())
                {
                    faceId = j;
                    break;
                }
            }

            if (faceId != -1)
            {
                mass_[i][faceId] += p.mass()*p.nParticle();
            }
        }
    }

    return true;
}


// ************************************************************************* //
