/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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
    Foam::pointFieldReconstructor

Description
    Point field reconstructor.

SourceFiles
    pointFieldReconstructor.C
    pointFieldReconstructorTemplates.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_pointFieldReconstructor_H
#define Foam_pointFieldReconstructor_H

#include "meshes/pointMesh/pointMesh.H"
#include "fields/GeometricFields/pointFields/pointFields.H"
#include "fields/pointPatchFields/pointPatchField/pointPatchFieldMapperPatchRef.H"
#include "db/IOobjectList/IOobjectList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                   Class pointFieldReconstructor Declaration
\*---------------------------------------------------------------------------*/

class pointFieldReconstructor
{
    // Private Data

        //- Reconstructed mesh reference
        const pointMesh& mesh_;

        //- List of processor meshes
        const PtrList<pointMesh>& procMeshes_;

        //- List of processor point addressing lists
        const PtrList<labelIOList>& pointProcAddressing_;

        //- List of processor boundary addressing lists
        const PtrList<labelIOList>& boundaryProcAddressing_;

        //- Point patch addressing
        labelListListList patchPointAddressing_;

        //- Number of fields reconstructed
        label nReconstructed_;


    // Private Member Functions

        //- No copy construct
        pointFieldReconstructor(const pointFieldReconstructor&) = delete;

        //- No copy assignment
        void operator=(const pointFieldReconstructor&) = delete;


public:

    // Public Classes

        class pointPatchFieldReconstructor
        :
            public pointPatchFieldMapper
        {
            label size_;

        public:

            // Constructors

                //- Construct given size
                pointPatchFieldReconstructor(const label size)
                :
                    size_(size)
                {}


            // Member functions

                label size() const
                {
                    return size_;
                }

                bool direct() const
                {
                    return true;
                }

                bool hasUnmapped() const
                {
                    return false;
                }

                const labelUList& directAddressing() const
                {
                    return labelUList::null();
                }
        };


    // Static Data

        //- Output verbosity when writing
        static int verbose_;


    // Constructors

        //- Construct from components
        pointFieldReconstructor
        (
            const pointMesh& mesh,
            const PtrList<pointMesh>& procMeshes,
            const PtrList<labelIOList>& pointProcAddressing,
            const PtrList<labelIOList>& boundaryProcAddressing
        );


    // Member Functions

        //- Return number of fields reconstructed
        label nReconstructed() const noexcept
        {
            return nReconstructed_;
        }


        //- Reconstruct field
        template<class Type>
        tmp<GeometricField<Type, pointPatchField, pointMesh>>
        reconstructField
        (
            const IOobject& fieldObject,
            const PtrList<GeometricField<Type, pointPatchField, pointMesh>>&
        ) const;


        //- Read and reconstruct point field
        template<class Type>
        tmp<GeometricField<Type, pointPatchField, pointMesh>>
        reconstructPointField(const IOobject& fieldObject);

        //- Reconstruct and write specified point fields
        template<class Type>
        label reconstructPointFields
        (
            const UPtrList<const IOobject>& fieldObjects
        );

        //- Reconstruct and write all or selected point fields
        //  An empty wordRes corresponds to select ALL.
        template<class Type>
        label reconstructPointFields
        (
            const IOobjectList& objects,
            const wordRes& selectedFields = wordRes()
        );

        //- Reconstruct and write all known field types
        label reconstructAllFields
        (
            const IOobjectList& objects,
            const wordRes& selectedFields = wordRes()
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "pointFieldReconstructorTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
