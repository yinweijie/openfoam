/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017,2022 OpenFOAM Foundation
    Copyright (C) 2020-2022 OpenCFD Ltd.
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

#include "fvMesh/fvMesh.H"
#include "db/Time/TimeOpenFOAM.H"
#include "fields/volFields/volFields.H"
#include "fields/surfaceFields/surfaceFields.H"
#include "fields/volFields/slicedVolFields.H"
#include "fields/surfaceFields/slicedSurfaceFields.H"
#include "fields/Fields/Field/SubField.H"
#include "fields/fvPatchFields/constraint/cyclic/cyclicFvPatchFields.H"
#include "fields/fvPatchFields/constraint/cyclicAMI/cyclicAMIFvPatchFields.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::fvMesh::makeSf() const
{
    DebugInFunction << "Assembling face areas" << endl;

    // It is an error to attempt to recalculate
    // if the pointer is already set
    if (SfPtr_)
    {
        FatalErrorInFunction
            << "face areas already exist"
            << abort(FatalError);
    }

    SfPtr_ = new slicedSurfaceVectorField
    (
        IOobject
        (
            "S",
            pointsInstance(),
            meshSubDir,
            *this,
            IOobject::NO_READ,
            IOobject::NO_WRITE,
            IOobject::NO_REGISTER
        ),
        *this,
        dimArea,
        faceAreas()
    );

    SfPtr_->setOriented();
}


void Foam::fvMesh::makeMagSf() const
{
    DebugInFunction << "Assembling mag face areas" << endl;

    // It is an error to attempt to recalculate
    // if the pointer is already set
    if (magSfPtr_)
    {
        FatalErrorInFunction
            << "mag face areas already exist"
            << abort(FatalError);
    }

    // Note: Added stabilisation for faces with exactly zero area.
    // These should be caught on mesh checking but at least this stops
    // the code from producing Nans.
    magSfPtr_ = new surfaceScalarField
    (
        IOobject
        (
            "magSf",
            pointsInstance(),
            meshSubDir,
            *this,
            IOobject::NO_READ,
            IOobject::NO_WRITE,
            IOobject::NO_REGISTER
        ),
        mag(Sf()) + dimensionedScalar("vs", dimArea, VSMALL)
    );
}


void Foam::fvMesh::makeC() const
{
    DebugInFunction << "Assembling cell centres" << endl;

    // It is an error to attempt to recalculate
    // if the pointer is already set
    if (CPtr_)
    {
        FatalErrorInFunction
            << "cell centres already exist"
            << abort(FatalError);
    }

    // Construct as slices. Only preserve processor (not e.g. cyclic)

    CPtr_ = new slicedVolVectorField
    (
        IOobject
        (
            "C",
            pointsInstance(),
            meshSubDir,
            *this,
            IOobject::NO_READ,
            IOobject::NO_WRITE,
            IOobject::NO_REGISTER
        ),
        *this,
        dimLength,
        cellCentres(),
        faceCentres(),
        true,               //preserveCouples
        true                //preserveProcOnly
    );
}


