/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2017 OpenFOAM Foundation
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
    Foam::pyrolysisChemistryModel

Description
    Pyrolysis chemistry model. It includes gas phase in the solid
    reaction.

SourceFiles
    pyrolysisChemistryModelI.H
    pyrolysisChemistryModel.C

\*---------------------------------------------------------------------------*/

#ifndef pyrolysisChemistryModel_H
#define pyrolysisChemistryModel_H

#include "fields/volFields/volFieldsFwd.H"
#include "fields/DimensionedFields/DimensionedField/DimensionedField.H"
#include "solidChemistryModel/solidChemistryModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class fvMesh;

/*---------------------------------------------------------------------------*\
                   Class pyrolysisChemistryModel Declaration
\*---------------------------------------------------------------------------*/

template<class CompType, class SolidThermo, class GasThermo>
class pyrolysisChemistryModel
:
    public solidChemistryModel<CompType, SolidThermo>
{
protected:

    // Protected Data

        //- List of gas species present in reaction system
        speciesTable pyrolisisGases_;

        //- Thermodynamic data of gases
        PtrList<GasThermo> gasThermo_;

        //- Number of gas species
        label nGases_;

        //- Number of components being solved by ODE
        label nSpecie_;

        //- List of reaction rate per gas [kg/m3/s]
        PtrList<volScalarField::Internal> RRg_;

        //- List of accumulative solid concentrations
        mutable PtrList<volScalarField> Ys0_;


    // Protected Member Functions

        //- Write access to source terms for gases
        inline PtrList<volScalarField::Internal>& RRg();

        //- No copy assignment
        void operator=(const pyrolysisChemistryModel&) = delete;


private:

        //- Cell counter
        label cellCounter_;


public:

    //- Runtime type information
    TypeName("pyrolysis");


    // Constructors

        //- Construct from thermo
        pyrolysisChemistryModel(typename CompType::reactionThermo& thermo);


    //- Destructor
    virtual ~pyrolysisChemistryModel();


    // Member Functions

        //- Thermodynamic data of gases
        inline const PtrList<GasThermo>& gasThermo() const;

        //- Gases table
        inline const speciesTable& gasTable() const;

        //- The number of solids
        inline label nSpecie() const;

        //- The number of solids
        inline label nGases() const;


        //- dc/dt = omega, rate of change in concentration, for each species
        virtual scalarField omega
        (
            const scalarField& c,
            const scalar T,
            const scalar p,
            const bool updateC0 = false
        ) const;

        //- Return the reaction rate for reaction r
        // NOTE: Currently does not calculate reference specie and
        // characteristic times (pf, cf,l Ref, etc.)
        virtual scalar omega
        (
            const Reaction<SolidThermo>& r,
            const scalarField& c,
            const scalar T,
            const scalar p,
            scalar& pf,
            scalar& cf,
            label& lRef,
            scalar& pr,
            scalar& cr,
            label& rRef
        ) const;


        //- Return the reaction rate for iReaction
        // NOTE: Currently does not calculate reference specie and
        // characteristic times (pf, cf,l Ref, etc.)
        virtual scalar omegaI
        (
            label iReaction,
            const scalarField& c,
            const scalar T,
            const scalar p,
            scalar& pf,
            scalar& cf,
            label& lRef,
            scalar& pr,
            scalar& cr,
            label& rRef
        ) const;


        //- Calculates the reaction rates
        virtual void calculate();


        // Chemistry model functions

            //- Return const access to the chemical source terms for gases
            inline const volScalarField::Internal& RRg
            (
                const label i
            ) const;

            //- Return total gas source term
            inline tmp<volScalarField::Internal> RRg() const;

            //- Return sensible enthalpy for gas i [J/Kg]
            virtual tmp<volScalarField> gasHs
            (
                const volScalarField& p,
                const volScalarField& T,
                const label i
            ) const;

            //- Solve the reaction system for the given time step
            //  and return the characteristic time
            virtual scalar solve(const scalar deltaT);


        // ODE functions (overriding abstract functions in ODE.H)

            //- Number of ODE's to solve
            virtual label nEqns() const;

            virtual void derivatives
            (
                const scalar t,
                const scalarField& c,
                scalarField& dcdt
            ) const;

            virtual void jacobian
            (
                const scalar t,
                const scalarField& c,
                scalarField& dcdt,
                scalarSquareMatrix& dfdc
            ) const;

            virtual void solve
            (
                scalarField &c,
                scalar& T,
                scalar& p,
                scalar& deltaT,
                scalar& subDeltaT
            ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #include "pyrolysisChemistryModel/pyrolysisChemistryModelI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "pyrolysisChemistryModel/pyrolysisChemistryModel.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
