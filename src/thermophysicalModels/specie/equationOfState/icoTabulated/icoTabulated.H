/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2020 OpenFOAM Foundation
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
    Foam::icoTabulated

Description
    Incompressible of equation of state using non-uniform tabulated
    density vs temperature.

Usage
    \table
        Property     | Description
        rho          | Density vs temperature table
    \endtable

    Example of the specification of the equation of state:
    \verbatim
    equationOfState
    {
        rho
        (
            (200 1010)
            (350 1000)
            (400 980)
        );
    }
    \endverbatim

SourceFiles
    icoTabulatedI.H
    icoTabulated.C

See also
    Foam::thermophysicalFunctions::nonUniformTable

\*---------------------------------------------------------------------------*/

#ifndef icoTabulated_H
#define icoTabulated_H

#include "thermophysicalFunctions/nonUniformTable/nonUniformTableThermophysicalFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations

template<class Specie> class icoTabulated;

template<class Specie>
Ostream& operator<<(Ostream& os, const icoTabulated<Specie>&);


/*---------------------------------------------------------------------------*\
                        Class icoTabulated Declaration
\*---------------------------------------------------------------------------*/

template<class Specie>
class icoTabulated
:
    public Specie
{
    // Private Data

        //- Density table [kg/m^3]
        nonUniformTable rho_;


public:

    // Constructors

        //- Construct from components
        inline icoTabulated
        (
            const Specie& sp,
            const nonUniformTable& rho
        );

        //- Construct from dictionary
        explicit icoTabulated(const dictionary& dict);

        //- Construct as named copy
        inline icoTabulated(const word& name, const icoTabulated&);

        //- Construct and return a clone
        inline autoPtr<icoTabulated> clone() const;

        //- Selector from dictionary
        inline static autoPtr<icoTabulated> New(const dictionary& dict);


    // Member Functions

        //- The instantiated type name
        static word typeName()
        {
            return "icoTabulated<" + word(Specie::typeName_()) + '>';
        }


        // Fundamental Properties

            //- Is the equation of state is incompressible i.e. rho != f(p)
            static const bool incompressible = true;

            //- Is the equation of state is isochoric i.e. rho = const
            static const bool isochoric = false;

            //- Return density [kg/m^3]
            inline scalar rho(scalar p, scalar T) const;

            //- Return enthalpy departure [J/kg]
            inline scalar H(const scalar p, const scalar T) const;

            //- Return Cp departure [J/(kg K]
            inline scalar Cp(scalar p, scalar T) const;

            //- Return internal energy departure [J/kg]
            inline scalar E(const scalar p, const scalar T) const;

            //- Return Cv departure [J/(kg K]
            inline scalar Cv(scalar p, scalar T) const;

            //- Return entropy departure to the integral of Cp/T [J/kg/K]
            inline scalar S(const scalar p, const scalar T) const;

            //- Return compressibility [s^2/m^2]
            inline scalar psi(scalar p, scalar T) const;

            //- Return compression factor []
            inline scalar Z(scalar p, scalar T) const;

            //- Return (Cp - Cv) [J/(kg K]
            inline scalar CpMCv(scalar p, scalar T) const;


        // IO

            //- Write to Ostream
            void write(Ostream& os) const;


    // Ostream Operator

        friend Ostream& operator<< <Specie>
        (
            Ostream&,
            const icoTabulated&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "equationOfState/icoTabulated/icoTabulatedI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "equationOfState/icoTabulated/icoTabulated.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
