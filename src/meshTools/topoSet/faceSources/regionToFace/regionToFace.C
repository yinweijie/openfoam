/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2017 OpenFOAM Foundation
    Copyright (C) 2018-2022 OpenCFD Ltd.
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

#include "topoSet/faceSources/regionToFace/regionToFace.H"
#include "meshes/polyMesh/polyMesh.H"
#include "topoSet/topoSets/faceSet.H"
#include "mappedPatches/mappedPolyPatch/mappedPatchBase.H"
#include "meshes/primitiveMesh/primitivePatch/indirectPrimitivePatch.H"
#include "meshes/primitiveMesh/PatchTools/PatchTools.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "algorithms/PatchEdgeFaceWave/PatchEdgeFaceWave.H"
#include "algorithms/PatchEdgeFaceWave/edgeTopoDistanceData.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(regionToFace, 0);
    addToRunTimeSelectionTable(topoSetSource, regionToFace, word);
    addToRunTimeSelectionTable(topoSetSource, regionToFace, istream);
    addToRunTimeSelectionTable(topoSetFaceSource, regionToFace, word);
    addToRunTimeSelectionTable(topoSetFaceSource, regionToFace, istream);
    addNamedToRunTimeSelectionTable
    (
        topoSetFaceSource,
        regionToFace,
        word,
        region
    );
    addNamedToRunTimeSelectionTable
    (
        topoSetFaceSource,
        regionToFace,
        istream,
        region
    );
}


Foam::topoSetSource::addToUsageTable Foam::regionToFace::usage_
(
    regionToFace::typeName,
    "\n    Usage: regionToFace <faceSet> (x y z)\n\n"
    "    Select all faces in the connected region of the faceSet"
    " starting from the point.\n"
);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::regionToFace::markZone
(
    const indirectPrimitivePatch& patch,
    const label proci,
    const label facei,
    const label zonei,
    labelList& faceZone
) const
{
    // Data on all edges and faces
    List<edgeTopoDistanceData<label>> allEdgeInfo(patch.nEdges());
    List<edgeTopoDistanceData<label>> allFaceInfo(patch.size());

    DynamicList<label> changedEdges;
    DynamicList<edgeTopoDistanceData<label>> changedInfo;

    if (Pstream::myProcNo() == proci)
    {
        const labelList& fEdges = patch.faceEdges()[facei];
        for (const label edgei : fEdges)
        {
            changedEdges.append(edgei);
            changedInfo.append
            (
                edgeTopoDistanceData<label>
                (
                    0,          // distance
                    zonei
                )
            );
        }
    }

    // Walk
    PatchEdgeFaceWave
    <
        indirectPrimitivePatch,
        edgeTopoDistanceData<label>
    > calc
    (
        mesh_,
        patch,
        changedEdges,
        changedInfo,
        allEdgeInfo,
        allFaceInfo,
        returnReduce(patch.nEdges(), sumOp<label>())
    );

    forAll(allFaceInfo, facei)
    {
        if
        (
            allFaceInfo[facei].valid(calc.data())
         && allFaceInfo[facei].data() == zonei
        )
        {
            faceZone[facei] = zonei;
        }
    }
}


void Foam::regionToFace::combine(topoSet& set, const bool add) const
{
    if (verbose_)
    {
        Info<< "    Loading subset " << setName_
            << " to delimit search region." << endl;
    }

    faceSet subSet(mesh_, setName_);

    indirectPrimitivePatch patch
    (
        IndirectList<face>(mesh_.faces(), subSet.toc()),
        mesh_.points()
    );

    mappedPatchBase::nearInfo ni
    (
        pointIndexHit(false, Zero, -1),
        Tuple2<scalar, label>
        (
            sqr(GREAT),
            Pstream::myProcNo()
        )
    );

    forAll(patch, i)
    {
        const point& fc = patch.faceCentres()[i];
        scalar d2 = magSqr(fc-nearPoint_);

        if (!ni.first().hit() || d2 < ni.second().first())
        {
            ni.second().first() = d2;
            ni.first().hitPoint(fc);
            ni.first().setIndex(i);
        }
    }

    // Globally reduce
    Pstream::combineReduce(ni, mappedPatchBase::nearestEqOp());

    if (verbose_)
    {
        Info<< "    Found nearest face at " << ni.first().point()
            << " on processor " << ni.second().second()
            << " face " << ni.first().index()
            << " distance " << Foam::sqrt(ni.second().first()) << endl;
    }

    labelList faceRegion(patch.size(), -1);
    markZone
    (
        patch,
        ni.second().second(),   // proci
        ni.first().index(),     // start face
        0,                      // currentZone
        faceRegion
    );

    forAll(faceRegion, facei)
    {
        if (faceRegion[facei] == 0)
        {
            addOrDelete(set, patch.addressing()[facei], add);
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::regionToFace::regionToFace
(
    const polyMesh& mesh,
    const word& setName,
    const point& nearPoint
)
:
    topoSetFaceSource(mesh),
    setName_(setName),
    nearPoint_(nearPoint)
{}


Foam::regionToFace::regionToFace
(
    const polyMesh& mesh,
    const dictionary& dict
)
:
    topoSetFaceSource(mesh),
    setName_(dict.get<word>("set")),
    nearPoint_(dict.get<point>("nearPoint"))
{}


Foam::regionToFace::regionToFace
(
    const polyMesh& mesh,
    Istream& is
)
:
    topoSetFaceSource(mesh),
    setName_(checkIs(is)),
    nearPoint_(checkIs(is))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::regionToFace::applyToSet
(
    const topoSetSource::setAction action,
    topoSet& set
) const
{
    if (action == topoSetSource::ADD || action == topoSetSource::NEW)
    {
        if (verbose_)
        {
            Info<< "    Adding all faces of connected region of set "
                << setName_ << " starting from point " << nearPoint_
                << " ..." << endl;
        }

        combine(set, true);
    }
    else if (action == topoSetSource::SUBTRACT)
    {
        if (verbose_)
        {
            Info<< "    Removing all cells of connected region of set "
                << setName_ << " starting from point " << nearPoint_
                << " ..." << endl;
        }

        combine(set, false);
    }
}


// ************************************************************************* //
