/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2020 OpenFOAM Foundation
    Copyright (C) 2020-2021 OpenCFD Ltd.
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

#include "derivedFvPatchFields/fixedMultiPhaseHeatFlux/fixedMultiPhaseHeatFluxFvPatchScalarField.H"
#include "fields/fvPatchFields/fvPatchField/fvPatchFieldMapper.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

#include "phaseSystem/phaseSystem.H"
#include "compressibleTurbulenceModel.H"
#include "ThermalDiffusivity/ThermalDiffusivity.H"
#include "TurbulenceModels/phaseCompressible/PhaseCompressibleTurbulenceModel/PhaseCompressibleTurbulenceModelPascal.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fixedMultiPhaseHeatFluxFvPatchScalarField::
fixedMultiPhaseHeatFluxFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(p, iF),
    q_(p.size(), Zero),
    relax_(1),
    Tmin_(273)
{}


Foam::fixedMultiPhaseHeatFluxFvPatchScalarField::
fixedMultiPhaseHeatFluxFvPatchScalarField
(
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const dictionary& dict
)
:
    fixedValueFvPatchScalarField(p, iF, dict),
    q_("q", dict, p.size()),
    relax_(dict.getOrDefault<scalar>("relax", 1)),
    Tmin_(dict.getOrDefault<scalar>("Tmin", 273))
{}


Foam::fixedMultiPhaseHeatFluxFvPatchScalarField::
fixedMultiPhaseHeatFluxFvPatchScalarField
(
    const fixedMultiPhaseHeatFluxFvPatchScalarField& psf,
    const fvPatch& p,
    const DimensionedField<scalar, volMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fixedValueFvPatchScalarField(psf, p, iF, mapper),
    q_(psf.q_, mapper),
    relax_(psf.relax_),
    Tmin_(psf.Tmin_)
{}


Foam::fixedMultiPhaseHeatFluxFvPatchScalarField::
fixedMultiPhaseHeatFluxFvPatchScalarField
(
    const fixedMultiPhaseHeatFluxFvPatchScalarField& psf
)
:
    fixedValueFvPatchScalarField(psf),
    q_(psf.q_),
    relax_(psf.relax_),
    Tmin_(psf.Tmin_)
{}


Foam::fixedMultiPhaseHeatFluxFvPatchScalarField::
fixedMultiPhaseHeatFluxFvPatchScalarField
(
    const fixedMultiPhaseHeatFluxFvPatchScalarField& psf,
    const DimensionedField<scalar, volMesh>& iF
)
:
    fixedValueFvPatchScalarField(psf, iF),
    q_(psf.q_),
    relax_(psf.relax_),
    Tmin_(psf.Tmin_)
{}



// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::fixedMultiPhaseHeatFluxFvPatchScalarField::updateCoeffs()
{
    if (updated())
    {
        return;
    }

    // Lookup the fluid model
    const phaseSystem& fluid =
        db().lookupObject<phaseSystem>("phaseProperties");

    const scalarField& Tp = *this;

    scalarField A(Tp.size(), Zero);
    scalarField B(Tp.size(), Zero);
    scalarField Q(Tp.size(), Zero);

    forAll(fluid.phases(), phasei)
    {
        const phaseModel& phase = fluid.phases()[phasei];
        const fluidThermo& thermo = phase.thermo();

        const fvPatchScalarField& alpha =
            phase.boundaryField()[patch().index()];

        const fvPatchScalarField& T =
            thermo.T().boundaryField()[patch().index()];

        const scalarField kappaEff(phase.kappaEff(patch().index()));

        if (debug)
        {
            const scalarField q0(T.snGrad()*alpha*kappaEff);
            Q += q0;

            Info<< patch().name() << " " << phase.name()
                << ": Heat flux " << gMin(q0) << " - " << gMax(q0) << endl;
        }

        A += T.patchInternalField()*alpha*kappaEff*patch().deltaCoeffs();
        B += alpha*kappaEff*patch().deltaCoeffs();
    }

    if (debug)
    {
        Info<< patch().name() << " " << ": overall heat flux "
            << gMin(Q) << " - " << gMax(Q) << " W/m2, power: "
            << gSum(patch().magSf()*Q) << " W" << endl;
    }

    operator==((scalar(1) - relax_)*Tp + relax_*max(Tmin_,(q_ + A)/(B)));

    fixedValueFvPatchScalarField::updateCoeffs();
}


void Foam::fixedMultiPhaseHeatFluxFvPatchScalarField::autoMap
(
    const fvPatchFieldMapper& m
)
{
    fixedValueFvPatchScalarField::autoMap(m);
    m(q_);
}


void Foam::fixedMultiPhaseHeatFluxFvPatchScalarField::rmap
(
    const fvPatchScalarField& ptf,
    const labelList& addr
)
{
    fixedValueFvPatchScalarField::rmap(ptf, addr);

    const fixedMultiPhaseHeatFluxFvPatchScalarField& mptf =
        refCast<const fixedMultiPhaseHeatFluxFvPatchScalarField>(ptf);

    q_.rmap(mptf.q_, addr);
}


void Foam::fixedMultiPhaseHeatFluxFvPatchScalarField::write(Ostream& os) const
{
    fvPatchField<scalar>::write(os);
    os.writeEntry("relax", relax_);
    q_.writeEntry("q", os);
    fvPatchField<scalar>::writeValueEntry(os);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    makePatchTypeField
    (
        fvPatchScalarField,
        fixedMultiPhaseHeatFluxFvPatchScalarField
    );
}


// ************************************************************************* //
