/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "SLGThermo/SLGThermo.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(SLGThermo, 0);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::SLGThermo::SLGThermo(const fvMesh& mesh, fluidThermo& thermo)
:
    regIOobject
    (
        IOobject
        (
            SLGThermo::typeName,
            mesh.polyMesh::instance(),
            mesh
        )
    ),
    thermo_(thermo),
    carrier_(nullptr),
    liquids_(nullptr),
    solids_(nullptr)
{
    Info<< "Creating component thermo properties:" << endl;

    if (isA<basicSpecieMixture>(thermo))
    {
        basicSpecieMixture& mcThermo =
            dynamic_cast<basicSpecieMixture&>(thermo);
        carrier_ = &mcThermo;

        Info<< "    multi-component carrier - " << mcThermo.species().size()
            << " species" << endl;
    }
    else
    {
        Info<< "    single component carrier" << endl;
    }

    if (thermo.found("liquids"))
    {
        liquids_ = liquidMixtureProperties::New(thermo.subDict("liquids"));
        Info<< "    liquids - " << liquids_->components().size()
            << " components" << endl;
    }
    else
    {
        Info<< "    no liquid components" << endl;
    }

    if (thermo.found("solids"))
    {
        solids_  = solidMixtureProperties::New(thermo.subDict("solids"));
        Info<< "    solids - " << solids_->components().size()
            << " components" << endl;
    }
    else
    {
        Info<< "    no solid components" << endl;
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::SLGThermo::~SLGThermo()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::fluidThermo& Foam::SLGThermo::thermo() const
{
    return thermo_;
}


const Foam::basicSpecieMixture& Foam::SLGThermo::carrier() const
{
    if (carrier_ == nullptr)
    {
        FatalErrorInFunction
            << "carrier requested, but object is not allocated"
            << abort(FatalError);
    }

    return *carrier_;
}


const Foam::liquidMixtureProperties& Foam::SLGThermo::liquids() const
{
    if (!liquids_)
    {
        FatalErrorInFunction
            << "liquids requested, but object is not allocated"
            << abort(FatalError);
    }

    return *liquids_;
}


const Foam::solidMixtureProperties& Foam::SLGThermo::solids() const
{
    if (!solids_)
    {
        FatalErrorInFunction
            << "solids requested, but object is not allocated"
            << abort(FatalError);
    }

    return *solids_;
}


Foam::label Foam::SLGThermo::carrierId
(
    const word& cmptName,
    bool allowNotfound
) const
{
    forAll(carrier().species(), i)
    {
        if (cmptName == carrier_->species()[i])
        {
            return i;
        }
    }

    if (!allowNotfound)
    {
        FatalErrorInFunction
            << "Unknown carrier component " << cmptName
            << ". Valid carrier components are:" << nl
            << carrier_->species() << exit(FatalError);
    }

    return -1;
}


Foam::label Foam::SLGThermo::liquidId
(
    const word& cmptName,
    bool allowNotfound
) const
{
    forAll(liquids().components(), i)
    {
        if (cmptName == liquids_->components()[i])
        {
            return i;
        }
    }

    if (!allowNotfound)
    {
        FatalErrorInFunction
            << "Unknown liquid component " << cmptName << ". Valid liquids are:"
            << nl << liquids_->components() << exit(FatalError);
    }

    return -1;
}


Foam::label Foam::SLGThermo::solidId
(
    const word& cmptName,
    bool allowNotfound
) const
{
    forAll(solids().components(), i)
    {
        if (cmptName == solids_->components()[i])
        {
            return i;
        }
    }

    if (!allowNotfound)
    {
        FatalErrorInFunction
            << "Unknown solid component " << cmptName << ". Valid solids are:"
            << nl << solids_->components() << exit(FatalError);
    }

    return -1;
}


bool Foam::SLGThermo::hasMultiComponentCarrier() const
{
    return (carrier_ != nullptr);
}


bool Foam::SLGThermo::hasLiquids() const
{
    return bool(liquids_);
}


bool Foam::SLGThermo::hasSolids() const
{
    return bool(solids_);
}


// ************************************************************************* //
