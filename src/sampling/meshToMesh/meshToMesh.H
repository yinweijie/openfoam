/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2016 OpenFOAM Foundation
    Copyright (C) 2015-2023 OpenCFD Ltd.
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
    Foam::meshToMesh

Description
    Class to calculate the cell-addressing between two overlapping meshes

    Mapping is performed using a run-time selectable interpolation mothod

SeeAlso
    meshToMeshMethod

SourceFiles
    meshToMesh.C
    meshToMeshParallelOps.C
    meshToMeshTemplates.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_meshToMesh_H
#define Foam_meshToMesh_H

#include "meshes/polyMesh/polyMesh.H"
#include "meshes/treeBoundBox/treeBoundBox.H"
#include "meshes/polyMesh/mapPolyMesh/mapDistribute/mapDistribute.H"
#include "fields/volFields/volFieldsFwd.H"
#include "primitives/enums/Enum.H"
#include "AMIInterpolation/AMIInterpolation/AMIPatchToPatchInterpolation.H"
#include "meshes/primitiveShapes/point/pointList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                          Class meshToMesh Declaration
\*---------------------------------------------------------------------------*/

class meshToMesh
{
public:

    // Public data types

        //- Enumeration specifying interpolation method
        enum class interpolationMethod
        {
            imDirect,
            imMapNearest,
            imCellVolumeWeight,
            imCorrectedCellVolumeWeight
        };

        static const Enum<interpolationMethod> interpolationMethodNames_;

        //- Enumeration specifying processor parallel map construction method
        enum class procMapMethod
        {
            pmAABB,
            pmLOD
        };

        static const Enum<procMapMethod> procMapMethodNames_;

private:

    // Private data

        //- Reference to the source mesh
        const polyMesh& srcRegion_;

        //- Reference to the target mesh
        const polyMesh& tgtRegion_;

        //- Processor parallel map construction method
        procMapMethod procMapMethod_;

        //- List of target patch IDs per source patch (local index)
        List<label> srcPatchID_;

        //- List of source patch IDs per target patch (local index)
        List<label> tgtPatchID_;

        //- List of AMIs between source and target patches
        PtrList<AMIPatchToPatchInterpolation> patchAMIs_;

        //- Cutting patches whose values are set using a zero-gradient condition
        List<label> cuttingPatches_;

        //- Source to target cell addressing
        labelListList srcToTgtCellAddr_;

        //- Target to source cell addressing
        labelListList tgtToSrcCellAddr_;

        //- Source to target cell interpolation weights
        scalarListList srcToTgtCellWght_;

        //- Target to source cell interpolation weights
        scalarListList tgtToSrcCellWght_;

        // Vectors from cell centre to overlap volume for 2nd order correction
        // (only set for corrected methods)

            //- Source to target cell offset vectors
            pointListList srcToTgtCellVec_;

            //- Target to source cell offset vectors
            pointListList tgtToSrcCellVec_;

        //- Cell total volume in overlap region [m3]
        scalar V_;

        //- Index of processor that holds all of both sides.
        //- The value is -1 for distributed cases
        label singleMeshProc_;

        //- Source map pointer - parallel running only
        autoPtr<mapDistribute> srcMapPtr_;

        //- Target map pointer - parallel running only
        autoPtr<mapDistribute> tgtMapPtr_;


    // Private Member Functions

        //- Helper function to add a constant offset to a list
        template<class Type>
        void add(UList<Type>& fld, const label offset) const;

        //- Helper function to interpolate internal field. Optionally uses
        //  gradient. Template specialisations for tensor types below
        template<class Type, class CombineOp>
        void mapInternalSrcToTgt
        (
            const VolumeField<Type>& field,
            const CombineOp& cop,
            VolumeField<Type>& result,
            const bool secondOrder
        ) const;

        //- Helper function to interpolate internal field. Optionally uses
        //  gradient. Template specialisations for tensor types below
        template<class Type, class CombineOp>
        void mapInternalTgtToSrc
        (
            const VolumeField<Type>& field,
            const CombineOp& cop,
            VolumeField<Type>& result,
            const bool secondOrder
        ) const;

        //- Helper function to interpolate patch field. Template
        //  specialisations  below
        template<class Type, class CombineOp>
        void mapAndOpSrcToTgt
        (
            const AMIPatchToPatchInterpolation& AMI,
            const Field<Type>& srcField,
            Field<Type>& tgtField,
            const CombineOp& cop
        ) const;

