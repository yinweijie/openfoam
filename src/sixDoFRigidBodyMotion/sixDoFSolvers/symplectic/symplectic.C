/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2016 OpenFOAM Foundation
    Copyright (C) 2023 OpenCFD Ltd.
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

#include "sixDoFSolvers/symplectic/symplectic.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace sixDoFSolvers
{
    defineTypeNameAndDebug(symplectic, 0);
    addToRunTimeSelectionTable(sixDoFSolver, symplectic, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::sixDoFSolvers::symplectic::symplectic
(
    const dictionary& dict,
    sixDoFRigidBodyMotion& body
)
:
    sixDoFSolver(dict, body)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::sixDoFSolvers::symplectic::~symplectic()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::sixDoFSolvers::symplectic::solve
(
    bool firstIter,
    const vector& fGlobal,
    const vector& tauGlobal,
    scalar deltaT,
    scalar deltaT0
)
{
    // First simplectic step:
    //     Half-step for linear and angular velocities
    //     Update position and orientation

    v() = tConstraints() & (v0() + aDamp()*0.5*deltaT0*a0());
    pi() = rConstraints() & (pi0() + aDamp()*0.5*deltaT0*tau0());

    centreOfRotation() = centreOfRotation0() + deltaT*v();

    Tuple2<tensor, vector> Qpi = rotate(Q0(), pi(), deltaT);
    Q() = Qpi.first();
    pi() = rConstraints() & Qpi.second();

    // Update the linear acceleration and torque
    updateAcceleration(fGlobal, tauGlobal);

    // Update the constraints to the object
    updateConstraints();

    // Second simplectic step:
    //     Complete update of linear and angular velocities

    v() += tConstraints() & aDamp()*0.5*deltaT*a();
    pi() += rConstraints() & aDamp()*0.5*deltaT*tau();
}

// ************************************************************************* //
