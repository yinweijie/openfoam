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

#include "submodels/addOns/radiation/absorptionEmission/cloudAbsorptionEmission/cloudAbsorptionEmission.H"
#include "clouds/baseClasses/thermoCloud/thermoCloud.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    namespace radiation
    {
        defineTypeNameAndDebug(cloudAbsorptionEmission, 0);

        addToRunTimeSelectionTable
        (
            absorptionEmissionModel,
            cloudAbsorptionEmission,
            dictionary
        );
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::radiation::cloudAbsorptionEmission::cloudAbsorptionEmission
(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    absorptionEmissionModel(dict, mesh),
    coeffsDict_(dict.subDict(typeName + "Coeffs")),
    cloudNames_(coeffsDict_.lookup("cloudNames"))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::radiation::cloudAbsorptionEmission::~cloudAbsorptionEmission()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::radiation::cloudAbsorptionEmission::aDisp(const label) const
{
    tmp<volScalarField> ta
    (
        new volScalarField
        (
            IOobject
            (
                "a",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            ),
            mesh_,
            dimensionedScalar(dimless/dimLength, Zero)
        )
    );

    forAll(cloudNames_, i)
    {
        const thermoCloud& tc
        (
            mesh_.objectRegistry::lookupObject<thermoCloud>(cloudNames_[i])
        );

        ta.ref() += tc.ap();
    }

    return ta;
}


Foam::tmp<Foam::volScalarField>
Foam::radiation::cloudAbsorptionEmission::eDisp(const label bandI) const
{
    return tmp<volScalarField>::New
    (
        IOobject
        (
            "e",
            mesh_.time().timeName(),
            mesh_,
            IOobject::NO_READ,
            IOobject::NO_WRITE,
            IOobject::NO_REGISTER
        ),
        mesh_,
        dimensionedScalar(dimless/dimLength, Zero)
    );
}


Foam::tmp<Foam::volScalarField>
Foam::radiation::cloudAbsorptionEmission::EDisp(const label bandI) const
{
    tmp<volScalarField> tE
    (
        new volScalarField
        (
            IOobject
            (
                "E",
                mesh_.time().timeName(),
                mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            ),
            mesh_,
            dimensionedScalar(dimMass/dimLength/pow3(dimTime), Zero)
        )
    );

    forAll(cloudNames_, i)
    {
        const thermoCloud& tc
        (
            mesh_.objectRegistry::lookupObject<thermoCloud>(cloudNames_[i])
        );

        tE.ref() += tc.Ep();
    }

    // Total emission is 4 times the projected emission
    return 4*tE;
}


// ************************************************************************* //
