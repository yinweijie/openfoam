/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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
    Foam::exponentialSolidTransport

Description
    Exponential properties for solid heat transport
    Templated into a given thermodynamics package.

SourceFiles
    exponentialSolidTransportI.H
    exponentialSolidTransport.C

\*---------------------------------------------------------------------------*/

#ifndef exponentialSolidTransport_H
#define exponentialSolidTransport_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations

template<class Thermo> class exponentialSolidTransport;

template<class Thermo>
inline exponentialSolidTransport<Thermo> operator*
(
    const scalar,
    const exponentialSolidTransport<Thermo>&
);

template<class Thermo>
Ostream& operator<<
(
    Ostream&,
    const exponentialSolidTransport<Thermo>&
);


/*---------------------------------------------------------------------------*\
                  Class exponentialSolidTransport Declaration
\*---------------------------------------------------------------------------*/

template<class Thermo>
class exponentialSolidTransport
:
    public Thermo
{
    // Private Data

        //- Constant thermal coefficient.
        scalar kappa0_;

        //- Exponent coefficient
        scalar n0_;

        //- Reference temperature
        scalar Tref_;


    // Private Member Functions

        //- Construct from components
        inline exponentialSolidTransport
        (
            const Thermo& t,
            const scalar kappa0,
            const scalar n0,
            const scalar Tref
        );


public:

    // Generated Methods: copy construct, copy assignment


    // Constructors

        //- Construct as named copy
        inline exponentialSolidTransport
        (
            const word&,
            const exponentialSolidTransport&
        );

        //- Construct from dictionary
        explicit exponentialSolidTransport(const dictionary&);

        // Selector from dictionary
        inline static autoPtr<exponentialSolidTransport> New
        (
            const dictionary& dict
        );


    // Member Functions

        //- Return the instantiated type name
        static word typeName()
        {
            return "exponential<" + Thermo::typeName() + '>';
        }

        //- Is the thermal conductivity isotropic
        static const bool isotropic = true;

        //- Thermal conductivity [W/mK]
        inline scalar kappa(const scalar p, const scalar T) const;

        //- Thermal conductivity [W/mK]
        inline vector Kappa(const scalar p, const scalar T) const;

        //- Dynamic viscosity [kg/ms]
        inline scalar mu(const scalar p, const scalar T) const;

        //- Thermal diffusivity of enthalpy [kg/ms]
        inline scalar alphah(const scalar p, const scalar T) const;

        //- Write to Ostream
        void write(Ostream& os) const;


    // Member Operators

        inline void operator+=(const exponentialSolidTransport&);


    // Friend Operators

        friend exponentialSolidTransport operator* <Thermo>
        (
            const scalar,
            const exponentialSolidTransport&
        );


    // IOstream Operators

        friend Ostream& operator<< <Thermo>
        (
            Ostream&,
            const exponentialSolidTransport&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "transport/exponential/exponentialSolidTransportI.H"

#ifdef NoRepository
    #include "transport/exponential/exponentialSolidTransport.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
