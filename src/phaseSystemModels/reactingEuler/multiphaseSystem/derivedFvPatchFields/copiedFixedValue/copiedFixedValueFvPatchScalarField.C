/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2019 OpenFOAM Foundation
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

#include "derivedFvPatchFields/copiedFixedValue/copiedFixedValueFvPatchScalarField.H"
#include "fields/fvPatchFields/fvPatchField/fvPatchFieldMapper.H"
#include "fields/volFields/volFields.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::copiedFixedValueFvPatchScalarField::copiedFixedValueFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(p, iF),
    sourceFieldName_("default")
{}


Foam::copiedFixedValueFvPatchScalarField::copiedFixedValueFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchScalarField(p, iF, dict),
    sourceFieldName_(dict.lookup("sourceFieldName"))
{}


Foam::copiedFixedValueFvPatchScalarField::copiedFixedValueFvPatchScalarField
(
    const copiedFixedValueFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchScalarField(ptf, p, iF, mapper),
    sourceFieldName_(ptf.sourceFieldName_)
{}


Foam::copiedFixedValueFvPatchScalarField::copiedFixedValueFvPatchScalarField
(
    const copiedFixedValueFvPatchScalarField& awfpsf
)
:
    fixedValueFvPatchScalarField(awfpsf),
    sourceFieldName_(awfpsf.sourceFieldName_)
{}


Foam::copiedFixedValueFvPatchScalarField::copiedFixedValueFvPatchScalarField
(
    const copiedFixedValueFvPatchScalarField& awfpsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(awfpsf, iF),
    sourceFieldName_(awfpsf.sourceFieldName_)
{}



// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::copiedFixedValueFvPatchScalarField::updateCoeffs()
{
    if (this->updated())
    {
        return;
    }

    operator==
    (
        patch().lookupPatchField<volScalarField>(sourceFieldName_)
    );

    fixedValueFvPatchScalarField::updateCoeffs();
}


void Foam::copiedFixedValueFvPatchScalarField::write(Ostream& os) const
{
    fvPatchField<scalar>::write(os);
    os.writeEntry("sourceFieldName", sourceFieldName_);
    fvPatchField<scalar>::writeValueEntry(os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        copiedFixedValueFvPatchScalarField
    );
}


// ************************************************************************* //
