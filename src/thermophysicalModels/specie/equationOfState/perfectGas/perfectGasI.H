/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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

#include "equationOfState/perfectGas/perfectGas.H"
#include "global/constants/thermodynamic/thermodynamicConstants.H"

using namespace Foam::constant::thermodynamic;

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Specie>
inline Foam::perfectGas<Specie>::perfectGas(const Specie& sp)
:
    Specie(sp)
{}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Specie>
inline Foam::perfectGas<Specie>::perfectGas
(
    const word& name,
    const perfectGas<Specie>& pg
)
:
    Specie(name, pg)
{}


template<class Specie>
inline Foam::autoPtr<Foam::perfectGas<Specie>>
Foam::perfectGas<Specie>::clone() const
{
    return autoPtr<perfectGas<Specie>>::New(*this);
}


template<class Specie>
inline Foam::autoPtr<Foam::perfectGas<Specie>> Foam::perfectGas<Specie>::New
(
    const dictionary& dict
)
{
    return autoPtr<perfectGas<Specie>>::New(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Specie>
inline Foam::scalar Foam::perfectGas<Specie>::rho(scalar p, scalar T) const
{
    return p/(this->R()*T);
}


template<class Specie>
inline Foam::scalar Foam::perfectGas<Specie>::H(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::perfectGas<Specie>::Cp(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::perfectGas<Specie>::E(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::perfectGas<Specie>::Cv(scalar p, scalar T) const
{
    return 0;
}


template<class Specie>
inline Foam::scalar Foam::perfectGas<Specie>::S(scalar p, scalar T) const
{
    return -this->R()*log(p/Pstd);
}


template<class Specie>
inline Foam::scalar Foam::perfectGas<Specie>::psi(scalar p, scalar T) const
{
    return 1.0/(this->R()*T);
}


template<class Specie>
inline Foam::scalar Foam::perfectGas<Specie>::Z(scalar p, scalar T) const
{
    return 1;
}


template<class Specie>
inline Foam::scalar Foam::perfectGas<Specie>::CpMCv(scalar p, scalar T) const
{
    return this->R();
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

template<class Specie>
inline void Foam::perfectGas<Specie>::operator+=(const perfectGas<Specie>& pg)
{
    Specie::operator+=(pg);
}


template<class Specie>
inline void Foam::perfectGas<Specie>::operator*=(const scalar s)
{
    Specie::operator*=(s);
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Specie>
inline Foam::perfectGas<Specie> Foam::operator+
(
    const perfectGas<Specie>& pg1,
    const perfectGas<Specie>& pg2
)
{
    return perfectGas<Specie>
    (
        static_cast<const Specie&>(pg1) + static_cast<const Specie&>(pg2)
    );
}


template<class Specie>
inline Foam::perfectGas<Specie> Foam::operator*
(
    const scalar s,
    const perfectGas<Specie>& pg
)
{
    return perfectGas<Specie>(s*static_cast<const Specie&>(pg));
}


template<class Specie>
inline Foam::perfectGas<Specie> Foam::operator==
(
    const perfectGas<Specie>& pg1,
    const perfectGas<Specie>& pg2
)
{
    return perfectGas<Specie>
    (
        static_cast<const Specie&>(pg1) == static_cast<const Specie&>(pg2)
    );
}


// ************************************************************************* //
