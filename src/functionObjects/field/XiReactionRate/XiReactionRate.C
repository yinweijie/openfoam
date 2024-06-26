/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
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

#include "XiReactionRate/XiReactionRate.H"
#include "fields/volFields/volFields.H"
#include "finiteVolume/fvc/fvcGrad.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
    defineTypeNameAndDebug(XiReactionRate, 0);
    addToRunTimeSelectionTable(functionObject, XiReactionRate, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::functionObjects::XiReactionRate::XiReactionRate
(
    const word& name,
    const Time& runTime,
    const dictionary& dict
)
:
    fvMeshFunctionObject(name, runTime, dict)
{
    read(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::functionObjects::XiReactionRate::read(const dictionary& dict)
{
    return fvMeshFunctionObject::read(dict);
}


bool Foam::functionObjects::XiReactionRate::execute()
{
    return true;
}


bool Foam::functionObjects::XiReactionRate::write()
{
    const volScalarField& b = mesh_.lookupObject<volScalarField>("b");

    const volScalarField& Su = mesh_.lookupObject<volScalarField>("Su");

    const volScalarField& Xi = mesh_.lookupObject<volScalarField>("Xi");

    const volScalarField St
    (
        IOobject
        (
            scopedName("St"),
            time_.timeName(),
            mesh_
        ),
        Xi*Su
    );

    Log << "    Writing turbulent flame-speed field " << St.name()
        << " to " << time_.timeName() << endl;

    St.write();

    const volScalarField wdot
    (
        IOobject
        (
            scopedName("wdot"),
            time_.timeName(),
            mesh_
        ),
        St*mag(fvc::grad(b))
    );

    Log << "    Writing reaction-rate field " << wdot.name()
        << " to " << time_.timeName() << endl;

    wdot.write();

    return true;
}


// ************************************************************************* //
