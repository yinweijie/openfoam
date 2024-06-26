/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2019 OpenCFD Ltd.
    Copyright (C) 2019-2020 DLR
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

#include "implicitFunctions/sin/sinImplicitFunction.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    namespace implicitFunctions
    {
        defineTypeNameAndDebug(sinImplicitFunction, 0);
        addToRunTimeSelectionTable
        (
            implicitFunction,
            sinImplicitFunction,
            dict
        );
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::implicitFunctions::sinImplicitFunction::sinImplicitFunction
(
    const scalar period,
    const scalar phase,
    const scalar amplitude,
    const vector& direction,
    const vector& up,
    const vector& origin
)
:
    period_(period),
    phase_(phase),
    amplitude_(amplitude),
    up_(normalised(up)),
    direction_(normalised(direction)),
    origin_(origin)
{}


Foam::implicitFunctions::sinImplicitFunction::sinImplicitFunction
(
    const dictionary& dict
)
:
    // __INTEL_COMPILER bug with inheriting constructors?? (issue #1821)
    period_(dict.get<scalar>("period")),
    phase_(dict.getOrDefault<scalar>("phase", 0)),
    amplitude_(dict.get<scalar>("amplitude")),
    up_(dict.get<vector>("up").normalise()),
    direction_(dict.get<vector>("direction").normalise()),
    origin_(dict.get<vector>("origin"))
{}


// ************************************************************************* //
