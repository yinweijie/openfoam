/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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
    Foam::degenerateMatcher

Description
    Collection of all hex degenerate matchers (hex, wedge, prism etc.)

    Has static member function to match a shape.

See also
    cellMatcher

SourceFiles
    degenerateMatcher.C

\*---------------------------------------------------------------------------*/

#ifndef degenerateMatcher_H
#define degenerateMatcher_H

#include "meshes/meshShapes/cellMatcher/hexMatcher.H"
#include "meshes/meshShapes/cellMatcher/wedgeMatcher.H"
#include "meshes/meshShapes/cellMatcher/prismMatcher.H"
#include "meshes/meshShapes/cellMatcher/tetWedgeMatcher.H"
#include "meshes/meshShapes/cellMatcher/pyrMatcher.H"
#include "meshes/meshShapes/cellMatcher/tetMatcher.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                      Class degenerateMatcher Declaration
\*---------------------------------------------------------------------------*/

class degenerateMatcher
{
    // Static Data Members

        //- Matchers for all degenerate hex shapes
        static hexMatcher hex;
        static wedgeMatcher wedge;
        static prismMatcher prism;
        static tetWedgeMatcher tetWedge;
        static pyrMatcher pyr;
        static tetMatcher tet;


    // Static Functions

        //- Recognize basic shape
        static cellShape match
        (
            const faceList& faces,
            const labelList& faceOwner,
            const label celli,
            const labelList& cellFaces
        );

public:

    // Static Functions

        //- Recognize shape given faces of a cell
        static cellShape match(const faceList& faces);

        //- Recognize given uncollapsed shape (usually hex) with duplicate
        //- vertices. cellShape just used to extract faces.
        static cellShape match(const cellShape& shape);

        //- Recognize shape given mesh and celli
        static cellShape match(const primitiveMesh& mesh, const label celli);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
