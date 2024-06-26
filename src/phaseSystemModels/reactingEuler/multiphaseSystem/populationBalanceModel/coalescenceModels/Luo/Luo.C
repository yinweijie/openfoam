/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2019 OpenFOAM Foundation
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

#include "populationBalanceModel/coalescenceModels/Luo/Luo.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "global/constants/mathematical/mathematicalConstants.H"
#include "turbulence/phaseCompressibleTurbulenceModel.H"
#include "interfacialModels/virtualMassModels/virtualMassModel/virtualMassModel.H"
#include "phaseSystem/phaseSystem.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace diameterModels
{
namespace coalescenceModels
{
    defineTypeNameAndDebug(Luo, 0);
    addToRunTimeSelectionTable
    (
        coalescenceModel,
        Luo,
        dictionary
    );
}
}
}

using Foam::constant::mathematical::pi;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::diameterModels::coalescenceModels::Luo::
Luo
(
    const populationBalanceModel& popBal,
    const dictionary& dict
)
:
    coalescenceModel(popBal, dict),
    beta_(dimensionedScalar::getOrDefault("beta", dict, dimless, 2.05)),
    C1_(dimensionedScalar::getOrDefault("C1", dict, dimless, 1.0))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::diameterModels::coalescenceModels::Luo::
addToCoalescenceRate
(
    volScalarField& coalescenceRate,
    const label i,
    const label j
)
{
    const sizeGroup& fi = popBal_.sizeGroups()[i];
    const sizeGroup& fj = popBal_.sizeGroups()[j];
    const phaseModel& continuousPhase = popBal_.continuousPhase();

    if
    (
        popBal_.fluid().foundSubModel<virtualMassModel>
        (
            fi.phase(),
            popBal_.continuousPhase()
        )
    )
    {
        const virtualMassModel& vm =
            popBal_.fluid().lookupSubModel<virtualMassModel>
            (
                fi.phase(),
                popBal_.continuousPhase()
            );

        const dimensionedScalar xi = fi.d()/fj.d();

        const volScalarField uij
        (
            sqrt(beta_)
           *cbrt(popBal_.continuousTurbulence().epsilon()*fi.d())
           *sqrt(1.0 + pow(xi, -2.0/3.0))
        );

        coalescenceRate +=
            pi/4.0*sqr(fi.d() + fj.d())*uij
           *exp
            (
              - C1_
               *sqrt(0.75*(1.0 + sqr(xi))*(1.0 + pow3(xi)))
               /(
                    sqrt(fi.phase().rho()/continuousPhase.rho()
                  + vm.Cvm())*pow3(1.0 + xi)
                )
               *sqrt
                (
                    continuousPhase.rho()*fi.d()*sqr(uij)
                   /popBal_.sigmaWithContinuousPhase(fi.phase())
                )
            );
    }
    else
    {
        FatalErrorInFunction
            << "A virtual mass model for " << fi.phase().name() << " in "
            << popBal_.continuousPhase().name() << " is not specified. This is "
            << "required by the Luo coalescence model." << exit(FatalError);
    }
}


// ************************************************************************* //
