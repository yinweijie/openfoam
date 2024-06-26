/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013 OpenFOAM Foundation
    Copyright (C) 2023 OpenCFD Ltd.
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
    Foam::procFacesGAMGProcAgglomeration

Description
    Processor agglomeration of GAMGAgglomerations. Needs nAgglomeratingCells
    which is when to start agglomerating processors. Processors get agglomerated
    by constructing a single cell mesh for each processor with each
    processor interface a face. This then gets agglomerated using
    the pairGAMGAgglomeration algorithm with the number of faces
    on the original processor interface as face weight.

SourceFiles
    procFacesGAMGProcAgglomeration.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_procFacesGAMGProcAgglomeration_H
#define Foam_procFacesGAMGProcAgglomeration_H

#include "matrices/lduMatrix/solvers/GAMG/GAMGProcAgglomerations/GAMGProcAgglomeration/GAMGProcAgglomeration.H"
#include "fields/Fields/labelField/labelField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class procFacesGAMGProcAgglomeration Declaration
\*---------------------------------------------------------------------------*/

class procFacesGAMGProcAgglomeration
:
    public GAMGProcAgglomeration
{
    // Private Data

        //- When to processor agglomerate
        const label nAgglomeratingCells_;


    // Private Member Functions

        //- Return (on master) all single-cell meshes collected. single-cell
        //  meshes are just one cell with all proc faces intact.
        autoPtr<lduPrimitiveMesh> singleCellMesh
        (
            const label singleCellMeshComm,
            const lduMesh& mesh,
            scalarField& faceWeights
        ) const;

        //- Construct processor agglomeration: for every processor the
        //  coarse processor-cluster it agglomerates onto
        tmp<labelField> processorAgglomeration(const lduMesh&) const;

        //- Do we need to agglomerate across processors?
        bool doProcessorAgglomeration(const lduMesh&) const;


public:

    //- Runtime type information
    TypeName("procFaces");

    //- No copy construct
    procFacesGAMGProcAgglomeration(const procFacesGAMGProcAgglomeration&)
        = delete;

    //- No copy assignment
    void operator=(const procFacesGAMGProcAgglomeration&) = delete;


    // Constructors

        //- Construct given agglomerator and controls
        procFacesGAMGProcAgglomeration
        (
            GAMGAgglomeration& agglom,
            const dictionary& controlDict
        );


    //- Destructor
    virtual ~procFacesGAMGProcAgglomeration();


    // Member Functions

        //- Modify agglomeration. Return true if modified
        virtual bool agglomerate();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
