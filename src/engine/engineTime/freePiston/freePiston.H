/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2017 OpenFOAM Foundation
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
    Foam::freePiston

Description
    In case of free pistons, the piston position is determined by a user-defined
    position according to time.

    The piston trajectory is specified in \c constant/engineGeometry using a
    table or a separated file. The position is relative to the cylinder head,
    i.e. decreasing when compressing.

    For example,
    \verbatim
        pistonPositionTime  table ((0 0.13) (0.020 0.03));
    \endverbatim
    or
    \verbatim
        pistonPositionTime
        {
            type    table;
            file    "<constant>/pistonPosition.dat";
            outOfBounds         clamp;
            interpolationScheme linear;
        }
    \endverbatim

SourceFiles
    freePiston.C

\*---------------------------------------------------------------------------*/

#ifndef freePiston_H
#define freePiston_H

#include "engineTime/engineTime/engineTime.H"
#include "db/dictionary/dictionary.H"
#include "dimensionedTypes/dimensionedScalar/dimensionedScalar.H"
#include "primitives/functions/Function1/Function1/Function1Pascal.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class freePiston Declaration
\*---------------------------------------------------------------------------*/

class freePiston
:
    public engineTime
{
    // Private Data

        autoPtr<Function1<scalar>> pistonPositionTime_;


    // Private Member Functions

        //- No copy construct
        freePiston(const freePiston&) = delete;

        //- No copy assignment
        void operator=(const freePiston&) = delete;


public:

    //- Runtime type information
    TypeName("freePiston");


    // Constructors

        //- Construct from objectRegistry arguments
        freePiston
        (
            const word& name,
            const fileName& rootPath,
            const fileName& caseName,
            const fileName& systemName = "system",
            const fileName& constantName = "constant",
            const fileName& dictName = "engineGeometry"
        );

    //- Destructor
    virtual ~freePiston() = default;


    // Member Functions

        // Conversion

            //- Calculate the piston position from the engine geometry
            //  and given crank angle.
            scalar pistonPosition(const scalar theta) const;


        // Access

            //- Return current engine time
            virtual scalar theta() const;

            //- Return time unit
            virtual word unit() const;

            //- Return engine time increment
            virtual scalar deltaTheta() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
