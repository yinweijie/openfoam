/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2017 OpenFOAM Foundation
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
    Foam::cellShapeControlMesh

Description

SourceFiles
    cellShapeControlMeshI.H
    cellShapeControlMesh.C

\*---------------------------------------------------------------------------*/

#ifndef cellShapeControlMesh_H
#define cellShapeControlMesh_H

#include "db/Time/TimeOpenFOAM.H"
#include "primitives/Scalar/scalar/scalar.H"
#include "meshes/primitiveShapes/point/point.H"
#include "primitives/Tensor/floats/tensor.H"
#include "primitives/triad/triad.H"
#include "primitives/strings/fileName/fileName.H"
#include "searchableSurfaces/searchableSurfaces/searchableSurfaces.H"
#include "conformationSurfaces/conformationSurfaces.H"
#include "DelaunayMesh/DistributedDelaunayMesh.H"
#include "conformalVoronoiMesh/CGALTriangulation3Ddefs.H"
#include "backgroundMeshDecomposition/backgroundMeshDecomposition.H"
#include "meshes/boundBox/boundBox.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class cellSizeAndAlignmentControls;

/*---------------------------------------------------------------------------*\
                      Class cellShapeControlMesh Declaration
\*---------------------------------------------------------------------------*/

class cellShapeControlMesh
:
    public DistributedDelaunayMesh<CellSizeDelaunay>
{
public:

        typedef CellSizeDelaunay::Cell_handle      Cell_handle;
        typedef CellSizeDelaunay::Vertex_handle    Vertex_handle;
        typedef CellSizeDelaunay::Point            Point;


private:

        // Private Data

        const Time& runTime_;

        mutable Cell_handle oldCellHandle_;

        /// const scalar defaultCellSize_;


    // Private Member Functions

        //- No copy construct
        cellShapeControlMesh(const cellShapeControlMesh&) = delete;

        //- No copy assignment
        void operator=(const cellShapeControlMesh&) = delete;


public:

    //- Runtime type information
    ClassName("cellShapeControlMesh");

    //- Return the mesh sub-directory name (usually "cellShapeControlMesh")
    static word meshSubDir;


    // Constructors

        //- Construct from runTime
        explicit cellShapeControlMesh(const Time& runTime);


    //- Destructor
    ~cellShapeControlMesh() = default;


    // Member Functions

        // Query

            const Time& time() const
            {
                return runTime_;
            }

            //- Calculate and return the barycentric coordinates for
            //  interpolating quantities on the background mesh
            void barycentricCoords
            (
                const Foam::point& pt,
                barycentric& bary,
                Cell_handle& ch
            ) const;

            boundBox bounds() const;


        // Edit

            label removePoints();

            //- Get the centres of all the tets
            tmp<pointField> cellCentres() const;

            inline Vertex_handle insert
            (
                const Foam::point& pt,
                const scalar& size,
                const triad& alignment,
                const Foam::indexedVertexEnum::vertexType type = Vb::vtInternal
            );

            inline Vertex_handle insertFar
            (
                const Foam::point& pt
            );

            void distribute(const backgroundMeshDecomposition& decomposition);

            tensorField dumpAlignments() const;

            void writeTriangulation();

            void write() const;

            label estimateCellCount
            (
                const autoPtr<backgroundMeshDecomposition>& decomposition
            ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "cellShapeControl/cellShapeControlMesh/cellShapeControlMeshI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#endif

// ************************************************************************* //
