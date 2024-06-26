/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "fields/fvPatchFields/derived/rotatingTotalPressure/rotatingTotalPressureFvPatchScalarField.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "fields/fvPatchFields/fvPatchField/fvPatchFieldMapper.H"
#include "fields/volFields/volFields.H"
#include "fields/surfaceFields/surfaceFields.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::rotatingTotalPressureFvPatchScalarField::
rotatingTotalPressureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    totalPressureFvPatchScalarField(p, iF),
    omega_()
{}


Foam::rotatingTotalPressureFvPatchScalarField::
rotatingTotalPressureFvPatchScalarField
(
    const rotatingTotalPressureFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    totalPressureFvPatchScalarField(ptf, p, iF, mapper),
    omega_(ptf.omega_.clone())
{}


Foam::rotatingTotalPressureFvPatchScalarField::
rotatingTotalPressureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    totalPressureFvPatchScalarField(p, iF, dict),
    omega_(Function1<vector>::New("omega", dict, &db()))
{}


Foam::rotatingTotalPressureFvPatchScalarField::
rotatingTotalPressureFvPatchScalarField
(
    const rotatingTotalPressureFvPatchScalarField& rtppsf
)
:
    totalPressureFvPatchScalarField(rtppsf),
    omega_(rtppsf.omega_.clone())
{}


Foam::rotatingTotalPressureFvPatchScalarField::
rotatingTotalPressureFvPatchScalarField
(
    const rotatingTotalPressureFvPatchScalarField& rtppsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    totalPressureFvPatchScalarField(rtppsf, iF),
    omega_(rtppsf.omega_.clone())
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::rotatingTotalPressureFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    const scalar t = this->db().time().timeOutputValue();
    const vector om = omega_->value(t);

    vector axisHat = om/mag(om);
    tmp<vectorField> rotationVelocity =
        om ^ (patch().Cf() - axisHat*(axisHat & patch().Cf()));

    const vectorField Up
    (
        patch().lookupPatchField<volVectorField>(UName())
      + rotationVelocity
    );

    totalPressureFvPatchScalarField::updateCoeffs(p0(), Up);
}


void Foam::rotatingTotalPressureFvPatchScalarField::write(Ostream& os) const
{
    totalPressureFvPatchScalarField::write(os);
    omega_->writeData(os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        rotatingTotalPressureFvPatchScalarField
    );
}

// ************************************************************************* //
