/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2013 OpenFOAM Foundation
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

#include "global/constants/mathematical/mathematicalConstants.H"
#include "global/constants/universal/universalConstants.H"
#include "global/constants/electromagnetic/electromagneticConstants.H"
#include "global/constants/physicoChemical/physicoChemicalConstants.H"

#include "global/constants/dimensionedConstants.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace constant
{

defineDimensionedConstantWithDefault
(
    physicoChemical::group,
    physicoChemical::R,
    dimensionedScalar
    (
        "R",
        physicoChemical::NA*physicoChemical::k
    ),
    constantphysicoChemicalR,
    "R"
);


defineDimensionedConstantWithDefault
(
    physicoChemical::group,
    physicoChemical::F,
    dimensionedScalar
    (
        "F",
        physicoChemical::NA*electromagnetic::e
    ),
    constantphysicoChemicalF,
    "F"
);


// Note: cannot use dimless etc. since not guaranteed to be constructed
defineDimensionedConstantWithDefault
(
    physicoChemical::group,
    physicoChemical::sigma,
    dimensionedScalar
    (
        "sigma",
        Foam::dimensionedScalar
        (
            "C",
            dimensionSet(0, 0, 0, 0, 0),    //Foam::dimless,
            Foam::sqr(mathematical::pi)/60.0
        )
       *Foam::pow4(physicoChemical::k)
       /(pow3(universal::hr)*sqr(universal::c))
    ),
    constantphysicoChemicalsigma,
    "sigma"
);


defineDimensionedConstantWithDefault
(
    physicoChemical::group,
    physicoChemical::b,
    dimensionedScalar
    (
        "b",
        (universal::h*universal::c/physicoChemical::k)
       /Foam::dimensionedScalar
        (
            "C",
            dimensionSet(0, 0, 0, 0, 0),    //Foam::dimless
            4.965114231
        )
    ),
    constantphysicoChemicalb,
    "b"
);


defineDimensionedConstantWithDefault
(
    physicoChemical::group,
    physicoChemical::c1,
    dimensionedScalar
    (
        "c1",
        Foam::dimensionedScalar
        (
            "C",
            dimensionSet(0, 0, 0, 0, 0),    //Foam::dimless,
            mathematical::twoPi
        )
       *universal::h*Foam::sqr(universal::c)
    ),
    constantphysicoChemicalc1,
    "c1"
);


defineDimensionedConstantWithDefault
(
    physicoChemical::group,
    physicoChemical::c2,
    dimensionedScalar
    (
        "c2",
        universal::h*universal::c/physicoChemical::k
    ),
    constantphysicoChemicalc2,
    "c2"
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace constant
} // End namespace Foam

// ************************************************************************* //
