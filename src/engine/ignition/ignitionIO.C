/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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

#include "engineTime/engineTime/engineTime.H"
#include "ignition/ignition.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::ignition::ignition
(
    const dictionary& combustionProperties,
    const Time& db,
    const fvMesh& mesh
)
:
    mesh_(mesh),
    ignite_(combustionProperties.get<bool>("ignite")),
    ignSites_
    (
        combustionProperties.lookup("ignitionSites"),
        ignitionSite::iNew(db, mesh)
    )
{
    if (ignite_)
    {
        Info<< "\nIgnition on" << endl;
    }
    else
    {
        Info<< "\nIgnition switched off" << endl;
    }
}


Foam::ignition::ignition
(
    const dictionary& combustionProperties,
    const engineTime& edb,
    const fvMesh& mesh
)
:
    mesh_(mesh),
    ignite_(combustionProperties.get<bool>("ignite")),
    ignSites_
    (
        combustionProperties.lookup("ignitionSites"),
        ignitionSite::iNew(edb, mesh)
    )
{
    if (ignite_)
    {
        Info<< "\nIgnition on" << endl;
    }
    else
    {
        Info<< "\nIgnition switched off" << endl;
    }
}


// ************************************************************************* //
