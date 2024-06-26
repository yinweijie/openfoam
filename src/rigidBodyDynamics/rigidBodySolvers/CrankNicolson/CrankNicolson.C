/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
    Copyright (C) 2020 OpenCFD Ltd.
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

#include "rigidBodySolvers/CrankNicolson/CrankNicolson.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace RBD
{
namespace rigidBodySolvers
{
    defineTypeNameAndDebug(CrankNicolson, 0);
    addToRunTimeSelectionTable(rigidBodySolver, CrankNicolson, dictionary);
}
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::RBD::rigidBodySolvers::CrankNicolson::CrankNicolson
(
    rigidBodyMotion& body,
    const dictionary& dict
)
:
    rigidBodySolver(body),
    aoc_(dict.getOrDefault<scalar>("aoc", 0.5)),
    voc_(dict.getOrDefault<scalar>("voc", 0.5))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::RBD::rigidBodySolvers::CrankNicolson::~CrankNicolson()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::RBD::rigidBodySolvers::CrankNicolson::solve
(
    const scalarField& tau,
    const Field<spatialVector>& fx
)
{
    // Accumulate the restraint forces
    scalarField rtau(tau);
    Field<spatialVector> rfx(fx);
    model_.applyRestraints(rtau, rfx, state());

    // Calculate the accelerations for the given state and forces
    model_.forwardDynamics(state(), rtau, rfx);

    // Correct velocity
    qDot() = qDot0() + deltaT()*(aoc_*qDdot() + (1 - aoc_)*qDdot0());

    // Correct position
    q() = q0() + deltaT()*(voc_*qDot() + (1 - voc_)*qDot0());

    correctQuaternionJoints();
}


// ************************************************************************* //
