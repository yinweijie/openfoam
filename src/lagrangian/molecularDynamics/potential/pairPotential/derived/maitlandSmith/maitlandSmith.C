/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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

#include "pairPotential/derived/maitlandSmith/maitlandSmith.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace pairPotentials
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(maitlandSmith, 0);

addToRunTimeSelectionTable
(
    pairPotential,
    maitlandSmith,
    dictionary
);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

maitlandSmith::maitlandSmith
(
    const word& name,
    const dictionary& maitlandSmith
)
:
    pairPotential(name, maitlandSmith),
    maitlandSmithCoeffs_(maitlandSmith.subDict(typeName + "Coeffs")),
    m_(maitlandSmithCoeffs_.get<scalar>("m")),
    gamma_(maitlandSmithCoeffs_.get<scalar>("gamma")),
    rm_(maitlandSmithCoeffs_.get<scalar>("rm")),
    epsilon_(maitlandSmithCoeffs_.get<scalar>("epsilon"))
{
    setLookupTables();
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

scalar maitlandSmith::unscaledEnergy(const scalar r) const
{
    scalar nr = (m_ + gamma_*(r/rm_ - 1.0));

    return epsilon_
       *(
            (6.0 / (nr - 6.0))*Foam::pow(r/rm_, -nr)
          - (nr / (nr - 6.0))*Foam::pow(r/rm_, -6)
        );
}


bool maitlandSmith::read(const dictionary& maitlandSmith)
{
    pairPotential::read(maitlandSmith);

    maitlandSmithCoeffs_ = maitlandSmith.subDict(typeName + "Coeffs");

    maitlandSmithCoeffs_.readEntry("m", m_);
    maitlandSmithCoeffs_.readEntry("gamma", gamma_);
    maitlandSmithCoeffs_.readEntry("rm", rm_);
    maitlandSmithCoeffs_.readEntry("epsilon", epsilon_);

    return true;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace pairPotentials
} // End namespace Foam

// ************************************************************************* //
