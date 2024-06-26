/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2012 OpenFOAM Foundation
    Copyright (C) 2017 OpenCFD Ltd.
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

#include "psiReactionThermo/psiReactionThermo.H"
#include "fvMesh/fvMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(psiReactionThermo, 0);
    defineRunTimeSelectionTable(psiReactionThermo, fvMesh);
    defineRunTimeSelectionTable(psiReactionThermo, fvMeshDictPhase);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::psiReactionThermo::psiReactionThermo
(
    const fvMesh& mesh,
    const word& phaseName
)
:
    psiThermo(mesh, phaseName)
{}


Foam::psiReactionThermo::psiReactionThermo
(
    const fvMesh& mesh,
    const word& phaseName,
    const word& dictName
)
:
    psiThermo(mesh, phaseName, dictName)
{}


// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::psiReactionThermo> Foam::psiReactionThermo::New
(
    const fvMesh& mesh,
    const word& phaseName
)
{
    return basicThermo::New<psiReactionThermo>(mesh, phaseName);
}


Foam::autoPtr<Foam::psiReactionThermo> Foam::psiReactionThermo::New
(
    const fvMesh& mesh,
    const word& phaseName,
    const word& dictName
)
{
    return basicThermo::New<psiReactionThermo>(mesh, phaseName, dictName);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::psiReactionThermo::~psiReactionThermo()
{}


// ************************************************************************* //
