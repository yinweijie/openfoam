/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
    Copyright (C) 2019-2021 OpenCFD Ltd.
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

#include "bodies/rigidBody/rigidBody.H"
#include "bodies/subBody/subBody.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace RBD
{
    defineTypeNameAndDebug(rigidBody, 0);
    defineRunTimeSelectionTable(rigidBody, dictionary);

    addToRunTimeSelectionTable
    (
        rigidBody,
        rigidBody,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::RBD::rigidBody> Foam::RBD::rigidBody::clone() const
{
    return autoPtr<rigidBody>::New(*this);
}


// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

Foam::autoPtr<Foam::RBD::rigidBody> Foam::RBD::rigidBody::New
(
    const word& name,
    const scalar& m,
    const vector& c,
    const symmTensor& Ic
)
{
    return autoPtr<rigidBody>::New(name, m, c, Ic);
}


Foam::autoPtr<Foam::RBD::rigidBody> Foam::RBD::rigidBody::New
(
    const word& name,
    const dictionary& dict
)
{
    const word bodyType(dict.get<word>("type"));

    auto* ctorPtr = dictionaryConstructorTable(bodyType);

    if (!ctorPtr)
    {
        FatalIOErrorInLookup
        (
            dict,
            "rigidBody",
            bodyType,
            *dictionaryConstructorTablePtr_
        ) << exit(FatalIOError);
    }

    return autoPtr<rigidBody>(ctorPtr(name, dict));
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::RBD::rigidBody::~rigidBody()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

bool Foam::RBD::rigidBody::massless() const
{
    return false;
}


void Foam::RBD::rigidBody::merge(const subBody& subBody)
{
    *this = rigidBody
    (
        name(),
        *this + transform(subBody.masterXT(), subBody.body())
    );
}


void Foam::RBD::rigidBody::write(Ostream& os) const
{
    os.writeEntry("type", type());
    os.writeEntry("mass", m());
    os.writeEntry("centreOfMass", c());
    os.writeEntry("inertia", Ic());
}


// ************************************************************************* //
