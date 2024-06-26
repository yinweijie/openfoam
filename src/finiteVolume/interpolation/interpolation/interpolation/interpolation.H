/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
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
    Foam::interpolation

Description
    Abstract base class for volume field interpolation

\*---------------------------------------------------------------------------*/

#ifndef Foam_interpolation_H
#define Foam_interpolation_H

#include "meshes/meshShapes/face/faceList.H"
#include "fields/volFields/volFieldsFwd.H"
#include "fields/GeometricFields/pointFields/pointFields.H"
#include "memory/autoPtr/autoPtr.H"
#include "db/typeInfo/typeInfo.H"
#include "primitives/Barycentric/barycentric/barycentric.H"
#include "meshes/polyMesh/polyMeshTetDecomposition/tetIndices.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class polyMesh;

/*---------------------------------------------------------------------------*\
                           Class interpolation Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class interpolation
{
protected:

    // Protected Data

        const GeometricField<Type, fvPatchField, volMesh>& psi_;

        const polyMesh& pMesh_;
        const vectorField& pMeshPoints_;
        const faceList& pMeshFaces_;
        const vectorField& pMeshFaceCentres_;
        const vectorField& pMeshFaceAreas_;


public:

    //- Runtime type information
    virtual const word& type() const = 0;


    // Declare run-time constructor selection table

        declareRunTimeSelectionTable
        (
            autoPtr,
            interpolation,
            dictionary,
            (
                const GeometricField<Type, fvPatchField, volMesh>& psi
            ),
            (psi)
        );


    // Selectors

        //- Return a reference to the specified interpolation scheme
        static autoPtr<interpolation<Type>> New
        (
            const word& interpolationType,
            const GeometricField<Type, fvPatchField, volMesh>& psi
        );

        //- Return a reference to the selected interpolation scheme
        static autoPtr<interpolation<Type>> New
        (
            const dictionary& interpolationSchemes,
            const GeometricField<Type, fvPatchField, volMesh>& psi
        );


    // Constructors

        //- Construct from components
        explicit interpolation
        (
            const GeometricField<Type, fvPatchField, volMesh>& psi
        );


    //- Destructor
    virtual ~interpolation() = default;


    // Member Functions

        //- Return the field to be interpolated
        const GeometricField<Type, fvPatchField, volMesh>& psi() const noexcept
        {
            return psi_;
        }

        //- Interpolate field to the given point in the given cell
        virtual Type interpolate
        (
            const vector& position,
            const label celli,
            const label facei = -1
        ) const = 0;

        //- Interpolate field to the given coordinates in the tetrahedron
        //- defined by the given indices.
        //  Calls interpolate function (vector, cell, face) except
        //  where overridden by derived interpolation types.
        virtual Type interpolate
        (
            const barycentric& coordinates,
            const tetIndices& tetIs,
            const label facei = -1
        ) const
        {
            return
                interpolate
                (
                    tetIs.tet(pMesh_).barycentricToPoint(coordinates),
                    tetIs.cell(),
                    facei
                );
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#define makeInterpolationType(SS, Type)                                        \
                                                                               \
defineNamedTemplateTypeNameAndDebug(SS<Type>, 0);                              \
                                                                               \
interpolation<Type>::adddictionaryConstructorToTable<SS<Type>>                 \
    add##SS##Type##ConstructorToTable_;


#define makeInterpolation(SS)                                                  \
                                                                               \
makeInterpolationType(SS, scalar)                                              \
makeInterpolationType(SS, vector)                                              \
makeInterpolationType(SS, sphericalTensor)                                     \
makeInterpolationType(SS, symmTensor)                                          \
makeInterpolationType(SS, tensor)


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "interpolation/interpolation/interpolation/interpolation.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
