/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018-2019 OpenFOAM Foundation
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
    Foam::diameterModels::driftModels::phaseChange

Description
    Drift induced by interfacial phase change. By default phase change mass
    flux is distributed between sizeGroups of each velocityGroup with phase
    change based on interfacial area of each size group.

SourceFiles
    phaseChange.C

\*---------------------------------------------------------------------------*/

#ifndef phaseChange_H
#define phaseChange_H

#include "populationBalanceModel/driftModels/driftModel/driftModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace diameterModels
{
namespace driftModels
{

/*---------------------------------------------------------------------------*\
                     Class phaseChange Declaration
\*---------------------------------------------------------------------------*/

class phaseChange
:
    public driftModel
{
    // Private data

        //- PhasePairs between which phaseChange occurs, e.g.,
        //  "((gasI and liquid) (gasII and liquid))"
        List<phasePairKey> pairKeys_;

        //- Distribute phase change mass flux between sizeGroups based on the
        //  number concentration, rather than the interfacial area
        Switch numberWeighted_;

        //- Weighting with which the phase change mass flux is distributed
        PtrList<volScalarField> W_;


public:

    //- Runtime type information
    TypeName("phaseChange");

    // Constructor

        //- Construct from a population balance model and a dictionary
        phaseChange
        (
            const populationBalanceModel& popBal,
            const dictionary& dict
        );


    //- Destructor
    virtual ~phaseChange() = default;


    // Member Functions

        //- Correct diameter independent expressions
        virtual void correct();

        //- Add to driftRate
        virtual void addToDriftRate
        (
            volScalarField& driftRate,
            const label i
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace driftModels
} // End namespace diameterModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
