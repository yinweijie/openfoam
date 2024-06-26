/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
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
    Foam::pairPotential

Description

SourceFiles
    pairPotential.C
    pairPotentialNew.C

\*---------------------------------------------------------------------------*/

#ifndef pairPotential_H
#define pairPotential_H

#include "db/IOobjects/IOdictionary/IOdictionary.H"
#include "db/typeInfo/typeInfo.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"
#include "memory/autoPtr/autoPtr.H"
#include "containers/Lists/List/List.H"
#include "primitives/tuples/Pair.H"
#include "primitives/bools/Switch/Switch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes
class energyScalingFunction;

/*---------------------------------------------------------------------------*\
                        Class pairPotential Declaration
\*---------------------------------------------------------------------------*/

class pairPotential
{

protected:

    // Protected member data

        word name_;
        dictionary pairPotentialProperties_;

        scalar rCut_;
        scalar rCutSqr_;

        scalar rMin_;
        scalar dr_;

        List<scalar> forceLookup_;
        List<scalar> energyLookup_;

        mutable energyScalingFunction* esfPtr_;

        bool writeTables_;


    // Protected member functions

        void scaleEnergy(scalar& e, const scalar r) const;

        //- No copy construct
        pairPotential(const pairPotential&) = delete;

        //- No copy assignment
        void operator=(const pairPotential&) = delete;


public:

    //- Runtime type information
    TypeName("pairPotential");


    // Declare run-time constructor selection table

        declareRunTimeSelectionTable
        (
            autoPtr,
            pairPotential,
            dictionary,
            (
                const word& name,
                const dictionary& pairPotentialProperties
            ),
            (name, pairPotentialProperties)
        );


    // Selectors

        //- Return a reference to the selected viscosity model
        static autoPtr<pairPotential> New
        (
            const word& name,
            const dictionary& pairPotentialProperties
        );


    // Constructors

        //- Construct from components
        pairPotential
        (
            const word& name,
            const dictionary& pairPotentialProperties
        );


    //- Destructor
    virtual ~pairPotential() = default;


    // Member Functions

        void setLookupTables();

        inline scalar rMin() const;

        inline scalar dr() const;

        inline scalar rCut() const;

        inline scalar rCutSqr() const;

        scalar energy (const scalar r) const;

        scalar force (const scalar r) const;

        List<Pair<scalar>> energyTable() const;

        List<Pair<scalar>> forceTable() const;

        inline bool writeTables() const;

        virtual scalar unscaledEnergy(const scalar r) const = 0;

        scalar scaledEnergy(const scalar r) const;

        scalar energyDerivative
        (
            const scalar r,
            const bool scaledEnergyDerivative = true
        ) const;

        const dictionary& pairPotentialProperties() const
        {
            return pairPotentialProperties_;
        }

        bool writeEnergyAndForceTables(Ostream& os) const;

        //- Read pairPotential dictionary
        virtual bool read(const dictionary& pairPotentialProperties) = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "pairPotential/basic/pairPotentialI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
