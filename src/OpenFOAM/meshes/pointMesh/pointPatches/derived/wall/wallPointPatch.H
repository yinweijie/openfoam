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
    Foam::wallPointPatch

Description
    Foam::wallPointPatch

SourceFiles
    wallPointPatch.C

\*---------------------------------------------------------------------------*/

#ifndef wallPointPatch_H
#define wallPointPatch_H

#include "meshes/pointMesh/pointPatches/facePointPatch/facePointPatch.H"
#include "meshes/polyMesh/polyPatches/derived/wall/wallPolyPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class wallPointPatch Declaration
\*---------------------------------------------------------------------------*/

class wallPointPatch
:
    public facePointPatch
{

public:

    //- Runtime type information
    TypeName(wallPolyPatch::typeName_());


    // Constructors

        //- Construct from polyPatch
        wallPointPatch
        (
            const polyPatch& patch,
            const pointBoundaryMesh& bm
        )
        :
            facePointPatch(patch, bm)
        {}
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
