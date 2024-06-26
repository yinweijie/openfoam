/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2012 OpenFOAM Foundation
    Copyright (C) 2022 OpenCFD Ltd.
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
    Foam::randomRenumber

Description
    Random renumber. Just to see effect of renumbering.

SourceFiles
    randomRenumber.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_randomRenumber_H
#define Foam_randomRenumber_H

#include "renumberMethod/renumberMethod.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                      Class randomRenumber Declaration
\*---------------------------------------------------------------------------*/

class randomRenumber
:
    public renumberMethod
{
    // Private Member Functions

        //- No copy construct
        randomRenumber(const randomRenumber&) = delete;

        //- No copy assignment
        void operator=(const randomRenumber&) = delete;


public:

    //- Runtime type information
    TypeName("random");


    // Constructors

        //- Construct given the renumber dictionary
        explicit randomRenumber(const dictionary& dict);


    //- Destructor
    virtual ~randomRenumber() = default;


    // Member Functions

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        virtual labelList renumber(const pointField&) const;

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        virtual labelList renumber(const polyMesh&, const pointField&) const;

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        virtual labelList renumber
        (
            const CompactListList<label>& cellCells,
            const pointField& cellCentres
        ) const;

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        virtual labelList renumber
        (
            const labelListList& cellCells,
            const pointField& cellCentres
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
