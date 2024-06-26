/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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
    Foam::renumberMethod

Description
    Abstract base class for renumbering

SourceFiles
    renumberMethod.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_renumberMethod_H
#define Foam_renumberMethod_H

#include "meshes/polyMesh/polyMesh.H"
#include "meshes/primitiveShapes/point/pointField.H"
#include "containers/CompactLists/CompactListList/CompactListList.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class renumberMethod Declaration
\*---------------------------------------------------------------------------*/

class renumberMethod
{
protected:

    // Protected Data

        const dictionary& renumberDict_;


    // Protected Member Functions

        //- No copy construct
        renumberMethod(const renumberMethod&) = delete;

        //- No copy assignment
        void operator=(const renumberMethod&) = delete;


public:

    //- Runtime type information
    TypeName("renumberMethod");


    // Declare run-time constructor selection tables

        declareRunTimeSelectionTable
        (
            autoPtr,
            renumberMethod,
            dictionary,
            (
                const dictionary& renumberDict
            ),
            (renumberDict)
        );


    // Selectors

        //- Return a reference to the selected renumbering method
        static autoPtr<renumberMethod> New
        (
            const dictionary& renumberDict
        );


    // Constructors

        //- Construct given the renumber dictionary
        explicit renumberMethod(const dictionary& dict)
        :
            renumberDict_(dict)
        {}


    //- Destructor
    virtual ~renumberMethod() = default;


    // Member Functions

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        //  This is only defined for geometric renumberMethods.
        virtual labelList renumber(const pointField&) const
        {
            NotImplemented;
            return labelList();
        }

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        //  Use the mesh connectivity (if needed)
        virtual labelList renumber(const polyMesh&, const pointField&) const;

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        //  Addressing in losort addressing (= neighbour + offsets into
        //  neighbour)
        virtual labelList renumber
        (
            const labelList& cellCells,
            const labelList& offsets,
            const pointField&
        ) const;

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        //  Gets passed agglomeration map (from fine to coarse cells) and coarse
        //  cell
        //  location. Can be overridden by renumberMethods that provide this
        //  functionality natively. Coarse cells are local to the processor
        //  (if in parallel). If you want to have coarse cells spanning
        //  processors use the globalCellCells instead.
        virtual labelList renumber
        (
            const polyMesh& mesh,
            const labelList& fineToCoarse,
            const pointField& coarsePoints
        ) const;

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        //  Uses 'unpack' internally, so should be overloaded when possible
        virtual labelList renumber
        (
            const CompactListList<label>& cellCells,
            const pointField& cellCentres
        ) const;

        //- Return the order in which cells need to be visited
        //- (ie. from ordered back to original cell label).
        //  The connectivity is equal to mesh.cellCells() except
        //  - the connections are across coupled patches
        virtual labelList renumber
        (
            const labelListList& cellCells,
            const pointField& cellCentres
        ) const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
