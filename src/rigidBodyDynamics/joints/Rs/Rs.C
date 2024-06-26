/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
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

#include "joints/Rs/Rs.H"
#include "rigidBodyModel/rigidBodyModel.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace RBD
{
namespace joints
{
    defineTypeNameAndDebug(Rs, 0);

    addToRunTimeSelectionTable
    (
        joint,
        Rs,
        dictionary
    );
}
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::RBD::joints::Rs::Rs()
:
    joint(3)
{
    S_[0] = spatialVector(1, 0, 0, 0, 0, 0);
    S_[1] = spatialVector (0, 1, 0, 0, 0, 0);
    S_[2] = spatialVector(0, 0, 1, 0, 0, 0);
}


Foam::RBD::joints::Rs::Rs(const dictionary& dict)
:
    joint(3)
{
    S_[0] = spatialVector(1, 0, 0, 0, 0, 0);
    S_[1] = spatialVector (0, 1, 0, 0, 0, 0);
    S_[2] = spatialVector(0, 0, 1, 0, 0, 0);
}


Foam::autoPtr<Foam::RBD::joint> Foam::RBD::joints::Rs::clone() const
{
    return autoPtr<joint>(new Rs(*this));
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::RBD::joints::Rs::~Rs()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

bool Foam::RBD::joints::Rs::unitQuaternion() const
{
    return true;
}


void Foam::RBD::joints::Rs::jcalc
(
    joint::XSvc& J,
    const scalarField& q,
    const scalarField& qDot
) const
{
    J.X.E() = joint::unitQuaternion(q).R().T();
    J.X.r() = Zero;

    J.S = Zero;
    J.S.xx() = 1;
    J.S.yy() = 1;
    J.S.zz() = 1;

    J.v = spatialVector(qDot.block<vector>(qIndex_), Zero);
    J.c = Zero;
}


// ************************************************************************* //
