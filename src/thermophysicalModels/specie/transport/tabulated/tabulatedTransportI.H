/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2020 OpenFOAM Foundation
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

#include "specie/specie.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Thermo>
inline Foam::tabulatedTransport<Thermo>::tabulatedTransport
(
    const Thermo& t,
    const nonUniformTable& mu,
    const nonUniformTable& kappa
)
:
    Thermo(t),
    mu_(mu),
    kappa_(kappa)
{}


template<class Thermo>
inline Foam::tabulatedTransport<Thermo>::tabulatedTransport
(
    const word& name,
    const tabulatedTransport& pt
)
:
    Thermo(name, pt),
    mu_(pt.mu_),
    kappa_(pt.kappa_)
{}


template<class Thermo>
inline Foam::autoPtr<Foam::tabulatedTransport<Thermo>>
Foam::tabulatedTransport<Thermo>::clone() const
{
    return autoPtr<tabulatedTransport<Thermo>>::New(*this);
}


template<class Thermo>
inline Foam::autoPtr<Foam::tabulatedTransport<Thermo>>
Foam::tabulatedTransport<Thermo>::New(const dictionary& dict)
{
    return autoPtr<tabulatedTransport<Thermo>>::New(dict);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Thermo>
inline Foam::scalar Foam::tabulatedTransport<Thermo>::mu
(
    const scalar p,
    const scalar T
) const
{
    return mu_.f(p, T);
}


template<class Thermo>
inline Foam::scalar Foam::tabulatedTransport<Thermo>::kappa
(
    const scalar p,
    const scalar T
) const
{
    return kappa_.f(p, T);
}


template<class Thermo>
inline Foam::scalar Foam::tabulatedTransport<Thermo>::alphah
(
    const scalar p,
    const scalar T
) const
{
    return kappa(p, T)/this->Cp(p, T);
}


// ************************************************************************* //
