/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2018 OpenFOAM Foundation
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

#include "diameterModels/IATE/IATEsources/phaseChange/phaseChange.H"
#include "twoPhaseSystem.H"
#include "phaseSystem/phaseSystem.H"
#include "PhaseSystems/ThermalPhaseChangePhaseSystem/ThermalPhaseChangePhaseSystem.H"
#include "PhaseSystems/MomentumTransferPhaseSystem/MomentumTransferPhaseSystem.H"
#include "finiteVolume/fvm/fvmSup.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace diameterModels
{
namespace IATEsources
{
    defineTypeNameAndDebug(phaseChange, 0);
    addToRunTimeSelectionTable(IATEsource, phaseChange, dictionary);
}
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::diameterModels::IATEsources::phaseChange::phaseChange
(
    const IATE& iate,
    const dictionary& dict
)
:
    IATEsource(iate),
    pairName_(dict.lookup("pairName")),
    iDmdtPtr_(nullptr)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::fvScalarMatrix>
Foam::diameterModels::IATEsources::phaseChange::R
(
    const volScalarField& alphai,
    volScalarField& kappai
) const
{
    if (!iDmdtPtr_)
    {
        iDmdtPtr_ = &alphai.mesh().lookupObject<volScalarField>
        (
            IOobject::groupName("iDmdt", pairName_)
        );
    }

    const volScalarField& iDmdt = *iDmdtPtr_;

    return -fvm::SuSp
    (
        (1.0/3.0)
       *iDmdt()
       /(alphai()*phase().rho()()),
        kappai
    );
}


// ************************************************************************* //
