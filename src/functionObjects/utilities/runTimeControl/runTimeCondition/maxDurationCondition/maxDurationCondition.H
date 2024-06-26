/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018-2022 OpenCFD Ltd.
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
    Foam::functionObjects::runTimeControls::maxDurationCondition

Description
    Activated after a user-specified duration

SourceFiles
    maxDurationCondition.H
    maxDurationCondition.C

\*---------------------------------------------------------------------------*/

#ifndef functionObjects_runTimeConditions_maxDurationCondition_H
#define functionObjects_runTimeConditions_maxDurationCondition_H

#include "runTimeControl/runTimeCondition/runTimeCondition/runTimeCondition.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{
namespace runTimeControls
{

/*---------------------------------------------------------------------------*\
                    Class maxDurationCondition Declaration
\*---------------------------------------------------------------------------*/

class maxDurationCondition
:
    public runTimeCondition
{
protected:

    // Protected data

        //- Duration
        scalar duration_;

        //- Time when the condition is activated
        scalar startTime_;

        //- Initialisation flag
        bool initialised_;

        //- Reset on restart (ignores any state information)
        Switch resetOnRestart_;


public:

    //- Runtime type information
    TypeName("maxDuration");

    //- Constructor
    maxDurationCondition
    (
        const word& name,
        const objectRegistry& obr,
        const dictionary& dict,
        stateFunctionObject& state
    );

    //- Destructor
    virtual ~maxDurationCondition() = default;


    // Public Member Functions

        //- Apply the condition
        virtual bool apply();

        //- Write
        virtual void write();

        //- Reset
        virtual void reset();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace runTimeControls
} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
