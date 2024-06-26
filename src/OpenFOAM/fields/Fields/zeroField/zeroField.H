/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
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

Class
    Foam::zeroField

Description
    A class representing the concept of a field of 0 used to avoid unnecessary
    manipulations for objects which are known to be zero at compile-time.

    Used for example as the argument to a function in which certain terms are
    optional, see source terms in the MULES solvers.

\*---------------------------------------------------------------------------*/

#ifndef Foam_zeroField_H
#define Foam_zeroField_H

#include "primitives/traits/zero.H"
#include "primitives/Scalar/scalar/scalarFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class zeroField Declaration
\*---------------------------------------------------------------------------*/

class zeroField
:
    public zero
{
public:

    // Constructors

        //- Default construct
        zeroField() noexcept = default;


    // Member Functions

        //- Method name compatibility with DimensionedField
        zeroField field() const noexcept
        {
            return zeroField{};
        }


    // Member Operators

        scalar operator[](const label) const noexcept
        {
            return scalar(0);
        }

        zeroField operator()() const noexcept
        {
            return zeroField{};
        }

        zeroField operator-() const noexcept
        {
            return zeroField{};
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Global Operators

#include "fields/Fields/zeroField/zeroFieldI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
