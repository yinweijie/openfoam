/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
    Copyright (C) 2021 OpenCFD Ltd.
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

#include "matrices/tolerances/tolerances.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::tolerances::tolerances(const Time& t, const fileName& dictName)
:
    IOdictionary
    (
        IOobject
        (
            dictName,
            t.system(),
            t,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    ),

    // Named, but empty dictionaries
    relaxationFactors_("relaxationFactors"),
    solverTolerances_("solverTolerances"),
    solverRelativeTolerances_("solverRelativeTolerances")
{
    read();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::tolerances::read()
{
    if (regIOobject::read())
    {
        const word toleranceSetName(get<word>("toleranceSet"));
        const dictionary& toleranceSet = subDict(toleranceSetName);

        if (toleranceSet.found("relaxationFactors"))
        {
            relaxationFactors_ = toleranceSet.subDict("relaxationFactors");
        }

        if (toleranceSet.found("solverTolerances"))
        {
            solverTolerances_ = toleranceSet.subDict("solverTolerances");
        }

        if (toleranceSet.found("solverRelativeTolerances"))
        {
            solverRelativeTolerances_ =
                toleranceSet.subDict("solverRelativeTolerances");
        }

        return true;
    }

    return false;
}


bool Foam::tolerances::relax(const word& name) const
{
    return relaxationFactors_.found(name);
}


Foam::scalar Foam::tolerances::relaxationFactor(const word& name) const
{
    return relaxationFactors_.get<scalar>(name);
}


Foam::scalar Foam::tolerances::solverTolerance(const word& name) const
{
    return solverTolerances_.get<scalar>(name);
}


bool Foam::tolerances::solverRelativeTolerances() const
{
    return solverRelativeTolerances_.size();
}


Foam::scalar Foam::tolerances::solverRelativeTolerance(const word& name) const
{
    return solverRelativeTolerances_.get<scalar>(name);
}


// ************************************************************************* //