        //- Helper function to interpolate patch field. Template
        //  specialisations  below
        template<class Type, class CombineOp>
        void mapAndOpTgtToSrc
        (
            const AMIPatchToPatchInterpolation& AMI,
            Field<Type>& srcField,
            const Field<Type>& tgtField,
            const CombineOp& cop
        ) const;

        //- Return src cell IDs for the overlap region
        labelList maskCells(const polyMesh& src, const polyMesh& tgt) const;

        //- Normalise the interpolation weights
        void normaliseWeights
        (
            const word& descriptor,
            const labelListList& addr,
            scalarListList& wght
        ) const;

        //- Calculate the addressing between overlapping regions of src and tgt
        //  meshes
        void calcAddressing
        (
            const word& methodName,
            const polyMesh& src,
            const polyMesh& tgt
        );

        //- Calculate - main driver function
        void calculate(const word& methodName, const bool normalise);

        //- Calculate patch overlap
        void calculatePatchAMIs(const word& amiMethodName);

        //- Constructor helper
        void constructNoCuttingPatches
        (
            const word& methodName,
            const word& AMIMethodName,
            const bool interpAllPatches
        );

        //- Constructor helper
        void constructFromCuttingPatches
        (
            const word& methodName,
            const word& AMIMethodName,
            const HashTable<word>& patchMap,
            const wordList& cuttingPatches,
            const bool normalise
        );

        // Parallel operations

            //- Determine whether the meshes are split across multiple
            //- processors
            label calcDistribution
            (
                const polyMesh& src,
                const polyMesh& tgt
            ) const;

            //- Determine which processor bounding-boxes overlap
            label calcOverlappingProcs
            (
                const List<treeBoundBoxList>& procBb,
                const boundBox& bb,
                boolList& overlaps
            ) const;

            //- Calculate the mapping between processors
            autoPtr<mapDistribute> calcProcMap
            (
                const polyMesh& src,
                const polyMesh& tgt
            ) const;

            //- Distribute mesh info from 'my' processor to others
            void distributeCells
            (
                const mapDistribute& map,
                const polyMesh& tgtMesh,
                const globalIndex& globalI,
                List<pointField>& points,
                List<label>& nInternalFaces,
                List<faceList>& faces,
                List<labelList>& faceOwner,
                List<labelList>& faceNeighbour,
                List<labelList>& cellIDs,
                List<labelList>& nbrProcIDs,
                List<labelList>& procLocalFaceIDs
            ) const;

            //- Collect pieces of tgt mesh from other processors and restructure
            void distributeAndMergeCells
            (
                const mapDistribute& map,
                const polyMesh& tgt,
                const globalIndex& globalI,
                pointField& tgtPoints,
                faceList& tgtFaces,
                labelList& tgtFaceOwners,
                labelList& tgtFaceNeighbours,
                labelList& tgtCellIDs
            ) const;


        //- No copy construct
        meshToMesh(const meshToMesh&) = delete;

        //- No copy assignment
        void operator=(const meshToMesh&) = delete;


public:

    //- Run-time type information
    TypeName("meshToMesh");


    //- Construct from source and target meshes
    meshToMesh
    (
        const polyMesh& src,
        const polyMesh& tgt,
        const interpolationMethod method,
        const procMapMethod mapMethod = procMapMethod::pmAABB,
        const bool interpAllPatches = true
    );

    //- Construct from source and target meshes, generic mapping methods
    meshToMesh
    (
        const polyMesh& src,
        const polyMesh& tgt,
        const word& methodName,     // internal mapping
        const word& AMIMethodName,  // boundary mapping
        const procMapMethod mapMethod = procMapMethod::pmAABB,
        const bool interpAllPatches = true
    );

    //- Construct from source and target meshes
    meshToMesh
    (
        const polyMesh& src,
        const polyMesh& tgt,
        const interpolationMethod method,
        const HashTable<word>& patchMap,
        const wordList& cuttingPatches,
        const procMapMethod mapMethod = procMapMethod::pmAABB,
        const bool normalise = true
    );


    //- Construct from source and target meshes, generic mapping methods
    meshToMesh
    (
        const polyMesh& src,
        const polyMesh& tgt,
        const word& methodName,     // internal mapping
        const word& AMIMethodName,  // boundary mapping
        const HashTable<word>& patchMap,
        const wordList& cuttingPatches,
        const procMapMethod mapMethod = procMapMethod::pmAABB,
        const bool normalise = true
    );


