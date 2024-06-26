/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2020 OpenCFD Ltd.
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
    Foam::dynamicMotionSolverFvMesh

Description
    The dynamicMotionSolverFvMesh

SourceFiles
    dynamicMotionSolverFvMesh.C

\*---------------------------------------------------------------------------*/

#ifndef dynamicMotionSolverFvMesh_H
#define dynamicMotionSolverFvMesh_H

#include "dynamicFvMesh/dynamicFvMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class motionSolver;

/*---------------------------------------------------------------------------*\
                           Class dynamicMotionSolverFvMesh Declaration
\*---------------------------------------------------------------------------*/

class dynamicMotionSolverFvMesh
:
    public dynamicFvMesh
{
    // Private data

        autoPtr<motionSolver> motionPtr_;


    // Private Member Functions

        //- No copy construct
        dynamicMotionSolverFvMesh(const dynamicMotionSolverFvMesh&) = delete;

        //- No copy assignment
        void operator=(const dynamicMotionSolverFvMesh&) = delete;


public:

    //- Runtime type information
    TypeName("dynamicMotionSolverFvMesh");


    // Constructors

        //- Construct from IOobject
        dynamicMotionSolverFvMesh(const IOobject& io, const bool doInit=true);

        //- Construct from components without boundary.
        //  Boundary is added using addFvPatches() member function
        dynamicMotionSolverFvMesh
        (
            const IOobject& io,
            pointField&& points,
            faceList&& faces,
            labelList&& allOwner,
            labelList&& allNeighbour,
            const bool syncPar = true
        );


    //- Destructor
    virtual ~dynamicMotionSolverFvMesh() = default;


    // Member Functions

        //- Initialise all non-demand-driven data
        virtual bool init(const bool doInit);

        //- Return the motionSolver
        const motionSolver& motion() const;

        //- Update the mesh for both mesh motion and topology change
        virtual bool update();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
