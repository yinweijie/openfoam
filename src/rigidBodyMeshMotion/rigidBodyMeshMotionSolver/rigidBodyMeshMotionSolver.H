/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 OpenFOAM Foundation
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
    Foam::rigidBodyMeshMotionSolver

Description
    Rigid-body mesh motion solver for fvMesh.

    Applies septernion interpolation of movement as function of distance to the
    object surface.

SourceFiles
    rigidBodyMeshMotionSolver.C

\*---------------------------------------------------------------------------*/

#ifndef rigidBodyMeshMotionSolver_H
#define rigidBodyMeshMotionSolver_H

#include "motionSolvers/displacement/displacement/displacementMotionSolver.H"
#include "rigidBodyMotion/rigidBodyMotion.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                  Class rigidBodyMeshMotionSolver Declaration
\*---------------------------------------------------------------------------*/

class rigidBodyMeshMotionSolver
:
    public motionSolver
{
    //- Class containing the patches and point motion weighting for each body
    class bodyMesh
    {
        //- Name of the body
        const word name_;

        //- ID of the body in the RBD::rigidBodyMotion
        const label bodyID_;

        //- List of mesh patches associated with this body
        const wordRes patches_;

        //- Patches to integrate forces
        const labelHashSet patchSet_;


    public:

        friend class rigidBodyMeshMotionSolver;

        bodyMesh
        (
            const polyMesh& mesh,
            const word& name,
            const label bodyID,
            const dictionary& dict
        );
    };


    // Private data

        //- Rigid-body model
        RBD::rigidBodyMotion model_;

        //- List of the bodyMeshes containing the patches and point motion
        //  weighting for each body
        PtrList<bodyMesh> bodyMeshes_;

        //- Test-mode in which only the gravitational body-force is applied
        bool test_;

        //- Reference density required by the forces object for
        //  incompressible calculations, required if rho == rhoInf
        scalar rhoInf_;

        //- Name of density field, optional unless used for an
        //  incompressible simulation, when this needs to be specified
        //  as rhoInf
        word rhoName_;

        //- Current time index (used for updating)
        label curTimeIndex_;

        autoPtr<motionSolver> meshSolverPtr_;

        displacementMotionSolver& meshSolver_;


    // Private Member Functions

        //- No copy construct
        rigidBodyMeshMotionSolver(const rigidBodyMeshMotionSolver&) = delete;

        //- No copy assignment
        void operator=(const rigidBodyMeshMotionSolver&) = delete;


public:

    //- Runtime type information
    TypeName("rigidBodyMotionSolver");


    // Constructors

        //- Construct from polyMesh and IOdictionary
        rigidBodyMeshMotionSolver
        (
            const polyMesh&,
            const IOdictionary& dict
        );


    //- Destructor
    ~rigidBodyMeshMotionSolver() = default;


    // Member Functions

        //- Return point location obtained from the current motion field
        virtual tmp<pointField> curPoints() const;

        //- Solve for motion
        virtual void solve();

        //- Write state using stream options
        virtual bool writeObject
        (
            IOstreamOption streamOpt,
            const bool writeOnProc
        ) const;

        //- Read dynamicMeshDict dictionary
        virtual bool read();

        //- Update local data for geometry changes
        virtual void movePoints(const pointField&);

        //-  Update local data for topology changes
        virtual void updateMesh(const mapPolyMesh&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
