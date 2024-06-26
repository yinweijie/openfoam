/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018 OpenFOAM Foundation
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

#include "fields/fvPatchFields/derived/freestreamVelocity/freestreamVelocityFvPatchVectorField.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::freestreamVelocityFvPatchVectorField::freestreamVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF
)
:
    mixedFvPatchVectorField(p, iF)
{}


Foam::freestreamVelocityFvPatchVectorField::freestreamVelocityFvPatchVectorField
(
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const dictionary& dict
)
:
    mixedFvPatchVectorField(p, iF)
{
    // freestreamValue() and refValue() are identical
    freestreamValue().assign("freestreamValue", dict, p.size());
    refGrad() = Zero;
    valueFraction() = 1;

    if (!this->readValueEntry(dict))
    {
        fvPatchVectorField::operator=(freestreamValue());
    }
}


Foam::freestreamVelocityFvPatchVectorField::freestreamVelocityFvPatchVectorField
(
    const freestreamVelocityFvPatchVectorField& ptf,
    const fvPatch& p,
    const DimensionedField<vector, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    mixedFvPatchVectorField(ptf, p, iF, mapper)
{}


Foam::freestreamVelocityFvPatchVectorField::freestreamVelocityFvPatchVectorField
(
    const freestreamVelocityFvPatchVectorField& wbppsf
)
:
    mixedFvPatchVectorField(wbppsf)
{}


Foam::freestreamVelocityFvPatchVectorField::freestreamVelocityFvPatchVectorField
(
    const freestreamVelocityFvPatchVectorField& wbppsf,
    const DimensionedField<vector, volMesh>& iF
)
:
    mixedFvPatchVectorField(wbppsf, iF)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::freestreamVelocityFvPatchVectorField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    const Field<vector>& Up = *this;

    valueFraction() = 0.5 - 0.5*(Up & patch().nf())/mag(Up);

    mixedFvPatchField<vector>::updateCoeffs();
}


void Foam::freestreamVelocityFvPatchVectorField::write(Ostream& os) const
{
    fvPatchField<vector>::write(os);
    freestreamValue().writeEntry("freestreamValue", os);
    fvPatchField<vector>::writeValueEntry(os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchVectorField,
        freestreamVelocityFvPatchVectorField
    );
}

// ************************************************************************* //
