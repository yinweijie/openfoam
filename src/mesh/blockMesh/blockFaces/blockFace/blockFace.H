/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
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
    Foam::blockFace

Description
    Define a curved face.

SourceFiles
    blockFace.C

\*---------------------------------------------------------------------------*/

#ifndef blockFace_H
#define blockFace_H

#include "searchableSurfaces/searchableSurfaces/searchableSurfaces.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class blockDescriptor;
class blockFace;

Ostream& operator<<(Ostream&, const blockFace&);

/*---------------------------------------------------------------------------*\
                         Class blockFace Declaration
\*---------------------------------------------------------------------------*/

class blockFace
{
protected:

    // Protected data

        //- Block face vertices
        const face vertices_;


public:

    //- Runtime type information
    TypeName("blockFace");


    // Declare run-time constructor selection tables

        declareRunTimeSelectionTable
        (
            autoPtr,
            blockFace,
            Istream,
            (
                const dictionary& dict,
                const label index,
                const searchableSurfaces& geometry,
                Istream& is
            ),
            (dict, index, geometry, is)
        );


    // Constructors

        //- Construct from face vertices
        explicit blockFace(const face& vertices);

        //- Construct from Istream
        blockFace
        (
            const dictionary& dict,
            const label index,
            Istream& is
        );

        //- Clone function
        virtual autoPtr<blockFace> clone() const;

        //- New function which constructs and returns pointer to a blockFace
        static autoPtr<blockFace> New
        (
            const dictionary& dict,
            const label index,
            const searchableSurfaces& geometry,
            Istream&
        );

        //- Class used for the read-construction of
        //  PtrLists of blockFace
        class iNew
        {
            const dictionary& dict_;
            const searchableSurfaces& geometry_;
            mutable label index_;

        public:

            iNew(const dictionary& dict, const searchableSurfaces& geometry)
            :
                dict_(dict),
                geometry_(geometry),
                index_(0)
            {}

            autoPtr<blockFace> operator()(Istream& is) const
            {
                return blockFace::New(dict_, index_++, geometry_, is);
            }
        };


    //- Destructor
    virtual ~blockFace() = default;


    // Member Functions

        //- Return block face vertices
        inline const face& vertices() const;

        //- Compare with given blockFace
        inline bool compare(const blockFace&) const;

        //- Compare with the given block and block face
        inline bool compare(const face& vertices) const;

        virtual void project
        (
            const blockDescriptor&,
            const label blockFacei,
            pointField& points
        ) const = 0;

        //- Write face with variable backsubstitution
        void write(Ostream&, const dictionary&) const;


    // Ostream Operator

        friend Ostream& operator<<(Ostream&, const blockFace&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "blockFaces/blockFace/blockFaceI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