    //- Destructor
    virtual ~meshToMesh();


    // Member Functions

        // Access

            //- Return const access to the source mesh
            inline const polyMesh& srcRegion() const;

            //- Return const access to the target mesh
            inline const polyMesh& tgtRegion() const;

            //- Return const access to the source to target cell addressing
            inline const labelListList& srcToTgtCellAddr() const;

            //- Return const access to the target to source cell addressing
            inline const labelListList& tgtToSrcCellAddr() const;

            //- Return const access to the source to target cell weights
            inline const scalarListList& srcToTgtCellWght() const;

            //- Return const access to the target to source cell weights
            inline const scalarListList& tgtToSrcCellWght() const;

            //- Return const access to the source to target offset vectors
            inline const pointListList& srcToTgtCellVec() const;

            //- Return const access to the target to source offset vectors
            inline const pointListList& tgtToSrcCellVec() const;

            //- Return const access to the overlap volume
            inline scalar V() const;

            //- Conversion between mesh and patch interpolation methods
            static word interpolationMethodAMI
            (
                const interpolationMethod method
            );

            //- Return the list of AMIs between source and target patches
            inline const PtrList<AMIPatchToPatchInterpolation>&
            patchAMIs() const;


        // Mapping

            //- Distributed across processors (singleMeshProc == -1)
            inline bool distributed() const noexcept;

            //- Pointer to the source map (if distributed).
            //- Can be checked as a bool.
            inline const mapDistribute* hasSrcMap() const noexcept;

            //- Pointer to the target map (if distributed).
            //- Can be checked as a bool.
            inline const mapDistribute* hasTgtMap() const noexcept;

            //- Source map pointer - valid if no singleMeshProc
            inline const autoPtr<mapDistribute>& srcMap() const noexcept;

            //- Target map pointer - valid if no singleMeshProc
            inline const autoPtr<mapDistribute>& tgtMap() const noexcept;


        // Evaluation

            // Source-to-target field mapping

                //- Map field from src to tgt mesh with defined operation.
                //  Values passed in via 'result' are used to initialise the
                //  return value
                template<class Type, class CombineOp>
                void mapSrcToTgt
                (
                    const UList<Type>& srcFld,
                    const CombineOp& cop,
                    List<Type>& result
                ) const;

                //- Map extrapolated field (using gradient) from src to tgt
                //  mesh with defined operation. Falls back to non-extrapolated
                //  mapping (above) if not constructed with method that supports
                //  getting offset vectors. Extrapolation only for internal
                //  values. Values passed in via 'result' are used to
                //  initialise the return value.
                template<class Type, class CombineOp>
                void mapSrcToTgt
                (
                    const UList<Type>& srcField,
                    const UList<typename outerProduct<vector, Type>::type>&,
                    const CombineOp& cop,
                    List<Type>& result
                ) const;

