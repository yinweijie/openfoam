/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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
    Foam::polyMeshModifier

Description
    Virtual base class for mesh modifiers.

SourceFiles
    polyMeshModifier.C

\*---------------------------------------------------------------------------*/

#ifndef polyMeshModifier_H
#define polyMeshModifier_H

#include "primitives/ints/label/label.H"
#include "primitives/strings/word/word.H"
#include "primitives/bools/Switch/Switch.H"
#include "db/typeInfo/typeInfo.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"
#include "memory/autoPtr/autoPtr.H"
#include "meshes/primitiveShapes/point/pointField.H"
#include "meshes/meshShapes/face/faceList.H"
#include "meshes/meshShapes/cell/cellList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class polyTopoChanger;
class polyTopoChange;
class mapPolyMesh;
class polyMeshModifier;

Ostream& operator<<(Ostream&, const polyMeshModifier&);


/*---------------------------------------------------------------------------*\
                       Class polyMeshModifier Declaration
\*---------------------------------------------------------------------------*/

class polyMeshModifier
{
    // Private data

        //- Name of modifier
        word name_;

        //- Index of modifier
        label index_;

        //- Reference to morph engine
        const polyTopoChanger& topoChanger_;

        //- Activation switch
        mutable Switch active_;


    // Private Member Functions

        //- No copy construct
        polyMeshModifier(const polyMeshModifier&) = delete;

        //- No copy assignment
        void operator=(const polyMeshModifier&) = delete;


public:

    // Static data members

        //- Runtime type information
        TypeName("meshModifier");


    // Declare run-time constructor selection tables

        declareRunTimeSelectionTable
        (
            autoPtr,
            polyMeshModifier,
            dictionary,
            (
                const word& name,
                const dictionary& dict,
                const label index,
                const polyTopoChanger& mme
            ),
            (name, dict, index, mme)
        );


    // Constructors

        //- Construct from components
        polyMeshModifier
        (
            const word& name,
            const label index,
            const polyTopoChanger& mme,
            const bool act
        );


    // Selectors

        //- Select constructed from dictionary
        static autoPtr<polyMeshModifier> New
        (
            const word& name,
            const dictionary& dict,
            const label index,
            const polyTopoChanger& mme
        );


    //- Destructor
    virtual ~polyMeshModifier() = default;


    // Member Functions

        //- Return name of this modifier
        const word& name() const
        {
            return name_;
        }

        //- Return the index of this modifier
        label index() const
        {
            return index_;
        }

        //- Return reference to morph engine
        const polyTopoChanger& topoChanger() const;

        //- Check for topology change
        virtual bool changeTopology() const = 0;

        //- Insert the topological change instructions
        virtual void setRefinement(polyTopoChange&) const = 0;

        //- Modify motion points to comply with the topological change
        virtual void modifyMotionPoints(pointField& motionPoints) const = 0;

        //- Force recalculation of locally stored data on topological change
        virtual void updateMesh(const mapPolyMesh&) = 0;


        // Activation and deactivation

            //- If modifier activate?
            Switch active() const
            {
                return active_;
            }

            //- Activate mesh modifier
            void enable() const
            {
                active_ = true;
            }

            //- Activate mesh modifier
            void disable() const
            {
                active_ = false;
            }


        //- Write
        virtual void write(Ostream&) const = 0;

        //- Write dictionary
        virtual void writeDict(Ostream&) const = 0;


    // Ostream Operator

        friend Ostream& operator<<(Ostream&, const polyMeshModifier&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
