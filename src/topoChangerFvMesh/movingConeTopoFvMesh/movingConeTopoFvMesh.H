/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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
    Foam::movingConeTopoFvMesh

Description
    Sample topoChangerFvMesh that moves an object in x direction
    and introduces/removes layers.

SourceFiles
    movingConeTopoFvMesh.C

\*---------------------------------------------------------------------------*/

#ifndef movingConeTopoFvMesh_H
#define movingConeTopoFvMesh_H

#include "topoChangerFvMesh/topoChangerFvMesh.H"
#include "motionSolvers/motionSolver/motionSolver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes

/*---------------------------------------------------------------------------*\
                      Class movingConeTopoFvMesh Declaration
\*---------------------------------------------------------------------------*/

class movingConeTopoFvMesh
:
    public topoChangerFvMesh
{
    // Private data

        //- Motion dictionary
        dictionary motionDict_;

        //- Motion velocity amplitude
        vector motionVelAmplitude_;

        //- Motion velocity period
        scalar motionVelPeriod_;

        //- Motion velocity period
        vector curMotionVel_;

        //- Left edge
        scalar leftEdge_;

        //- Current left obstacle position
        scalar curLeft_;

        //- Current right obstacle position
        scalar curRight_;

        //- Vertex motion mask
        scalarField motionMask_;


    // Private Member Functions

        //- No copy construct
        movingConeTopoFvMesh(const movingConeTopoFvMesh&) = delete;

        //- No copy assignment
        void operator=(const movingConeTopoFvMesh&) = delete;


        //- Add mixer zones and modifiers
        void addZonesAndModifiers();

        //- Markup motion vertices
        tmp<scalarField> vertexMarkup
        (
            const pointField& p,
            const scalar curLeft,
            const scalar curRight
        ) const;


public:

    //- Runtime type information
    TypeName("movingConeTopoFvMesh");


    // Constructors

        //- Construct from database
        explicit movingConeTopoFvMesh
        (
            const IOobject& io,
            const bool doInit = true
        );


    //- Destructor
    virtual ~movingConeTopoFvMesh();


    // Member Functions

        //- Initialise all non-demand-driven data
        virtual bool init(const bool doInit);

        //- Update the mesh for both mesh motion and topology change
        virtual bool update();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
