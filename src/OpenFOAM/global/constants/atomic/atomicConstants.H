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

Namespace
    Foam::constant::atomic

Description
    Atomic constants

\*---------------------------------------------------------------------------*/

#ifndef atomicConstants_H
#define atomicConstants_H

#include "dimensionedTypes/dimensionedScalar/dimensionedScalar.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace constant
{
namespace atomic
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    //- Group name for atomic constants
    constexpr const char* const group = "atomic";

    //- Fine-structure constant: default SI units: []
    extern const dimensionedScalar alpha;

    //- Rydberg constant: default SI units: [1/m]
    extern const dimensionedScalar Rinf;

    //- Bohr radius: default SI units: [m]
    extern const dimensionedScalar a0;

    //- Classical electron radius: default SI units: [m]
    extern const dimensionedScalar re;

    //- Hartree energy: default SI units: [J]
    extern const dimensionedScalar Eh;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace atomic
} // End namespace constant
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
