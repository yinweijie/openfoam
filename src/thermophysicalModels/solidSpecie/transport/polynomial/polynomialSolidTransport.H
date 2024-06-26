/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2017 OpenFOAM Foundation
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
    Foam::polynomialSolidTransport

Description
    Transport package using polynomial functions for solid \c kappa.

Usage

    \table
        Property        | Description
        kappaCoeffs<8>  | Thermal conductivity polynomial coefficients
    \endtable

    Example of the specification of the transport properties:
    \verbatim
    transport
    {
        kappaCoeffs<8>     ( 1000 -0.05 0.003 0 0 0 0 0 );
    }
    \endverbatim

    The polynomial expression is evaluated as so:

        \f[
            \kappa = 1000 - 0.05 T + 0.003 T^2
        \f]

Note
    Thermal conductivity polynomial coefficients evaluate to an expression in
    [W/m/K].

SourceFiles
    polynomialSolidTransportI.H
    polynomialSolidTransport.C

See also
    Foam::Polynomial

\*---------------------------------------------------------------------------*/

#ifndef polynomialSolidTransport_H
#define polynomialSolidTransport_H

#include "primitives/functions/Polynomial/PolynomialPascal.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations

template<class Thermo, int PolySize> class polynomialSolidTransport;

template<class Thermo, int PolySize>
inline polynomialSolidTransport<Thermo, PolySize> operator+
(
    const polynomialSolidTransport<Thermo, PolySize>&,
    const polynomialSolidTransport<Thermo, PolySize>&
);

template<class Thermo, int PolySize>
inline polynomialSolidTransport<Thermo, PolySize> operator*
(
    const scalar,
    const polynomialSolidTransport<Thermo, PolySize>&
);

template<class Thermo, int PolySize>
Ostream& operator<<
(
    Ostream&,
    const polynomialSolidTransport<Thermo, PolySize>&
);


/*---------------------------------------------------------------------------*\
                     Class polynomialSolidTransport Declaration
\*---------------------------------------------------------------------------*/

template<class Thermo, int PolySize=8>
class polynomialSolidTransport
:
    public Thermo
{
    // Private Data

        //- Thermal conductivity polynomial coefficients
        //  Note: input in [W/m/K]
        Polynomial<PolySize> kappaCoeffs_;


    // Private Member Functions

        //- Coeffs name. Eg, "kappaCoeffs<10>"
        inline static word coeffsName(const char* name)
        {
            return word(name + ("Coeffs<" + std::to_string(PolySize) + '>'));
        }

        //- Construct from components
        inline polynomialSolidTransport
        (
            const Thermo& t,
            const Polynomial<PolySize>& kappaPoly
        );


public:

    // Generated Methods: copy construct, copy assignment


    // Constructors

        //- Construct as named copy
        inline polynomialSolidTransport
        (
            const word&,
            const polynomialSolidTransport&
        );

        //- Construct from dictionary
        explicit polynomialSolidTransport(const dictionary& dict);

        //- Construct and return a clone
        inline autoPtr<polynomialSolidTransport> clone() const;

        // Selector from dictionary
        inline static autoPtr<polynomialSolidTransport> New
        (
            const dictionary&dict
        );


    // Member Functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "polynomial<" + Thermo::typeName() + '>';
        }

        //- Is the thermal conductivity isotropic
        static const bool isotropic = true;

        //- Dynamic viscosity [kg/ms]
        inline scalar mu(const scalar p, const scalar T) const;

        //- Thermal conductivity [W/mK]
        inline scalar kappa(const scalar p, const scalar T) const;

        //- Thermal conductivity [W/mK]
        inline vector Kappa(const scalar p, const scalar T) const;

        //- Thermal diffusivity of enthalpy [kg/ms]
        inline scalar alphah(const scalar p, const scalar T) const;


        //- Write to Ostream
        void write(Ostream& os) const;


    // Member Operators

        inline void operator+=(const polynomialSolidTransport&);
        inline void operator*=(const scalar);


    // Friend Operators

        friend polynomialSolidTransport operator+ <Thermo, PolySize>
        (
            const polynomialSolidTransport&,
            const polynomialSolidTransport&
        );

        friend polynomialSolidTransport operator* <Thermo, PolySize>
        (
            const scalar,
            const polynomialSolidTransport&
        );


    // IOstream Operators

        friend Ostream& operator<< <Thermo, PolySize>
        (
            Ostream&,
            const polynomialSolidTransport&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "transport/polynomial/polynomialSolidTransportI.H"

#ifdef NoRepository
    #include "transport/polynomial/polynomialSolidTransport.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