                //- Return the src field mapped to the tgt mesh with a defined
                //  operation. Initial values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<Field<Type>> mapSrcToTgt
                (
                    const Field<Type>& srcFld,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a tmp field to the tgt mesh
                //  with a defined operation
                template<class Type, class CombineOp>
                tmp<Field<Type>> mapSrcToTgt
                (
                    const tmp<Field<Type>>& tsrcFld,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a field to the tgt mesh with a
                //  default operation (plusEqOp)
                template<class Type>
                tmp<Field<Type>> mapSrcToTgt
                (
                    const Field<Type>& srcFld
                ) const;

                //- Convenience function to map a tmp field to the tgt mesh
                //  with a default operation (plusEqOp)
                template<class Type>
                tmp<Field<Type>> mapSrcToTgt
                (
                    const tmp<Field<Type>>& tsrcFld
                ) const;


            // Target-to-source field mapping

                //- Map field from tgt to src mesh with defined operation
                //  Values passed in via 'result' are used to initialise the
                //  return value
                template<class Type, class CombineOp>
                void mapTgtToSrc
                (
                    const UList<Type>& tgtFld,
                    const CombineOp& cop,
                    List<Type>& result
                ) const;

                //- Map extrapolated field (using gradient) from tgt to src
                //  mesh with defined operation. Falls back to non-extrapolated
                //  mapping (above) if not constructed with method that supports
                //  getting offset vectors. Extrapolation only for internal
                //  values. Values passed in via 'result' are used to
                //  initialise the return value
                template<class Type, class CombineOp>
                void mapTgtToSrc
                (
                    const UList<Type>& srcField,
                    const UList<typename outerProduct<vector, Type>::type>&,
                    const CombineOp& cop,
                    List<Type>& result
                ) const;

                //- Return the tgt field mapped to the src mesh with a defined
                //  operation. Initial values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<Field<Type>> mapTgtToSrc
                (
                    const Field<Type>& tgtFld,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a tmp field to the src mesh
                //  with a defined operation
                template<class Type, class CombineOp>
                tmp<Field<Type>> mapTgtToSrc
                (
                    const tmp<Field<Type>>& ttgtFld,
                    const CombineOp& cop
                ) const;

                //- Convenience function to map a field to the src mesh with a
                //  default operation (plusEqOp)
                template<class Type>
                tmp<Field<Type>> mapTgtToSrc
                (
                    const Field<Type>& tgtFld
                ) const;

                //- Convenience function to map a tmp field to the src mesh
                //  with a default operation (plusEqOp)
                template<class Type>
                tmp<Field<Type>> mapTgtToSrc
                (
                    const tmp<Field<Type>>& ttgtFld
                ) const;


            // Source-to-target volume field mapping

                //- Interpolate a field with a defined operation.  Values
                //  passed in via 'result' are used to initialise the return
                //  value. Optionally uses gradient correction (internal
                //  field only) if interpolationMethod supports it
                template<class Type, class CombineOp>
                void mapSrcToTgt
                (
                    const VolumeField<Type>& field,
                    const CombineOp& cop,
                    VolumeField<Type>& result,
                    const bool secondOrder = true
                ) const;

                //- Interpolate a field with a defined operation.  The initial
                //  values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<VolumeField<Type>> mapSrcToTgt
                (
                    const VolumeField<Type>& field,
                    const CombineOp& cop,
                    const bool secondOrder = true
                ) const;

                //- Interpolate a tmp field with a defined operation.  The
                //  initial values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<VolumeField<Type>> mapSrcToTgt
                (
                    const tmp<VolumeField<Type>>& tfield,
                    const CombineOp& cop,
                    const bool secondOrder = true
                ) const;

                //- Convenience function to map a field with a default
                //  operation (plusEqOp)
                template<class Type>
                tmp<VolumeField<Type>> mapSrcToTgt
                (
                    const VolumeField<Type>& field,
                    const bool secondOrder = true
                ) const;

                //- Convenience function to map a tmp field with a default
                //  operation (plusEqOp)
                template<class Type>
                tmp<VolumeField<Type>> mapSrcToTgt
                (
                    const tmp<VolumeField<Type>>& tfield,
                    const bool secondOrder = true
                ) const;


            // Target-to-source volume field mapping

                //- Interpolate a field with a defined operation.  Values
                //  passed in via 'result' are used to initialise the return
                //  value. Optionally uses gradient correction (internal
                //  field only) if interpolationMethod supports it
                template<class Type, class CombineOp>
                void mapTgtToSrc
                (
                    const VolumeField<Type>& field,
                    const CombineOp& cop,
                    VolumeField<Type>& result,
                    const bool secondOrder = true
                ) const;

                //- Interpolate a field with a defined operation.  The initial
                //  values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<VolumeField<Type>> mapTgtToSrc
                (
                    const VolumeField<Type>& field,
                    const CombineOp& cop,
                    const bool secondOrder = true
                ) const;

                //- Interpolate a tmp field with a defined operation.  The
                //  initial values of the result are set to zero
                template<class Type, class CombineOp>
                tmp<VolumeField<Type>> mapTgtToSrc
                (
                    const tmp<VolumeField<Type>>&
                        tfield,
                    const CombineOp& cop,
                    const bool secondOrder = true
                ) const;

                //- Convenience function to map a field with a default
                //  operation (plusEqOp)
                template<class Type>
                tmp<VolumeField<Type>> mapTgtToSrc
                (
                    const VolumeField<Type>& field,
                    const bool secondOrder = true
                ) const;

                //- Convenience function to map a tmp field with a default
                //  operation (plusEqOp)
                template<class Type>
                tmp<VolumeField<Type>> mapTgtToSrc
                (
                    const tmp<VolumeField<Type>>& tfield,
                    const bool secondOrder = true
                ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Disable gradient 2nd order correction for tensor types

template<>
void meshToMesh::mapInternalSrcToTgt
(
    const VolumeField<sphericalTensor>&,
    const plusEqOp<sphericalTensor>&,
    VolumeField<sphericalTensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalSrcToTgt
(
    const VolumeField<sphericalTensor>&,
    const minusEqOp<sphericalTensor>&,
    VolumeField<sphericalTensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalSrcToTgt
(
    const VolumeField<symmTensor>&,
    const plusEqOp<symmTensor>&,
    VolumeField<symmTensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalSrcToTgt
(
    const VolumeField<symmTensor>&,
    const minusEqOp<symmTensor>&,
    VolumeField<symmTensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalSrcToTgt
(
    const VolumeField<tensor>&,
    const plusEqOp<tensor>&,
    VolumeField<tensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalSrcToTgt
(
    const VolumeField<tensor>&,
    const minusEqOp<tensor>&,
    VolumeField<tensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalTgtToSrc
(
    const VolumeField<sphericalTensor>&,
    const plusEqOp<sphericalTensor>&,
    VolumeField<sphericalTensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalTgtToSrc
(
    const VolumeField<sphericalTensor>&,
    const minusEqOp<sphericalTensor>&,
    VolumeField<sphericalTensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalTgtToSrc
(
    const VolumeField<symmTensor>&,
    const plusEqOp<symmTensor>&,
    VolumeField<symmTensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalTgtToSrc
(
    const VolumeField<symmTensor>&,
    const minusEqOp<symmTensor>&,
    VolumeField<symmTensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalTgtToSrc
(
    const VolumeField<tensor>&,
    const plusEqOp<tensor>&,
    VolumeField<tensor>&,
    const bool
) const;
template<>
void meshToMesh::mapInternalTgtToSrc
(
    const VolumeField<tensor>&,
    const minusEqOp<tensor>&,
    VolumeField<tensor>&,
    const bool
) const;


// Disable fvPatchField value override after rmap

template<>
void meshToMesh::mapAndOpSrcToTgt
(
    const AMIPatchToPatchInterpolation& AMI,
    const Field<scalar>& srcField,
    Field<scalar>& tgtField,
    const plusEqOp<scalar>& cop
) const;
template<>
void meshToMesh::mapAndOpSrcToTgt
(
    const AMIPatchToPatchInterpolation& AMI,
    const Field<vector>& srcField,
    Field<vector>& tgtField,
    const plusEqOp<vector>& cop
) const;
template<>
void meshToMesh::mapAndOpSrcToTgt
(
    const AMIPatchToPatchInterpolation& AMI,
    const Field<sphericalTensor>& srcField,
    Field<sphericalTensor>& tgtField,
    const plusEqOp<sphericalTensor>& cop
) const;
template<>
void meshToMesh::mapAndOpSrcToTgt
(
    const AMIPatchToPatchInterpolation& AMI,
    const Field<symmTensor>& srcField,
    Field<symmTensor>& tgtField,
    const plusEqOp<symmTensor>& cop
) const;
template<>
void meshToMesh::mapAndOpSrcToTgt
(
    const AMIPatchToPatchInterpolation& AMI,
    const Field<tensor>& srcField,
    Field<tensor>& tgtField,
    const plusEqOp<tensor>& cop
) const;


template<>
void meshToMesh::mapAndOpTgtToSrc
(
    const AMIPatchToPatchInterpolation& AMI,
    Field<scalar>& srcField,
    const Field<scalar>& tgtField,
    const plusEqOp<scalar>& cop
) const;
template<>
void meshToMesh::mapAndOpTgtToSrc
(
    const AMIPatchToPatchInterpolation& AMI,
    Field<vector>& srcField,
    const Field<vector>& tgtField,
    const plusEqOp<vector>& cop
) const;
template<>
void meshToMesh::mapAndOpTgtToSrc
(
    const AMIPatchToPatchInterpolation& AMI,
    Field<sphericalTensor>& srcField,
    const Field<sphericalTensor>& tgtField,
    const plusEqOp<sphericalTensor>& cop
) const;
template<>
void meshToMesh::mapAndOpTgtToSrc
(
    const AMIPatchToPatchInterpolation& AMI,
    Field<symmTensor>& srcField,
    const Field<symmTensor>& tgtField,
    const plusEqOp<symmTensor>& cop
) const;
template<>
void meshToMesh::mapAndOpTgtToSrc
(
    const AMIPatchToPatchInterpolation& AMI,
    Field<tensor>& srcField,
    const Field<tensor>& tgtField,
    const plusEqOp<tensor>& cop
) const;


} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "meshToMesh/meshToMeshI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "meshToMesh/meshToMeshTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
