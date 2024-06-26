/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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
    Foam::constant::physicoChemical

Description
    Physico-chemical constants

\*---------------------------------------------------------------------------*/

#ifndef physicoChemicalConstants_H
#define physicoChemicalConstants_H

#include "dimensionedTypes/dimensionedScalar/dimensionedScalar.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace constant
{
namespace physicoChemical
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    //- Group name for physico-chemical constants
    constexpr const char* const group = "physicoChemical";

    //- Universal gas constant: default SI units: [J/mol/K]
    extern const dimensionedScalar R;

    //- Faraday constant: default SI units: [C/mol]
    extern const dimensionedScalar F;

    //- Stefan-Boltzmann constant: default SI units: [W/m2/K4]
    extern const dimensionedScalar sigma;

    //- Wien displacement law constant: default SI units: [m.K]
    extern const dimensionedScalar b;

    //- First radiation constant: default SI units: [W/m2]
    extern const dimensionedScalar c1;

    //- Second radiation constant: default SI units: [m.K]
    extern const dimensionedScalar c2;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace physicoChemical
} // End namespace constant
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
