/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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

#include "XiModels/fixed/fixed.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace XiModels
{
    defineTypeNameAndDebug(fixed, 0);
    addToRunTimeSelectionTable(XiModel, fixed, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::XiModels::fixed::fixed
(
    const dictionary& XiProperties,
    const psiuReactionThermo& thermo,
    const compressible::RASModel& turbulence,
    const volScalarField& Su,
    const volScalarField& rho,
    const volScalarField& b,
    const surfaceScalarField& phi
)
:
    XiModel(XiProperties, thermo, turbulence, Su, rho, b, phi)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::XiModels::fixed::~fixed()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

bool Foam::XiModels::fixed::read(const dictionary& XiProperties)
{
    return XiModel::read(XiProperties);
}


// ************************************************************************* //
