/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2017 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "derivedFvPatchFields/semiPermeableBaffle/semiPermeableBaffleMassFraction/semiPermeableBaffleMassFractionFvPatchScalarField.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "fields/fvPatchFields/fvPatchField/fvPatchFieldMapper.H"
#include "fields/volFields/volFields.H"
#include "fields/surfaceFields/surfaceFields.H"
#include "turbulenceModel.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::semiPermeableBaffleMassFractionFvPatchScalarField::
semiPermeableBaffleMassFractionFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    mappedPatchBase(p.patch()),
    mixedFvPatchScalarField(p, iF),
    c_(0),
    phiName_("phi")
{
    refValue() = Zero;
    refGrad() = Zero;
    valueFraction() = Zero;
}


Foam::semiPermeableBaffleMassFractionFvPatchScalarField::
semiPermeableBaffleMassFractionFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    mappedPatchBase(p.patch(), NEARESTPATCHFACE, dict),
    mixedFvPatchScalarField(p, iF),
    c_(dict.getOrDefault<scalar>("c", 0)),
    phiName_(dict.getOrDefault<word>("phi", "phi"))
{
    this->readValueEntry(dict, IOobjectOption::MUST_READ);

    refValue() = Zero;
    refGrad() = Zero;
    valueFraction() = Zero;
}


Foam::semiPermeableBaffleMassFractionFvPatchScalarField::
semiPermeableBaffleMassFractionFvPatchScalarField
(
    const semiPermeableBaffleMassFractionFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    mappedPatchBase(p.patch(), ptf),
    mixedFvPatchScalarField(ptf, p, iF, mapper),
    c_(ptf.c_),
    phiName_(ptf.phiName_)
{}


Foam::semiPermeableBaffleMassFractionFvPatchScalarField::
semiPermeableBaffleMassFractionFvPatchScalarField
(
    const semiPermeableBaffleMassFractionFvPatchScalarField& ptf
)
:
    mappedPatchBase(ptf.patch().patch(), ptf),
    mixedFvPatchScalarField(ptf),
    c_(ptf.c_),
    phiName_(ptf.phiName_)
{}


Foam::semiPermeableBaffleMassFractionFvPatchScalarField::
semiPermeableBaffleMassFractionFvPatchScalarField
(
    const semiPermeableBaffleMassFractionFvPatchScalarField& ptf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    mappedPatchBase(ptf.patch().patch(), ptf),
    mixedFvPatchScalarField(ptf, iF),
    c_(ptf.c_),
    phiName_(ptf.phiName_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::scalarField>
Foam::semiPermeableBaffleMassFractionFvPatchScalarField::phiY() const
{
    if (c_ == scalar(0))
    {
        return tmp<scalarField>::New(patch().size(), Zero);
    }

    const word& YName = internalField().name();

    const label nbrPatchi = samplePolyPatch().index();
    const fvPatch& nbrPatch = patch().boundaryMesh()[nbrPatchi];

    const auto& nbrYp = nbrPatch.lookupPatchField<volScalarField>(YName);
    scalarField nbrYc(nbrYp.patchInternalField());
    mappedPatchBase::map().distribute(nbrYc);

    return c_*patch().magSf()*(patchInternalField() - nbrYc);
}


void Foam::semiPermeableBaffleMassFractionFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    const scalarField& phip =
        patch().lookupPatchField<surfaceScalarField>(phiName_);

    const turbulenceModel& turbModel =
        db().lookupObject<turbulenceModel>
        (
            turbulenceModel::propertiesName
        );
    const scalarField muEffp(turbModel.muEff(patch().index()));
    const scalarField AMuEffp(patch().magSf()*muEffp);

    valueFraction() = phip/(phip - patch().deltaCoeffs()*AMuEffp);
    refGrad() = - phiY()/AMuEffp;

    mixedFvPatchScalarField::updateCoeffs();
}


void Foam::semiPermeableBaffleMassFractionFvPatchScalarField::write
(
    Ostream& os
) const
{
    fvPatchField<scalar>::write(os);
    mappedPatchBase::write(os);
    os.writeEntryIfDifferent<scalar>("c", 0, c_);
    os.writeEntryIfDifferent<word>("phi", "phi", phiName_);
    fvPatchField<scalar>::writeValueEntry(os);
}


// * * * * * * * * * * * * * * Build Macro Function  * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        semiPermeableBaffleMassFractionFvPatchScalarField
    );
}

// ************************************************************************* //
