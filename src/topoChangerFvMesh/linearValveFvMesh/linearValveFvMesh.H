/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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
    Foam::linearValveFvMesh

Description
    A sliding linear valve

SourceFiles
    linearValveFvMesh.C

\*---------------------------------------------------------------------------*/

#ifndef linearValveFvMesh_H
#define linearValveFvMesh_H

#include "topoChangerFvMesh/topoChangerFvMesh.H"
#include "motionSolvers/motionSolver/motionSolver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class linearValveFvMesh Declaration
\*---------------------------------------------------------------------------*/

class linearValveFvMesh
:
    public topoChangerFvMesh
{
    // Private data

        //- Motion dictionary
        dictionary motionDict_;

        //- Motion solver
        autoPtr<motionSolver> msPtr_;


    // Private Member Functions

        //- No copy construct
        linearValveFvMesh(const linearValveFvMesh&) = delete;

        //- No copy assignment
        void operator=(const linearValveFvMesh&) = delete;


        //- Add linearValve zones and modifiers
        void addZonesAndModifiers();

        //- Make sliding modifiers live
        void makeSlidersDead();

        //- Make sliding modifiers live
        void makeSlidersLive();

        //- Return true if sliders are attached
        bool attached() const;


public:

    //- Runtime type information
    TypeName("linearValveFvMesh");


    // Constructors

        //- Construct from database
        explicit linearValveFvMesh(const IOobject& io);


    //- Destructor
    virtual ~linearValveFvMesh();


    // Member Functions

        //- Update the mesh for both mesh motion and topology change
        virtual bool update();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
