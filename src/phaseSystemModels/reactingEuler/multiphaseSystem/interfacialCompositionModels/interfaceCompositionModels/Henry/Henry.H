/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2018 OpenFOAM Foundation
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
    Foam::interfaceCompositionModels::Henry

Description
    Henry's law for gas solubility in liquid. The concentration of a dissolved
    species in the liquid is proportional to its partial pressure in the gas. A
    dimensionless solubility, \f$k\f$, is given for each species. This is the
    ratio of the concentration of the species in the liquid to the
    corresponding concentration in the gas; i.e., \f$k =
    c_{i,liq}/c_{i,gas}\f$. Mixing in the gas is assumed to be ideal.

SourceFiles
    Henry.C

\*---------------------------------------------------------------------------*/

#ifndef Henry_H
#define Henry_H

#include "interfacialCompositionModels/interfaceCompositionModels/InterfaceCompositionModelCaseDir/InterfaceCompositionModelPascal1.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class phasePair;

namespace interfaceCompositionModels
{

/*---------------------------------------------------------------------------*\
                         Class Henry Declaration
\*---------------------------------------------------------------------------*/

template<class Thermo, class OtherThermo>
class Henry
:
    public InterfaceCompositionModel<Thermo, OtherThermo>
{
    // Private data

        //- Dimensionless solubility coefficients
        const scalarList k_;

        //- The remaining solvent species fraction
        volScalarField YSolvent_;


public:

    //- Runtime type information
    TypeName("Henry");

    // Constructors

        //- Construct from components
        Henry
        (
            const dictionary& dict,
            const phasePair& pair
        );


    //- Destructor
    virtual ~Henry() = default;


    // Member Functions

        //- Update the composition
        virtual void update(const volScalarField& Tf);

        //- The interface species fraction
        virtual tmp<volScalarField> Yf
        (
            const word& speciesName,
            const volScalarField& Tf
        ) const;

        //- The interface species fraction derivative w.r.t. temperature
        virtual tmp<volScalarField> YfPrime
        (
            const word& speciesName,
            const volScalarField& Tf
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace interfaceCompositionModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "interfacialCompositionModels/interfaceCompositionModels/Henry/Henry.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
