/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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
    Foam::MGridGenGAMGAgglomeration

Description
    Agglomerate using the MGridGen algorithm.

SourceFiles
    MGridGenGAMGAgglomeration.C
    MGridGenGAMGAgglomerate.C

\*---------------------------------------------------------------------------*/

#ifndef MGridGenGAMGAgglomeration_H
#define MGridGenGAMGAgglomeration_H

#include "fvMesh/fvMesh.H"
#include "matrices/lduMatrix/solvers/GAMG/GAMGAgglomerations/GAMGAgglomeration/GAMGAgglomeration.H"

extern "C"
{
    #include "mgridgen.h"
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class fvMesh;

/*---------------------------------------------------------------------------*\
                           Class MGridGenGAMGAgglomeration Declaration
\*---------------------------------------------------------------------------*/

class MGridGenGAMGAgglomeration
:
    public GAMGAgglomeration
{
    // Private data

        const fvMesh& fvMesh_;

        // Min, max size of agglomerated cells
        const label minSize_;
        const label maxSize_;

        // Number of iterations applied to improve agglomeration consistency
        // across processor boundaries
        label nProcConsistencyIter_;


    // Private Member Functions

        void swap
        (
            const lduInterfacePtrsList& interfaces,
            const labelUList& cellValues,
            PtrList<labelList>& nbrValues
        ) const;

        void getNbrAgglom
        (
            const lduAddressing& addr,
            const lduInterfacePtrsList& interfaces,
            const PtrList<labelList>& nbrGlobalAgglom,
            labelList& cellToNbrAgglom
        ) const;

        void detectSharedFaces
        (
            const lduMesh& mesh,
            const labelList& value,
            labelHashSet& sharedFaces
        ) const;


        //- Construct the CSR format addressing
        void makeCompactCellFaceAddressingAndFaceWeights
        (
            const lduAddressing& fineAddressing,
            List<idxtype>& cellCells,
            List<idxtype>& cellCellOffsets,
            const scalarField& magSi,
            List<scalar>& faceWeights
        );

        //- Calculate and return agglomeration
        tmp<labelField> agglomerate
        (
            label& nCoarseCells,
            const label minSize,
            const label maxSize,
            const lduAddressing& fineAddressing,
            const scalarField& V,
            const scalarField& magSf,
            const scalarField& magSb
        );


        //- No copy construct
        MGridGenGAMGAgglomeration(const MGridGenGAMGAgglomeration&) = delete;

        //- No copy assignment
        void operator=(const MGridGenGAMGAgglomeration&) = delete;


public:

    //- Runtime type information
    TypeName("MGridGen");


    // Constructors

        //- Construct given mesh and controls
        MGridGenGAMGAgglomeration
        (
            const lduMesh& mesh,
            const dictionary& controlDict
        );

        //- Agglomerate from a starting level. Starting level is usually 0
        //- (initial mesh) but sometimes >0 (restarting after processor
        //- agglomeration)
        virtual void agglomerate
        (
            const label nCellsInCoarsestLevel,
            const label startLevel,
            const scalarField& startFaceWeights,
            const bool doProcessorAgglomerate = true
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
