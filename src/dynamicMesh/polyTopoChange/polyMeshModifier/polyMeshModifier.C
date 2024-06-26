/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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

Description
    Virtual base class for mesh modifiers.

\*---------------------------------------------------------------------------*/

#include "polyTopoChange/polyMeshModifier/polyMeshModifier.H"
#include "db/dictionary/dictionary.H"
#include "polyTopoChange/polyTopoChanger/polyTopoChanger.H"
#include "meshes/polyMesh/polyMesh.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(polyMeshModifier, 0);
    defineRunTimeSelectionTable(polyMeshModifier, dictionary);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::polyMeshModifier::polyMeshModifier
(
    const word& name,
    const label index,
    const polyTopoChanger& mme,
    const bool act
)
:
    name_(name),
    index_(index),
    topoChanger_(mme),
    active_(act)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::polyTopoChanger& Foam::polyMeshModifier::topoChanger() const
{
    return topoChanger_;
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

Foam::Ostream& Foam::operator<<(Ostream& os, const polyMeshModifier& pmm)
{
    pmm.write(os);
    os.check(FUNCTION_NAME);
    return os;
}


// ************************************************************************* //
