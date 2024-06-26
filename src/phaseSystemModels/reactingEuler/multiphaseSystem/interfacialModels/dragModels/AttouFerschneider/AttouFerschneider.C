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

#include "interfacialModels/dragModels/AttouFerschneider/AttouFerschneider.H"
#include "phasePair/reactingEuler_phasePair.H"
#include "phaseSystem/phaseSystem.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace dragModels
{
    defineTypeNameAndDebug(AttouFerschneider, 0);
    addToRunTimeSelectionTable(dragModel, AttouFerschneider, dictionary);
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::dragModels::AttouFerschneider::KGasLiquid
(
    const phaseModel& gas,
    const phaseModel& liquid
) const
{
    const phaseModel& solid = gas.fluid().phases()[solidName_];

    const volScalarField oneMinusGas(max(1 - gas, liquid.residualAlpha()));
    const volScalarField cbrtR
    (
        cbrt(max(solid, solid.residualAlpha())/oneMinusGas)
    );
    const volScalarField magURel(mag(gas.U() - liquid.U()));

    return
        E2_*gas.mu()*sqr(oneMinusGas/solid.d())*sqr(cbrtR)
       /max(gas, gas.residualAlpha())
      + E2_*gas.rho()*magURel*(1 - gas)/solid.d()*cbrtR;
}


Foam::tmp<Foam::volScalarField>
Foam::dragModels::AttouFerschneider::KGasSolid
(
    const phaseModel& gas,
    const phaseModel& solid
) const
{
    const volScalarField oneMinusGas(max(1 - gas, solid.residualAlpha()));
    const volScalarField cbrtR
    (
        cbrt(max(solid, solid.residualAlpha())/oneMinusGas)
    );

    return
        E1_*gas.mu()*sqr(oneMinusGas/solid.d())*sqr(cbrtR)
       /max(gas, gas.residualAlpha())
      + E2_*gas.rho()*mag(gas.U())*(1 - gas)/solid.d()*cbrtR;
}


Foam::tmp<Foam::volScalarField>
Foam::dragModels::AttouFerschneider::KLiquidSolid
(
    const phaseModel& liquid,
    const phaseModel& solid
) const
{
    const phaseModel& gas = liquid.fluid().phases()[gasName_];

    return
        E1_*liquid.mu()*sqr(max(solid, solid.residualAlpha())/solid.d())
       /max(liquid, liquid.residualAlpha())
      + E2_*liquid.rho()*mag(gas.U())*solid/solid.d();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::dragModels::AttouFerschneider::AttouFerschneider
(
    const dictionary& dict,
    const phasePair& pair,
    const bool registerObject
)
:
    dragModel(dict, pair, registerObject),
    gasName_(dict.lookup("gas")),
    liquidName_(dict.lookup("liquid")),
    solidName_(dict.lookup("solid")),
    E1_("E1", dimless, dict),
    E2_("E2", dimless, dict)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::dragModels::AttouFerschneider::CdRe() const
{
    FatalErrorInFunction
        << "Not implemented."
        << "Drag coefficient is not defined for the AttouFerschneider model."
        << exit(FatalError);

    return nullptr;
}


Foam::tmp<Foam::volScalarField>
Foam::dragModels::AttouFerschneider::K() const
{
    switch (Pair<word>::compare(pair_, phasePairKey(gasName_, liquidName_)))
    {
        case 1:
            return KGasLiquid(pair_.phase1(), pair_.phase2());
        case -1:
            return KGasLiquid(pair_.phase2(), pair_.phase1());
    }

    switch (Pair<word>::compare(pair_, phasePairKey(gasName_, solidName_)))
    {
        case 1:
            return KGasSolid(pair_.phase1(), pair_.phase2());
        case -1:
            return KGasSolid(pair_.phase2(), pair_.phase1());
    }

    switch (Pair<word>::compare(pair_, phasePairKey(liquidName_, solidName_)))
    {
        case 1:
            return KLiquidSolid(pair_.phase1(), pair_.phase2());
        case -1:
            return KLiquidSolid(pair_.phase2(), pair_.phase1());
    }

    FatalErrorInFunction
        << "The pair does not contain two of out of the gas, liquid and solid "
        << "phase models."
        << exit(FatalError);

    return nullptr;
}


Foam::tmp<Foam::surfaceScalarField>
Foam::dragModels::AttouFerschneider::Kf() const
{
    return fvc::interpolate(K());
}


// ************************************************************************* //
