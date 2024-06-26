/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2019 OpenFOAM Foundation
    Copyright (C) 2018-2020 OpenCFD Ltd.
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

#include "derivedFvPatchFields/wallBoilingSubModels/departureDiameterModels/KocamustafaogullariIshii/KocamustafaogullariIshii.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "fields/UniformDimensionedFields/uniformDimensionedFields.H"
#include "compressibleTurbulenceModel.H"
#include "ThermalDiffusivity/ThermalDiffusivity.H"
#include "TurbulenceModels/phaseCompressible/PhaseCompressibleTurbulenceModel/PhaseCompressibleTurbulenceModelPascal.H"
#include "phaseSystem/phaseSystem.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace wallBoilingModels
{
namespace departureDiameterModels
{
    defineTypeNameAndDebug(KocamustafaogullariIshii, 0);
    addToRunTimeSelectionTable
    (
        departureDiameterModel,
        KocamustafaogullariIshii,
        dictionary
    );
}
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::wallBoilingModels::departureDiameterModels::
KocamustafaogullariIshii::KocamustafaogullariIshii
(
    const dictionary& dict
)
:
    departureDiameterModel(),
    phi_(dict.get<scalar>("phi"))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::scalarField>
Foam::wallBoilingModels::departureDiameterModels::
KocamustafaogullariIshii::dDeparture
(
    const phaseModel& liquid,
    const phaseModel& vapor,
    const label patchi,
    const scalarField& Tl,
    const scalarField& Tsatw,
    const scalarField& L
) const
{
    // Gravitational acceleration
    const auto& g =
        liquid.mesh().time().lookupObject<uniformDimensionedVectorField>("g");

    const scalarField rhoLiquid(liquid.thermo().rho(patchi));
    const scalarField rhoVapor(vapor.thermo().rho(patchi));

    const scalarField rhoM((rhoLiquid - rhoVapor)/rhoVapor);

    const tmp<volScalarField>& tsigma =
        liquid.fluid().sigma(phasePairKey(liquid.name(), vapor.name()));

    const volScalarField& sigma = tsigma();
    const fvPatchScalarField& sigmaw = sigma.boundaryField()[patchi];

    return
        0.0012*pow(rhoM, 0.9)*0.0208*phi_
       *sqrt(sigmaw/(mag(g.value())*(rhoLiquid - rhoVapor)));
}


void Foam::wallBoilingModels::departureDiameterModels::
KocamustafaogullariIshii::write(Ostream& os) const
{
    departureDiameterModel::write(os);
    os.writeEntry("phi", phi_);
}


// ************************************************************************* //
