/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2012 OpenFOAM Foundation
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

#include "surfaceRegistry/surfaceRegistry.H"
#include "db/Time/TimeOpenFOAM.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(surfaceRegistry, 0);
}

const Foam::word Foam::surfaceRegistry::prefix("surfaces");

Foam::word Foam::surfaceRegistry::defaultName("default");


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::surfaceRegistry::surfaceRegistry
(
    const objectRegistry& obr
)
:
    surfaceRegistry(obr, surfaceRegistry::defaultName)
{}


Foam::surfaceRegistry::surfaceRegistry
(
    const objectRegistry& obr,
    const word& surfName
)
:
    objectRegistry
    (
        IOobject
        (
            (surfName.empty() ? surfaceRegistry::defaultName : surfName),
            obr.time().timeName(),
            surfaceRegistry::prefix,
            obr,
            IOobjectOption::NO_READ,
            IOobjectOption::NO_WRITE,
            IOobjectOption::REGISTER
        )
    )
{}


// ************************************************************************* //
