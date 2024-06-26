/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2014-2017 OpenFOAM Foundation
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

#include "interfacialModels/dragModels/IshiiZuber/IshiiZuber.H"
#include "phasePair/phasePair.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace dragModels
{
    defineTypeNameAndDebug(IshiiZuber, 0);
    addToRunTimeSelectionTable(dragModel, IshiiZuber, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::dragModels::IshiiZuber::IshiiZuber
(
    const dictionary& dict,
    const phasePair& pair,
    const bool registerObject
)
:
    dragModel(dict, pair, registerObject)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::dragModels::IshiiZuber::~IshiiZuber()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::dragModels::IshiiZuber::CdRe() const
{
    volScalarField Re(pair_.Re());
    volScalarField Eo(pair_.Eo());

    volScalarField mud(pair_.dispersed().mu());
    volScalarField muc(pair_.continuous().mu());

    volScalarField muStar((mud + 0.4*muc)/(mud + muc));

    volScalarField muMix
    (
        muc
       *pow(max(1 - pair_.dispersed(), scalar(1e-3)), -2.5*muStar)
    );

    volScalarField ReM(Re*muc/muMix);
    volScalarField CdRe
    (
        pos0(1000 - ReM)*24.0*(scalar(1) + 0.15*pow(ReM, 0.687))
      + neg(1000 - ReM)*0.44*ReM
    );

    volScalarField F((muc/muMix)*sqrt(1 - pair_.dispersed()));
    F.clamp_min(1e-3);

    volScalarField Ealpha((1 + 17.67*pow(F, 0.8571428))/(18.67*F));

    volScalarField CdReEllipse(Ealpha*0.6666*sqrt(Eo)*Re);

    return
        pos0(CdReEllipse - CdRe)
       *min(CdReEllipse, Re*sqr(1 - pair_.dispersed())*2.66667)
      + neg(CdReEllipse - CdRe)*CdRe;
}


// ************************************************************************* //
