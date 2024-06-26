/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015 OpenFOAM Foundation
    Copyright (C) 2020-2023 OpenCFD Ltd.
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

#include "sixDoFSolvers/CrankNicolson/CrankNicolson.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace sixDoFSolvers
{
    defineTypeNameAndDebug(CrankNicolson, 0);
    addToRunTimeSelectionTable(sixDoFSolver, CrankNicolson, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::sixDoFSolvers::CrankNicolson::CrankNicolson
(
    const dictionary& dict,
    sixDoFRigidBodyMotion& body
)
:
    sixDoFSolver(dict, body),
    aoc_(dict.getOrDefault<scalar>("aoc", 0.5)),
    voc_(dict.getOrDefault<scalar>("voc", 0.5))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::sixDoFSolvers::CrankNicolson::~CrankNicolson()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::sixDoFSolvers::CrankNicolson::solve
(
    bool firstIter,
    const vector& fGlobal,
    const vector& tauGlobal,
    scalar deltaT,
    scalar deltaT0
)
{
    // Update the linear acceleration and torque
    updateAcceleration(fGlobal, tauGlobal);

    // Update the constraints to the object
    updateConstraints();

    // Correct linear velocity
    v() = tConstraints()
      & (v0() + aDamp()*deltaT*(aoc_*a() + (1 - aoc_)*a0()));

    // Correct angular momentum
    pi() = rConstraints()
      & (pi0() + aDamp()*deltaT*(aoc_*tau() + (1 - aoc_)*tau0()));

    // Correct position
    centreOfRotation() =
        centreOfRotation0() + deltaT*(voc_*v() + (1 - voc_)*v0());

    // Correct orientation
    Tuple2<tensor, vector> Qpi =
        rotate(Q0(), (voc_*pi() + (1 - voc_)*pi0()), deltaT);
    Q() = Qpi.first();
}

// ************************************************************************* //
