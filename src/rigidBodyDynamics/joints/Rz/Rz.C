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

#include "joints/Rz/Rz.H"
#include "rigidBodyModel/rigidBodyModel.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace RBD
{
namespace joints
{
    defineTypeNameAndDebug(Rz, 0);

    addToRunTimeSelectionTable
    (
        joint,
        Rz,
        dictionary
    );
}
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::RBD::joints::Rz::Rz()
:
    joint(1)
{
    S_[0] = spatialVector(0, 0, 1, 0, 0, 0);
}


Foam::RBD::joints::Rz::Rz(const dictionary& dict)
:
    joint(1)
{
    S_[0] = spatialVector(0, 0, 1, 0, 0, 0);
}


Foam::autoPtr<Foam::RBD::joint> Foam::RBD::joints::Rz::clone() const
{
    return autoPtr<joint>(new Rz(*this));
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::RBD::joints::Rz::~Rz()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::RBD::joints::Rz::jcalc
(
    joint::XSvc& J,
    const scalarField& q,
    const scalarField& qDot
) const
{
    J.X = Xrz(q[qIndex_]);
    J.S1 = S_[0];
    J.v = Zero;
    J.v.wz() = qDot[qIndex_];
    J.c = Zero;
}


// ************************************************************************* //
