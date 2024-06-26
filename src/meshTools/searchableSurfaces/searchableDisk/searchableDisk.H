/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2014-2017 OpenFOAM Foundation
    Copyright (C) 2018 OpenCFD Ltd.
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
    Foam::searchableDisk

Description
    Searching on circular disk given as origin, normal (gets normalised)
    and radius. Optionally it can be an annular ring.

    \heading Dictionary parameters
    \table
        Property    | Description                       | Required | Default
        type        | disk                              | selector |
        origin      | centre of disk                    | yes   |
        normal      | normal vector                     | yes   |
        radius      | disk radius                       | yes   |
        innerRadius | inner disk radius                 | no    | 0
    \endtable

Note
    Longer type name : \c searchableDisk

SourceFiles
    searchableDisk.C

\*---------------------------------------------------------------------------*/

#ifndef searchableDisk_H
#define searchableDisk_H

#include "meshes/primitiveShapes/plane/plane.H"
#include "primitives/ranges/MinMax/MinMax.H"
#include "searchableSurfaces/searchableSurface/searchableSurface.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class searchableDisk Declaration
\*---------------------------------------------------------------------------*/

class searchableDisk
:
    public searchableSurface,
    public plane
{
private:

    // Private Data

        //- Inner/outer radial limits
        const scalarMinMax radialLimits_;

        //- Names of regions
        mutable wordList regions_;


    // Private Member Functions

        //- Inherit findNearest from searchableSurface
        using searchableSurface::findNearest;

        //- Find nearest point on disk
        pointIndexHit findNearest
        (
            const point& sample,
            const scalar nearestDistSqr
        ) const;

        //- Find intersection with disk
        void findLine
        (
            const point& start,
            const point& end,
            pointIndexHit&
        ) const;

        //- No copy construct
        searchableDisk(const searchableDisk&) = delete;

        //- No copy assignment
        void operator=(const searchableDisk&) = delete;

public:

    //- Runtime type information
    TypeName("searchableDisk");


    // Constructors

        //- Construct from components
        searchableDisk
        (
            const IOobject& io,
            const point& originPoint,
            const vector& normalVector,
            const scalar outerRadius,
            const scalar innerRadius = 0
        );

        //- Construct from dictionary (used by searchableSurface)
        searchableDisk
        (
            const IOobject& io,
            const dictionary& dict
        );


    //- Destructor
    virtual ~searchableDisk() = default;


    // Member Functions

        //- Names of regions
        virtual const wordList& regions() const;

        //- Whether supports volume type (below)
        virtual bool hasVolumeType() const
        {
            return false;
        }

        //- What is type of points outside bounds
        virtual volumeType outsideVolumeType() const
        {
            return volumeType::UNKNOWN;
        }

        //- Range of local indices that can be returned.
        virtual label size() const
        {
            return 1;
        }

        //- Get representative set of element coordinates
        //  Usually the element centres (should be of length size()).
        virtual tmp<pointField> coordinates() const
        {
            return tmp<pointField>::New(one{}, origin());
        }

        //- Get bounding spheres (centre and radius squared), one per element.
        //  Any point on element is guaranteed to be inside.
        virtual void boundingSpheres
        (
            pointField& centres,
            scalarField& radiusSqr
        ) const;

        //- Get the points that define the surface.
        virtual tmp<pointField> points() const
        {
            return coordinates();
        }

        //- Does any part of the surface overlap the supplied bound box?
        virtual bool overlaps(const boundBox& bb) const
        {
            NotImplemented;
            return false;
        }


        // Multiple point queries.

            virtual void findNearest
            (
                const pointField& sample,
                const scalarField& nearestDistSqr,
                List<pointIndexHit>&
            ) const;

            virtual void findLine
            (
                const pointField& start,
                const pointField& end,
                List<pointIndexHit>&
            ) const;

            virtual void findLineAny
            (
                const pointField& start,
                const pointField& end,
                List<pointIndexHit>&
            ) const;

            //- Get all intersections in order from start to end.
            virtual void findLineAll
            (
                const pointField& start,
                const pointField& end,
                List<List<pointIndexHit>>&
            ) const;

            //- From a set of points and indices get the region
            virtual void getRegion
            (
                const List<pointIndexHit>&,
                labelList& region
            ) const;

            //- From a set of points and indices get the normal
            virtual void getNormal
            (
                const List<pointIndexHit>&,
                vectorField& normals
            ) const;

            //- Determine type (inside/outside/mixed) for point.
            //  Unknown if cannot be determined (e.g. non-manifold surface)
            virtual void getVolumeType
            (
                const pointField& points,
                List<volumeType>& volType
            ) const;


        // regIOobject implementation

            bool writeData(Ostream&) const
            {
                NotImplemented;
                return false;
            }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
