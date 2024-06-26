/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 Wikki Ltd
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

#include "fields/faPatchFields/basic/basicSymmetry/basicSymmetryFaPatchField.H"
#include "fields/areaFields/areaFields.H"

// No run-time selection

// * * * * * * * * * * * * * * * Specialisations * * * * * * * * * * * * * * //

template<>
Foam::tmp<Foam::scalarField>
Foam::basicSymmetryFaPatchField<Foam::scalar>::snGrad() const
{
    return tmp<scalarField>::New(size(), Zero);
}


template<>
void Foam::basicSymmetryFaPatchField<Foam::scalar>::evaluate
(
    const Pstream::commsTypes
)
{
    if (!updated())
    {
        updateCoeffs();
    }

    scalarField::operator=(patchInternalField());
    transformFaPatchField<scalar>::evaluate();
}


// ************************************************************************* //
