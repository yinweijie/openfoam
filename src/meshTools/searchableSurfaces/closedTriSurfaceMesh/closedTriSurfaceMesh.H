/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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
    Foam::closedTriSurfaceMesh

Description
    A triSurfaceMesh where it is forced to check volumeTypes, used for surfaces
    that are topologically non-manifold (small holes or multiple parts) but are
    geometrically essentially closed

SourceFiles
    closedTriSurfaceMesh.C

\*---------------------------------------------------------------------------*/

#ifndef closedTriSurfaceMesh_H
#define closedTriSurfaceMesh_H

#include "searchableSurfaces/triSurfaceMesh/triSurfaceMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class closedTriSurfaceMesh Declaration
\*---------------------------------------------------------------------------*/

class closedTriSurfaceMesh
:
    public triSurfaceMesh
{

    // Private Member Functions

        //- No copy construct
        closedTriSurfaceMesh(const closedTriSurfaceMesh&) = delete;

        //- No copy assignment
        void operator=(const closedTriSurfaceMesh&) = delete;


public:

    //- Runtime type information
    TypeName("closedTriSurfaceMesh");


    // Constructors

        //- Construct from triSurface
        closedTriSurfaceMesh(const IOobject&, const triSurface&);

        //- Construct read.
        closedTriSurfaceMesh(const IOobject& io);

        //- Construct from IO and dictionary (used by searchableSurface).
        //  Dictionary may contain a 'scale' entry (eg, 0.001: mm to m)
        closedTriSurfaceMesh
        (
            const IOobject& io,
            const dictionary& dict
        );


    //- Destructor
    virtual ~closedTriSurfaceMesh() = default;


    // Member Functions

        //- Whether supports volume type, forcing to true to force getVolumeType
        //  queries for this type
        virtual bool hasVolumeType() const
        {
            return true;
        }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
