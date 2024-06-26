/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2016-2020 OpenCFD Ltd.
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

#include "surfaceInterpolate/fieldFunctionObjects_surfaceInterpolate.H"
#include "fields/surfaceFields/surfaceFields.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(surfaceInterpolate, 0);
    addToRunTimeSelectionTable(functionObject, surfaceInterpolate, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::surfaceInterpolate::surfaceInterpolate
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    fvMeshFunctionObject(name, runTime, dict),
    fieldSet_()
{
    read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::functionObjects::surfaceInterpolate::read
(
    const dictionary& dict
)
{
    fvMeshFunctionObject::read(dict);

    dict.readEntry("fields", fieldSet_);

    return true;
}


bool Foam::functionObjects::surfaceInterpolate::execute()
{
    Log << type() << " " << name() << " write:" << nl;

    interpolateFields<scalar>();
    interpolateFields<vector>();
    interpolateFields<sphericalTensor>();
    interpolateFields<symmTensor>();
    interpolateFields<tensor>();

    Log << endl;

    return true;
}


bool Foam::functionObjects::surfaceInterpolate::write()
{
    Log << "    functionObjects::" << type() << " " << name()
        << " writing interpolated surface fields:" << nl;

    forAll(fieldSet_, i)
    {
        const word& fieldName = fieldSet_[i].second();

        const regIOobject* ioptr = obr_.findObject<regIOobject>(fieldName);

        if (ioptr)
        {
            Log << "        " << fieldName << nl;
            ioptr->write();
        }
        else
        {
            WarningInFunction
                << "Unable to find field " << fieldName
                << " in the mesh database" << endl;
        }
    }

    Log << endl;

    return true;
}


// ************************************************************************* //
