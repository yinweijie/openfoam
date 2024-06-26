/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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

#include "FSD/reactionRateFlameAreaModels/relaxation/relaxation.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "finiteVolume/fvm/fvm.H"
#include "LES/LESModel/LESModel.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace reactionRateFlameAreaModels
{
    defineTypeNameAndDebug(relaxation, 0);
    addToRunTimeSelectionTable
    (
        reactionRateFlameArea,
        relaxation,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::reactionRateFlameAreaModels::relaxation::relaxation
(
    const word modelType,
    const dictionary& dict,
    const fvMesh& mesh,
    const combustionModel& combModel
)
:
    reactionRateFlameArea(modelType, dict, mesh, combModel),
    correlation_(dict.optionalSubDict(typeName + "Coeffs").subDict(fuel_)),
    C_(dict.optionalSubDict(typeName + "Coeffs").get<scalar>("C")),
    alpha_(dict.optionalSubDict(typeName + "Coeffs").get<scalar>("alpha"))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::reactionRateFlameAreaModels::relaxation::~relaxation()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::reactionRateFlameAreaModels::relaxation::correct
(
    const volScalarField& sigma
)
{
    dimensionedScalar omega0
    (
        "omega0",
        dimensionSet(1, -2, -1, 0, 0, 0, 0),
        correlation_.omega0()
    );

    dimensionedScalar sigmaExt
    (
        "sigmaExt",
        dimensionSet(0, 0, -1, 0, 0, 0, 0),
        correlation_.sigmaExt()
    );

    dimensionedScalar omegaMin
    (
        "omegaMin",
        omega0.dimensions(),
        1e-4
    );

    dimensionedScalar kMin
    (
        "kMin",
        sqr(dimVelocity),
        SMALL
    );

    const compressibleTurbulenceModel& turbulence = combModel_.turbulence();

    // Total strain
    const volScalarField sigmaTotal
    (
        sigma + alpha_*turbulence.epsilon()/(turbulence.k() + kMin)
    );

    const volScalarField omegaInf(correlation_.omega0Sigma(sigmaTotal));

    const volScalarField tau(C_*mag(sigmaTotal));

    volScalarField Rc
    (
        (tau*omegaInf*(omega0 - omegaInf) + sqr(omegaMin)*sigmaExt)
       /(sqr(omega0 - omegaInf) + sqr(omegaMin))
    );

    const volScalarField& rho = combModel_.rho();
    const tmp<surfaceScalarField> tphi = combModel_.phi();
    const surfaceScalarField& phi = tphi();

    solve
    (
         fvm::ddt(rho, omega_)
       + fvm::div(phi, omega_)
      ==
         rho*Rc*omega0
       - fvm::SuSp(rho*(tau + Rc), omega_)
    );

    omega_.clamp_range(0, omega0.value());
}


bool  Foam::reactionRateFlameAreaModels::relaxation::read
(
    const dictionary& dict
)
{
    if (reactionRateFlameArea::read(dict))
    {
        coeffDict_ = dict.optionalSubDict(typeName + "Coeffs");
        coeffDict_.readEntry("C", C_);
        coeffDict_.readEntry("alpha", alpha_);
        correlation_.read
        (
            coeffDict_.subDict(fuel_)
        );
        return true;
    }

    return false;
}

// ************************************************************************* //
