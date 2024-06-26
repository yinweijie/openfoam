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
    Foam::orderedPhasePair

Description

SourceFiles
    orderedPhasePair.C

\*---------------------------------------------------------------------------*/

#ifndef multiphaseInter_orderedPhasePair_H
#define multiphaseInter_orderedPhasePair_H

#include "phasePair/phasePair.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                      Class orderedPhasePair Declaration
\*---------------------------------------------------------------------------*/

class orderedPhasePair
:
    public phasePair
{
public:

    // Constructors

        //- Construct from two phases and gravity
        orderedPhasePair
        (
            const multiphaseInter::phaseModel& from,
            const multiphaseInter::phaseModel& to
        );


    //- Destructor
    virtual ~orderedPhasePair() = default;


    // Member Functions

        //- Dispersed phase
        virtual const multiphaseInter::phaseModel& from() const;

        //- Continuous phase
        virtual const multiphaseInter::phaseModel& to() const;

        //- Pair name
        virtual word name() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
