/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018-2019 OpenFOAM Foundation
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

#include "populationBalanceModel/coalescenceModels/PrinceBlanch/PrinceBlanch.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "global/constants/mathematical/mathematicalConstants.H"
#include "turbulence/phaseCompressibleTurbulenceModel.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace diameterModels
{
namespace coalescenceModels
{
    defineTypeNameAndDebug(PrinceBlanch, 0);
    addToRunTimeSelectionTable
    (
        coalescenceModel,
        PrinceBlanch,
        dictionary
    );
}
}
}

using Foam::constant::mathematical::pi;

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::diameterModels::coalescenceModels::PrinceBlanch::
PrinceBlanch
(
    const populationBalanceModel& popBal,
    const dictionary& dict
)
:
    coalescenceModel(popBal, dict),
    C1_(dimensionedScalar::getOrDefault("C1", dict, dimless, 0.356)),
    h0_
    (
        dimensionedScalar::getOrDefault
        (
            "h0",
            dict,
            dimLength,
            1e-4
        )
    ),
    hf_
    (
        dimensionedScalar::getOrDefault
        (
            "hf",
            dict,
            dimLength,
            1e-8
        )
    ),
    turbulence_(dict.lookup("turbulence")),
    buoyancy_(dict.lookup("buoyancy")),
    laminarShear_(dict.lookup("laminarShear"))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::diameterModels::coalescenceModels::PrinceBlanch::
addToCoalescenceRate
(
    volScalarField& coalescenceRate,
    const label i,
    const label j
)
{
    const phaseModel& continuousPhase = popBal_.continuousPhase();
    const sizeGroup& fi = popBal_.sizeGroups()[i];
    const sizeGroup& fj = popBal_.sizeGroups()[j];
    const uniformDimensionedVectorField& g =
        popBal_.mesh().time().lookupObject<uniformDimensionedVectorField>("g");

    const dimensionedScalar rij(1.0/(1.0/fi.d() + 1.0/fj.d()));

    const volScalarField collisionEfficiency
    (
        exp
        (
          - sqrt
            (
                pow3(rij)*continuousPhase.rho()
               /(16.0*popBal_.sigmaWithContinuousPhase(fi.phase()))
            )
           *log(h0_/hf_)
           *cbrt(popBal_.continuousTurbulence().epsilon())/pow(rij, 2.0/3.0)
        )
    );

    if (turbulence_)
    {
        coalescenceRate +=
            (
                C1_*pi*sqr(fi.d() + fj.d())
               *cbrt(popBal_.continuousTurbulence().epsilon())
               *sqrt(pow(fi.d(), 2.0/3.0) + pow(fj.d(), 2.0/3.0))
            )
           *collisionEfficiency;
    }

    if (buoyancy_)
    {
        const dimensionedScalar Sij(pi/4.0*sqr(fi.d() + fj.d()));

        coalescenceRate +=
            (
                Sij
               *mag
                (
                    sqrt
                    (
                        2.14*popBal_.sigmaWithContinuousPhase(fi.phase())
                       /(continuousPhase.rho()*fi.d()) + 0.505*mag(g)*fi.d()
                    )
                  - sqrt
                    (
                        2.14*popBal_.sigmaWithContinuousPhase(fi.phase())
                       /(continuousPhase.rho()*fj.d()) + 0.505*mag(g)*fj.d()
                    )
                )
            )
           *collisionEfficiency;
    }

    if (laminarShear_)
    {
        FatalErrorInFunction
            << "Laminar shear collision contribution not implemented for "
            << this->type() << " coalescence model."
            << exit(FatalError);
    }
}


// ************************************************************************* //