void Foam::fvMesh::makeCf() const
{
    DebugInFunction << "Assembling face centres" << endl;

    // It is an error to attempt to recalculate
    // if the pointer is already set
    if (CfPtr_)
    {
        FatalErrorInFunction
            << "face centres already exist"
            << abort(FatalError);
    }

    CfPtr_ = new slicedSurfaceVectorField
    (
        IOobject
        (
            "Cf",
            pointsInstance(),
            meshSubDir,
            *this,
            IOobject::NO_READ,
            IOobject::NO_WRITE,
            IOobject::NO_REGISTER
        ),
        *this,
        dimLength,
        faceCentres()
    );
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::volScalarField::Internal& Foam::fvMesh::V() const
{
    if (!VPtr_)
    {
        DebugInFunction
            << "Constructing from primitiveMesh::cellVolumes()" << endl;

        VPtr_ = new SlicedDimensionedField<scalar, volMesh>
        (
            IOobject
            (
                "V",
                time().timeName(),
                *this,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            ),
            *this,
            dimVolume,
            cellVolumes()
        );
    }

    return *VPtr_;
}


const Foam::volScalarField::Internal& Foam::fvMesh::V0() const
{
    if (!V0Ptr_)
    {
        FatalErrorInFunction
            << "V0 is not available"
            << abort(FatalError);
    }

    return *V0Ptr_;
}


Foam::volScalarField::Internal& Foam::fvMesh::setV0()
{
    if (!V0Ptr_)
    {
        FatalErrorInFunction
            << "V0 is not available"
            << abort(FatalError);
    }

    return *V0Ptr_;
}


const Foam::volScalarField::Internal& Foam::fvMesh::V00() const
{
    if (!V00Ptr_)
    {
        DebugInFunction << "Constructing from V0" << endl;

        V00Ptr_ = new DimensionedField<scalar, volMesh>
        (
            IOobject
            (
                "V00",
                time().timeName(),
                *this,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                IOobject::REGISTER
            ),
            V0()
        );


        // If V00 is used then V0 should be stored for restart
        V0Ptr_->writeOpt(IOobject::AUTO_WRITE);
    }

    return *V00Ptr_;
}


Foam::tmp<Foam::volScalarField::Internal> Foam::fvMesh::Vsc() const
{
    if (!steady() && moving() && time().subCycling())
    {
        const TimeState& ts = time();
        const TimeState& ts0 = time().prevTimeState();

        scalar tFrac =
        (
            ts.value() - (ts0.value() - ts0.deltaTValue())
        )/ts0.deltaTValue();

        if (tFrac < (1 - SMALL))
        {
            return V0() + tFrac*(V() - V0());
        }
    }

    return V();
}


Foam::tmp<Foam::volScalarField::Internal> Foam::fvMesh::Vsc0() const
{
    if (!steady() && moving() && time().subCycling())
    {
        const TimeState& ts = time();
        const TimeState& ts0 = time().prevTimeState();

        scalar t0Frac =
        (
            (ts.value() - ts.deltaTValue())
          - (ts0.value() - ts0.deltaTValue())
        )/ts0.deltaTValue();

        if (t0Frac > SMALL)
        {
            return V0() + t0Frac*(V() - V0());
        }
    }

    return V0();
}


const Foam::surfaceVectorField& Foam::fvMesh::Sf() const
{
    if (!SfPtr_)
    {
        makeSf();
    }

    return *SfPtr_;
}


const Foam::surfaceScalarField& Foam::fvMesh::magSf() const
{
    if (!magSfPtr_)
    {
        makeMagSf();
    }

    return *magSfPtr_;
}


Foam::tmp<Foam::surfaceVectorField> Foam::fvMesh::unitSf() const
{
    auto tunitVectors = tmp<surfaceVectorField>::New
    (
        IOobject
        (
            "unit(Sf)",
            pointsInstance(),
            meshSubDir,
            *this,
            IOobject::NO_READ,
            IOobject::NO_WRITE,
            IOobject::NO_REGISTER
        ),
        *this,
        dimless,
        (this->Sf() / this->magSf())
    );

    tunitVectors.ref().oriented() = this->Sf().oriented();
    return tunitVectors;
}


const Foam::volVectorField& Foam::fvMesh::C() const
{
    if (!CPtr_)
    {
        makeC();
    }

    return *CPtr_;
}


const Foam::surfaceVectorField& Foam::fvMesh::Cf() const
{
    if (!CfPtr_)
    {
        makeCf();
    }

    return *CfPtr_;
}


Foam::tmp<Foam::surfaceVectorField> Foam::fvMesh::delta() const
{
    DebugInFunction << "Calculating face deltas" << endl;

    tmp<surfaceVectorField> tdelta
    (
        new surfaceVectorField
        (
            IOobject
            (
                "delta",
                pointsInstance(),
                meshSubDir,
                *this,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            ),
            *this,
            dimLength
        )
    );
    surfaceVectorField& delta = tdelta.ref();
    delta.setOriented();

    const volVectorField& C = this->C();
    const labelUList& owner = this->owner();
    const labelUList& neighbour = this->neighbour();

    forAll(owner, facei)
    {
        delta[facei] = C[neighbour[facei]] - C[owner[facei]];
    }

    surfaceVectorField::Boundary& deltabf =  delta.boundaryFieldRef();

    forAll(deltabf, patchi)
    {
        deltabf[patchi] = boundary()[patchi].delta();
    }

    return tdelta;
}


const Foam::surfaceScalarField& Foam::fvMesh::phi() const
{
    if (!phiPtr_)
    {
        FatalErrorInFunction
            << "mesh flux field does not exist, is the mesh actually moving?"
            << abort(FatalError);
    }

    // Set zero current time
    // mesh motion fluxes if the time has been incremented
    if (!time().subCycling() && phiPtr_->timeIndex() != time().timeIndex())
    {
        (*phiPtr_) = dimensionedScalar(dimVolume/dimTime, Zero);
    }

    phiPtr_->setOriented();

    return *phiPtr_;
}


Foam::refPtr<Foam::surfaceScalarField> Foam::fvMesh::setPhi()
{
    if (!phiPtr_)
    {
        return nullptr;
    }
    else
    {
        // Return non-const reference
        refPtr<surfaceScalarField> p;
        p.ref(*phiPtr_);
        return p;
    }
}


// ************************************************************************* //
