/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "radiationModels/opaqueSolid/opaqueSolid.H"
#include "global/constants/physicoChemical/physicoChemicalConstants.H"
#include "fvMesh/fvMesh.H"
#include "db/Time/TimeOpenFOAM.H"
#include "fields/volFields/volFields.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    namespace radiation
    {
        defineTypeNameAndDebug(opaqueSolid, 0);

        addToRadiationRunTimeSelectionTables(opaqueSolid);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::radiation::opaqueSolid::opaqueSolid(const volScalarField& T)
:
    radiationModel(typeName, T)
{}


Foam::radiation::opaqueSolid::opaqueSolid
(
    const dictionary& dict,
    const volScalarField& T
)
:
    radiationModel(typeName, dict, T)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::radiation::opaqueSolid::~opaqueSolid()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::radiation::opaqueSolid::read()
{
    return radiationModel::read();
}


void Foam::radiation::opaqueSolid::calculate()
{}


Foam::tmp<Foam::volScalarField> Foam::radiation::opaqueSolid::Rp() const
{
    return tmp<volScalarField>::New
    (
        IOobject
        (
            "Rp",
            mesh_.time().timeName(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        dimensionedScalar
        (
            constant::physicoChemical::sigma.dimensions()/dimLength, Zero
        )
    );
}


Foam::tmp<Foam::DimensionedField<Foam::scalar, Foam::volMesh>>
Foam::radiation::opaqueSolid::Ru() const
{
    return tmp<volScalarField::Internal>::New
    (
        IOobject
        (
            "Ru",
            mesh_.time().timeName(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh_,
        dimensionedScalar(dimMass/dimLength/pow3(dimTime), Zero)
    );
}


Foam::label Foam::radiation::opaqueSolid::nBands() const
{
    return absorptionEmission_->nBands();
}

// ************************************************************************* //
