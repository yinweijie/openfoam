/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenCFD Ltd.
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
    Foam::solidBodyDisplacementLaplacianFvMotionSolver

Group
    grpMeshMotionSolvers

Description
    Applies Laplacian displacement solving on top of a transformation of
    the initial points using a solid-body motion.

See also
    Foam::displacementLaplacian

SourceFiles
    solidBodyDisplacementLaplacianFvMotionSolver.C

\*---------------------------------------------------------------------------*/

#ifndef solidBodyDisplacementLaplacianFvMotionSolver_H
#define solidBodyDisplacementLaplacianFvMotionSolver_H

#include "motionSolvers/displacement/displacement/displacementMotionSolver.H"
#include "fvMotionSolvers/fvMotionSolver/fvMotionSolver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward class declarations
class motionInterpolation;
class motionDiffusivity;
class solidBodyMotionFunction;

/*---------------------------------------------------------------------------*\
        Class solidBodyDisplacementLaplacianFvMotionSolver Declaration
\*---------------------------------------------------------------------------*/

class solidBodyDisplacementLaplacianFvMotionSolver
:
    public displacementMotionSolver,
    public fvMotionSolver
{
    // Private data

        //- Motion function
        autoPtr<solidBodyMotionFunction> SBMFPtr_;

        //- Cell-centre motion field
        mutable volVectorField cellDisplacement_;

        //- Optionally read point-position field. Used only for position
        //  boundary conditions.
        mutable autoPtr<pointVectorField> pointLocation_;

        //- Interpolation used to transfer cell displacement to the points
        autoPtr<motionInterpolation> interpolationPtr_;

        //- Diffusivity used to control the motion
        autoPtr<motionDiffusivity> diffusivityPtr_;

        //- Frozen points (that are not on patches). -1 or points that are
        //  fixed to be at points0_ location
        label frozenPointsZone_;


    // Private Member Functions

        //- No copy construct
        solidBodyDisplacementLaplacianFvMotionSolver
        (
            const solidBodyDisplacementLaplacianFvMotionSolver&
        ) = delete;

        //- No copy assignment
        void operator=
        (
            const solidBodyDisplacementLaplacianFvMotionSolver&
        ) = delete;


public:

    //- Runtime type information
    TypeName("solidBodyDisplacementLaplacian");


    // Constructors

        //- Construct from polyMesh and IOdictionary
        solidBodyDisplacementLaplacianFvMotionSolver
        (
            const polyMesh&,
            const IOdictionary&
        );

        //- Construct from components
        solidBodyDisplacementLaplacianFvMotionSolver
        (
            const polyMesh& mesh,
            const IOdictionary& dict,
            const pointVectorField& pointDisplacement,
            const pointIOField& points0
        );


    //- Destructor
    ~solidBodyDisplacementLaplacianFvMotionSolver();


    // Member Functions

        //- Return reference to the diffusivity field
        motionDiffusivity& diffusivity();

        //- Return point location obtained from the current motion field
        virtual tmp<pointField> curPoints() const;

        //- Solve for motion
        virtual void solve();

        //- Update topology
        virtual void updateMesh(const mapPolyMesh&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
