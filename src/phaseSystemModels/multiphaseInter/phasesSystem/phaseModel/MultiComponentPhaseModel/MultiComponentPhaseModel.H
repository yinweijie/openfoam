/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2017-2022 OpenCFD Ltd.
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
    Foam::MultiComponentPhaseModel

Description
    Class which represents a phase with multiple species. Returns the species'
    mass fractions, and their governing equations.

SourceFiles
    MultiComponentPhaseModel.C

\*---------------------------------------------------------------------------*/

#ifndef MultiComponentPhaseModel_H
#define MultiComponentPhaseModel_H

#include "phaseModel/phaseModel/phaseModel.H"
#include "primitives/strings/lists/hashedWordList.H"
#include "rhoReactionThermo/rhoReactionThermo.H"
#include "basicThermo/basicThermo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class phaseModel Declaration
\*---------------------------------------------------------------------------*/

template<class BasePhaseModel, class phaseThermo>
class MultiComponentPhaseModel
:
    public BasePhaseModel
{
protected:

    // Protected data

        //- Species table
        hashedWordList species_;

        //- Inert species index
        label inertIndex_;

        //- Thermophysical model
        autoPtr<phaseThermo> thermoPtr_;

        //- Ptr list of volumetric fractions for species
        PtrList<volScalarField> X_;

        //- Add diffusion transport on Yi's Eq
        bool addDiffusion_;

        //- Schmidt number
        scalar Sct_;


    // Protected functions

        //- Transfor volume fraction into mass fractions
        void calculateMassFractions();

        //- Transfor mass fraction into volume fractions
        void calculateVolumeFractions();


public:

    // Constructors

        MultiComponentPhaseModel
        (
            const multiphaseInterSystem& fluid,
            const word& phaseName
        );


    //- Destructor
    virtual ~MultiComponentPhaseModel() = default;


    // Member Functions

        // Access

            //- Species table
            const hashedWordList& species() const
            {
                return species_;
            }


        // Thermo

            //- Access to thermo
            virtual const phaseThermo& thermo() const;

            //- Access non-const thermo
            virtual phaseThermo& thermo();

            //- Correct phase thermo
            virtual void correct();

            //- Solve species fraction equation
            virtual void solveYi
            (
                PtrList<volScalarField::Internal>&,
                PtrList<volScalarField::Internal>&
            );

            //- Constant access the species mass fractions
            virtual const PtrList<volScalarField>& Y() const;

            //- Access the species mass fractions
            virtual PtrList<volScalarField>& Y();

            //- Return inert species index
            label inertIndex() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "MultiComponentPhaseModel.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
