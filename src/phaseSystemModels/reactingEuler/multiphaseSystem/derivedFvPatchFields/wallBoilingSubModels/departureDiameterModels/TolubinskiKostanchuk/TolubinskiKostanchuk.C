/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2019 OpenFOAM Foundation
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

#include "derivedFvPatchFields/wallBoilingSubModels/departureDiameterModels/TolubinskiKostanchuk/TolubinskiKostanchuk.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace wallBoilingModels
{
namespace departureDiameterModels
{
    defineTypeNameAndDebug(TolubinskiKostanchuk, 0);
    addToRunTimeSelectionTable
    (
        departureDiameterModel,
        TolubinskiKostanchuk,
        dictionary
    );
}
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::wallBoilingModels::departureDiameterModels::
TolubinskiKostanchuk::TolubinskiKostanchuk
(
    const dictionary& dict
)
:
    departureDiameterModel(),
    dRef_(dict.getOrDefault<scalar>("dRef", 6e-4)),
    dMax_(dict.getOrDefault<scalar>("dMax", 0.0014)),
    dMin_(dict.getOrDefault<scalar>("dMin", 1e-6))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::scalarField>
Foam::wallBoilingModels::departureDiameterModels::
TolubinskiKostanchuk::dDeparture
(
    const phaseModel& liquid,
    const phaseModel& vapor,
    const label patchi,
    const scalarField& Tl,
    const scalarField& Tsatw,
    const scalarField& L
) const
{
    return clamp
    (
        dRef_*exp(-(Tsatw - Tl)/scalar(45)),
        scalarMinMax(dMin_, dMax_)
    );
}


void Foam::wallBoilingModels::departureDiameterModels::
TolubinskiKostanchuk::write(Ostream& os) const
{
    departureDiameterModel::write(os);
    os.writeEntry("dRef", dRef_);
    os.writeEntry("dMax", dMax_);
    os.writeEntry("dMin", dMin_);
}


// ************************************************************************* //
