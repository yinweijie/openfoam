/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) YEAR AUTHOR,AFFILIATION
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

#include "FUNCTIONOBJECT.H"
#include "TimeOpenFOAM.H"
#include "fvMesh.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(FUNCTIONOBJECT, 0);
    addToRunTimeSelectionTable(functionObject, FUNCTIONOBJECT, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::FUNCTIONOBJECT::FUNCTIONOBJECT
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    fvMeshFunctionObject(name, runTime, dict),
    boolData_(dict.getOrDefault<bool>("boolData", true)),
    labelData_(dict.get<label>("labelData")),
    wordData_(dict.getOrDefault<word>("wordData", "defaultWord")),
    scalarData_(dict.getOrDefault<scalar>("scalarData", 1.0))
{
    read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::functionObjects::FUNCTIONOBJECT::read(const dictionary& dict)
{
    dict.readEntry("boolData", boolData_);
    dict.readEntry("labelData", labelData_);
    dict.readIfPresent("wordData", wordData_);
    dict.readEntry("scalarData", scalarData_);

    return true;
}


bool Foam::functionObjects::FUNCTIONOBJECT::execute()
{
    return true;
}


bool Foam::functionObjects::FUNCTIONOBJECT::end()
{
    return true;
}


bool Foam::functionObjects::FUNCTIONOBJECT::write()
{
    return true;
}


// ************************************************************************* //
