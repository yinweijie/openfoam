/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2016 OpenFOAM Foundation
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

#include "submodels/MPPIC/ParticleStressModels/Lun/Lun.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace ParticleStressModels
{
    defineTypeNameAndDebug(Lun, 0);

    addToRunTimeSelectionTable
    (
        ParticleStressModel,
        Lun,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::ParticleStressModels::Lun::Lun
(
    const dictionary& dict
)
:
    ParticleStressModel(dict),
    e_(dict.get<scalar>("e")),
    eps_(dict.get<scalar>("eps"))
{}


Foam::ParticleStressModels::Lun::Lun
(
    const Lun& ln
)
:
    ParticleStressModel(ln),
    e_(ln.e_),
    eps_(ln.eps_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::ParticleStressModels::Lun::~Lun()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::Field<Foam::scalar>>
Foam::ParticleStressModels::Lun::tau
(
    const Field<scalar>& alpha,
    const Field<scalar>& rho,
    const Field<scalar>& uSqr
) const
{
    tmp<Field<scalar>> g0
    (
        0.6
      / max
        (
            1.0 - cbrt(alpha/alphaPacked_),
            max(eps_*(1.0 - alpha), SMALL)
        )
    );

    tmp<Field<scalar>> gT(uSqr/3.0);

    return alpha*rho*(1.0 + alpha*(1.0 + e_)*g0)*gT;
}


Foam::tmp<Foam::Field<Foam::scalar>>
Foam::ParticleStressModels::Lun::dTaudTheta
(
    const Field<scalar>& alpha,
    const Field<scalar>& rho,
    const Field<scalar>& uSqr
) const
{
    NotImplemented;
    return nullptr;
}


// ************************************************************************* //
