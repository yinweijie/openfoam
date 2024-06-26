/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2017 OpenCFD Ltd.
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
    Foam::heheuReactionThermo

Description
    Foam::heheuReactionThermo

SourceFiles
    heheuReactionThermo.C

\*---------------------------------------------------------------------------*/

#ifndef heheuPsiThermo_H
#define heheuPsiThermo_H

#include "heThermo/heThermo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                      Class heheuPsiThermo Declaration
\*---------------------------------------------------------------------------*/

template<class BasicPsiThermo, class MixtureType>
class heheuPsiThermo
:
    public heThermo<BasicPsiThermo, MixtureType>
{
    // Private Member Functions

        volScalarField Tu_;
        volScalarField heu_;


    // Private Member Functions

        //- Calculate thermo based on he
        void calculate();

        //- Calculate thermo based on T
        //void calculateT();

        //- Construct as copy (not implemented)
        heheuPsiThermo
        (
            const heheuPsiThermo<BasicPsiThermo, MixtureType>&
        );


public:

    //- Runtime type information
    TypeName("heheuPsiThermo");


    // Constructors

        //- Construct from mesh and phase name
        heheuPsiThermo
        (
            const fvMesh&,
            const word& phaseName
        );


        //- Construct from mesh,dictionary,phase name with a single temperature
        heheuPsiThermo
        (
            const fvMesh&,
            const word& phaseName,
            const word& dictName
        );


    //- Destructor
    virtual ~heheuPsiThermo();


    // Member functions

        //- Update properties
        virtual void correct();


        //- Update properties based on T
        //virtual void correctT();

        // Access to thermodynamic state variables.

            //- Unburnt gas enthalpy [J/kg]
            //  Non-const access allowed for transport equations
            virtual volScalarField& heu()
            {
                return heu_;
            }

            //- Unburnt gas enthalpy [J/kg]
            virtual const volScalarField& heu() const
            {
                return heu_;
            }

            //- Unburnt gas temperature [K]
            virtual const volScalarField& Tu() const
            {
                return Tu_;
            }


        // Fields derived from thermodynamic state variables

            //- Unburnt gas enthalpy for cell-set [J/kg]
            virtual tmp<scalarField> heu
            (
                const scalarField& p,
                const scalarField& T,
                const labelList& cells
            ) const;

            //- Unburnt gas enthalpy for patch [J/kg]
            virtual tmp<scalarField> heu
            (
                const scalarField& p,
                const scalarField& T,
                const label patchi
            ) const;


            //- Burnt gas temperature [K]
            virtual tmp<volScalarField> Tb() const;

            //- Unburnt gas compressibility [s^2/m^2]
            virtual tmp<volScalarField> psiu() const;

            //- Burnt gas compressibility [s^2/m^2]
            virtual tmp<volScalarField> psib() const;


        // Access to transport variables

            //- Dynamic viscosity of unburnt gas [kg/ms]
            virtual tmp<volScalarField> muu() const;

            //- Dynamic viscosity of burnt gas [kg/ms]
            virtual tmp<volScalarField> mub() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#ifdef NoRepository
    #include "psiuReactionThermo/heheuPsiThermo.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
