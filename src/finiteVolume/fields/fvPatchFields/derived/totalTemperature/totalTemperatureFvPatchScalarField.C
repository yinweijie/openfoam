/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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

#include "fields/fvPatchFields/derived/totalTemperature/totalTemperatureFvPatchScalarField.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "fields/fvPatchFields/fvPatchField/fvPatchFieldMapper.H"
#include "fields/volFields/volFields.H"
#include "fields/surfaceFields/surfaceFields.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::totalTemperatureFvPatchScalarField::totalTemperatureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(p, iF),
    UName_("U"),
    phiName_("phi"),
    psiName_("thermo:psi"),
    gamma_(0.0),
    T0_(p.size(), Zero)
{}


Foam::totalTemperatureFvPatchScalarField::totalTemperatureFvPatchScalarField
(
    const totalTemperatureFvPatchScalarField& ptf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchScalarField(ptf, p, iF, mapper),
    UName_(ptf.UName_),
    phiName_(ptf.phiName_),
    psiName_(ptf.psiName_),
    gamma_(ptf.gamma_),
    T0_(ptf.T0_, mapper)
{}


Foam::totalTemperatureFvPatchScalarField::totalTemperatureFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchScalarField(p, iF, dict, IOobjectOption::NO_READ),
    UName_(dict.getOrDefault<word>("U", "U")),
    phiName_(dict.getOrDefault<word>("phi", "phi")),
    psiName_(dict.getOrDefault<word>("psi", "thermo:psi")),
    gamma_(dict.get<scalar>("gamma")),
    T0_("T0", dict, p.size())
{
    if (!this->readValueEntry(dict))
    {
        fvPatchField<scalar>::operator=(T0_);
    }
}


Foam::totalTemperatureFvPatchScalarField::totalTemperatureFvPatchScalarField
(
    const totalTemperatureFvPatchScalarField& tppsf
)
:
    fixedValueFvPatchScalarField(tppsf),
    UName_(tppsf.UName_),
    phiName_(tppsf.phiName_),
    psiName_(tppsf.psiName_),
    gamma_(tppsf.gamma_),
    T0_(tppsf.T0_)
{}


Foam::totalTemperatureFvPatchScalarField::totalTemperatureFvPatchScalarField
(
    const totalTemperatureFvPatchScalarField& tppsf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(tppsf, iF),
    UName_(tppsf.UName_),
    phiName_(tppsf.phiName_),
    psiName_(tppsf.psiName_),
    gamma_(tppsf.gamma_),
    T0_(tppsf.T0_)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::totalTemperatureFvPatchScalarField::autoMap
(
    const fvPatchFieldMapper& m
)
{
    fixedValueFvPatchScalarField::autoMap(m);
    T0_.autoMap(m);
}


void Foam::totalTemperatureFvPatchScalarField::rmap
(
    const fvPatchScalarField& ptf,
    const labelList& addr
)
{
    fixedValueFvPatchScalarField::rmap(ptf, addr);

    const totalTemperatureFvPatchScalarField& tiptf =
        refCast<const totalTemperatureFvPatchScalarField>(ptf);

    T0_.rmap(tiptf.T0_, addr);
}


void Foam::totalTemperatureFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    const auto& Up =
        patch().lookupPatchField<volVectorField>(UName_);

    const auto& phip =
        patch().lookupPatchField<surfaceScalarField>(phiName_);

    const auto& psip =
        patch().lookupPatchField<volScalarField>(psiName_);

    scalar gM1ByG = (gamma_ - 1.0)/gamma_;

    operator==
    (
        T0_/(1.0 + 0.5*psip*gM1ByG*(neg(phip))*magSqr(Up))
    );

    fixedValueFvPatchScalarField::updateCoeffs();
}


void Foam::totalTemperatureFvPatchScalarField::write(Ostream& os) const
{
    fvPatchField<scalar>::write(os);
    os.writeEntryIfDifferent<word>("U", "U", UName_);
    os.writeEntryIfDifferent<word>("phi", "phi", phiName_);
    os.writeEntryIfDifferent<word>("psi", "thermo:psi", psiName_);
    os.writeEntry("gamma", gamma_);
    T0_.writeEntry("T0", os);
    fvPatchField<scalar>::writeValueEntry(os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        totalTemperatureFvPatchScalarField
    );
}

// ************************************************************************* //
