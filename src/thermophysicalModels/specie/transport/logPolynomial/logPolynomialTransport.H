/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 OpenFOAM Foundation
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

Class
    Foam::logPolynomialTransport

Group
    grpSpecieTransport

Description
    Transport package using polynomial functions of \c ln(T) for \c mu and
    \c kappa:

        \f[
            ln(mu)    = \sum_{i=1}^N \left( a[i] * ln(T)^{i-1} \right)
        \f]

        \f[
            ln(kappa) = \sum_{i=1}^N \left( b[i] * ln(T)^{i-1} \right)
        \f]

Usage

    \table
        Property        | Description
        muCoeffs<8>     | Dynamic viscosity polynomial coefficients
        kappaCoeffs<8>  | Thermal conductivity polynomial coefficients
    \endtable

    Example of the specification of the transport properties:
    \verbatim
    transport
    {
        muCoeffs<8>     ( 1000 -0.05 0.003 0 0 0 0 0 );
        kappaCoeffs<8>  ( 2000 -0.15 0.023 0 0 0 0 0 );
    }
    \endverbatim

    The polynomial expressions are evaluated as so:

        \f[
            \mu    = 1000 - 0.05 ln(T) + 0.003 ln(T)^2
        \f]

        \f[
            \kappa = 2000 - 0.15 ln(T) + 0.023 ln(T)^2
        \f]

Note
    - Dynamic viscosity polynomial coefficients evaluate to an expression in
      [Pa.s], but internally uses [Pa.s/kmol].
    - Thermal conductivity polynomial coefficients evaluate to an expression in
      [W/m/K], but internally uses [W/m/K/kmol].

SourceFiles
    logPolynomialTransportI.H
    logPolynomialTransport.C

See also
    Foam::Polynomial

\*---------------------------------------------------------------------------*/

#ifndef logPolynomialTransport_H
#define logPolynomialTransport_H

#include "primitives/functions/Polynomial/PolynomialPascal.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations

template<class Thermo, int PolySize> class logPolynomialTransport;

template<class Thermo, int PolySize>
inline logPolynomialTransport<Thermo, PolySize> operator+
(
    const logPolynomialTransport<Thermo, PolySize>&,
    const logPolynomialTransport<Thermo, PolySize>&
);

template<class Thermo, int PolySize>
inline logPolynomialTransport<Thermo, PolySize> operator*
(
    const scalar,
    const logPolynomialTransport<Thermo, PolySize>&
);

template<class Thermo, int PolySize>
Ostream& operator<<
(
    Ostream&,
    const logPolynomialTransport<Thermo, PolySize>&
);


/*---------------------------------------------------------------------------*\
                     Class logPolynomialTransport Declaration
\*---------------------------------------------------------------------------*/

template<class Thermo, int PolySize=8>
class logPolynomialTransport
:
    public Thermo
{
    // Private Data

        //- Dynamic viscosity polynomial coefficients
        //  Note: input in [Pa.s], but internally uses [Pa.s/kmol]
        Polynomial<PolySize> muCoeffs_;

        //- Thermal conductivity polynomial coefficients
        //  Note: input in [W/m/K], but internally uses [W/m/K/kmol]
        Polynomial<PolySize> kappaCoeffs_;


    // Private Member Functions

        //- Coeffs name. Eg, "muLogCoeffs<10>"
        inline static word coeffsName(const char* name)
        {
            return word(name + ("LogCoeffs<" + std::to_string(PolySize) + '>'));
        }

        //- Construct from components
        inline logPolynomialTransport
        (
            const Thermo& t,
            const Polynomial<PolySize>& muPoly,
            const Polynomial<PolySize>& kappaPoly
        );


public:

    // Generated Methods: copy construct, copy assignment


    // Constructors

        //- Construct as named copy
        inline logPolynomialTransport
        (
            const word&,
            const logPolynomialTransport&
        );

        //- Construct from dictionary
        explicit logPolynomialTransport(const dictionary& dict);

        //- Construct and return a clone
        inline autoPtr<logPolynomialTransport> clone() const;

        // Selector from dictionary
        inline static autoPtr<logPolynomialTransport> New
        (
            const dictionary& dict
        );


    // Member Functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "logPolynomial<" + Thermo::typeName() + '>';
        }

        //- Dynamic viscosity [kg/ms]
        inline scalar mu(const scalar p, const scalar T) const;

        //- Thermal conductivity [W/mK]
        inline scalar kappa(const scalar p, const scalar T) const;

        //- Thermal diffusivity of enthalpy [kg/ms]
        inline scalar alphah(const scalar p, const scalar T) const;

        // Species diffusivity
        //inline scalar D(const scalar p, const scalar T) const;

        //- Write to Ostream
        void write(Ostream& os) const;


    // Member Operators

        inline void operator+=(const logPolynomialTransport&);

        inline void operator*=(const scalar);


    // Friend Operators

        friend logPolynomialTransport operator+ <Thermo, PolySize>
        (
            const logPolynomialTransport&,
            const logPolynomialTransport&
        );

        friend logPolynomialTransport operator* <Thermo, PolySize>
        (
            const scalar,
            const logPolynomialTransport&
        );


    // IOstream Operators

        friend Ostream& operator<< <Thermo, PolySize>
        (
            Ostream&,
            const logPolynomialTransport&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "transport/logPolynomial/logPolynomialTransportI.H"

#ifdef NoRepository
    #include "transport/logPolynomial/logPolynomialTransport.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
