/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2015 OpenFOAM Foundation
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
    Foam::controlMeshRefinement

Description

SourceFiles
    controlMeshRefinementI.H
    controlMeshRefinement.C
    controlMeshRefinementIO.C

\*---------------------------------------------------------------------------*/

#ifndef controlMeshRefinement_H
#define controlMeshRefinement_H

#include "cellShapeControl/cellShapeControl/cellShapeControl.H"
#include "cellShapeControl/cellShapeControlMesh/cellShapeControlMesh.H"
#include "cellShapeControl/cellSizeAndAlignmentControl/cellSizeAndAlignmentControls.H"
#include "conformationSurfaces/conformationSurfaces.H"
#include "backgroundMeshDecomposition/backgroundMeshDecomposition.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                   Class controlMeshRefinement Declaration
\*---------------------------------------------------------------------------*/

class controlMeshRefinement
{
    // Private data

        const cellShapeControl& shapeController_;

        cellShapeControlMesh& mesh_;

        const cellSizeAndAlignmentControls& sizeControls_;

        const conformationSurfaces& geometryToConformTo_;


    // Private Member Functions

        scalar calcFirstDerivative
        (
            const Foam::point& a,
            const scalar& cellSizeA,
            const Foam::point& b,
            const scalar& cellSizeB
        ) const;

        scalar calcSecondDerivative
        (
            const Foam::point& a,
            const scalar& cellSizeA,
            const Foam::point& midPoint,
            const scalar& cellSizeMid,
            const Foam::point& b,
            const scalar& cellSizeB
        ) const
        {
            return (cellSizeA - 2*cellSizeMid + cellSizeB)/magSqr((a - b)/2);
        }


        bool detectEdge
        (
            const Foam::point& startPt,
            const Foam::point& endPt,
            pointHit& pointFound,
            const scalar tolSqr,
            const scalar secondDerivTolSqr
        ) const;

        pointHit findDiscontinuities(const linePointRef& l) const;

        //- No copy construct
        controlMeshRefinement(const controlMeshRefinement&) = delete;

        //- No copy assignment
        void operator=(const controlMeshRefinement&) = delete;


public:

        //- Runtime type information
        ClassName("controlMeshRefinement");


    // Constructors

        //- Construct null
        controlMeshRefinement(cellShapeControl& shapeController);


    //- Destructor
    ~controlMeshRefinement();


    // Member Functions

        // Edit

            void initialMeshPopulation
            (
                const autoPtr<backgroundMeshDecomposition>& decomposition
            );

            label refineMesh
            (
                const autoPtr<backgroundMeshDecomposition>& decomposition
            );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
