/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2012 OpenFOAM Foundation
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
    Foam::psiuReactionThermo

Group
    grpPsiThermo

Description
    Foam::psiuReactionThermo

SourceFiles
    psiuReactionThermo.C
    psiuReactionThermoNew.C

\*---------------------------------------------------------------------------*/

#ifndef psiuReactionThermo_H
#define psiuReactionThermo_H

#include "psiReactionThermo/psiReactionThermo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Class psiuReactionThermo Declaration
\*---------------------------------------------------------------------------*/

class psiuReactionThermo
:
    public psiReactionThermo
{

protected:

    // Protected Member Functions

        wordList heuBoundaryTypes();
        void heuBoundaryCorrection(volScalarField& heu);


public:

    //- Runtime type information
    TypeName("psiuReactionThermo");


    // Declare run-time constructor selection tables

        declareRunTimeSelectionTable
        (
            autoPtr,
            psiuReactionThermo,
            fvMesh,
            (const fvMesh& mesh, const word& phaseName),
            (mesh, phaseName)
        );


        declareRunTimeSelectionTable
        (
            autoPtr,
            psiuReactionThermo,
            fvMeshDictPhase,
            (const fvMesh& mesh, const word& phaseName, const word& dictName),
            (mesh, phaseName, dictName)
        );


    // Constructors

        //- Construct from mesh and phase name
        psiuReactionThermo
        (
            const fvMesh&,
            const word& phaseName
        );


        //- Construct from mesh,dictionary,phase name with a single temperature
        psiuReactionThermo
        (
            const fvMesh&,
            const word& phaseName,
            const word& dictName
        );


    // Selectors

        static autoPtr<psiuReactionThermo> New
        (
            const fvMesh&,
            const word& phaseName=word::null
        );


        static autoPtr<psiuReactionThermo> New
        (
            const fvMesh&,
            const word& phaseName,
            const word& dictName
        );


    //- Destructor
    virtual ~psiuReactionThermo();


    // Member functions

        //- Update properties
        virtual void correct() = 0;


        // Access to thermodynamic state variables.

            //- Unburnt gas enthalpy [J/kg]
            //  Non-const access allowed for transport equations
            virtual volScalarField& heu() = 0;

            //- Unburnt gas enthalpy [J/kg]
            virtual const volScalarField& heu() const = 0;


        // Fields derived from thermodynamic state variables

            //- Unburnt gas enthalpy for cell-set [J/kg]
            virtual tmp<scalarField> heu
            (
                const scalarField& p,
                const scalarField& T,
                const labelList& cells
            ) const = 0;

            //- Unburnt gas enthalpy for patch [J/kg]
            virtual tmp<scalarField> heu
            (
                const scalarField& p,
                const scalarField& T,
                const label patchi
            ) const = 0;

            //- Unburnt gas temperature [K]
            virtual const volScalarField& Tu() const = 0;

            //- Burnt gas temperature [K]
            virtual tmp<volScalarField> Tb() const = 0;

            //- Unburnt gas density [kg/m^3]
            virtual tmp<volScalarField> rhou() const
            {
                return p_*psiu();
            }

            //- Burnt gas density [kg/m^3]
            virtual tmp<volScalarField> rhob() const
            {
                return p_*psib();
            }

            //- Unburnt gas compressibility [s^2/m^2]
            virtual tmp<volScalarField> psiu() const = 0;

            //- Burnt gas compressibility [s^2/m^2]
            virtual tmp<volScalarField> psib() const = 0;

            //- Dynamic viscosity of unburnt gas [kg/ms]
            virtual tmp<volScalarField> muu() const = 0;

            //- Dynamic viscosity of burnt gas [kg/ms]
            virtual tmp<volScalarField> mub() const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
