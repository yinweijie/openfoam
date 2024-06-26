/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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

#include "energyScalingFunction/derived/sigmoid/sigmoid.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace energyScalingFunctions
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(sigmoid, 0);

addToRunTimeSelectionTable
(
    energyScalingFunction,
    sigmoid,
    dictionary
);


// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

scalar sigmoid::sigmoidScale
    (
        const scalar r,
        const scalar shift,
        const scalar scale
    ) const
{
    return 1.0 / (1.0 + exp( scale * (r - shift)));
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

sigmoid::sigmoid
(
    const word& name,
    const dictionary& energyScalingFunctionProperties,
    const pairPotential& pairPot
)
:
    energyScalingFunction(name, energyScalingFunctionProperties, pairPot),
    sigmoidCoeffs_
    (
        energyScalingFunctionProperties.subDict(typeName + "Coeffs")
    ),
    shift_(sigmoidCoeffs_.get<scalar>("shift")),
    scale_(sigmoidCoeffs_.get<scalar>("scale"))
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void sigmoid::scaleEnergy(scalar& e, const scalar r) const
{
    e *= sigmoidScale(r, shift_, scale_);
}


bool sigmoid::read(const dictionary& energyScalingFunctionProperties)
{
    energyScalingFunction::read(energyScalingFunctionProperties);

    sigmoidCoeffs_ =
        energyScalingFunctionProperties.subDict(typeName + "Coeffs");

    sigmoidCoeffs_.readEntry("shift", shift_);
    sigmoidCoeffs_.readEntry("scale", shift_);

    return true;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace energyScalingFunctions
} // End namespace Foam

// ************************************************************************* //
