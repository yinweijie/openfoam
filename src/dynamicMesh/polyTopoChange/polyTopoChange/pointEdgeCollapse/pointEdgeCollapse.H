/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2016 OpenFOAM Foundation
    Copyright (C) 2019-2020 OpenCFD Ltd.
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
    Foam::pointEdgeCollapse

Description
    Determines length of string of edges walked to point.

SourceFiles
    pointEdgeCollapseI.H
    pointEdgeCollapse.C

\*---------------------------------------------------------------------------*/

#ifndef pointEdgeCollapse_H
#define pointEdgeCollapse_H

#include "meshes/primitiveShapes/point/point.H"
#include "primitives/Tensor/floats/tensor.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class polyPatch;
class polyMesh;
class pointEdgeCollapse;

Istream& operator>>(Istream&, pointEdgeCollapse&);
Ostream& operator<<(Ostream&, const pointEdgeCollapse&);

/*---------------------------------------------------------------------------*\
                      Class pointEdgeCollapse Declaration
\*---------------------------------------------------------------------------*/

class pointEdgeCollapse
{
    // Private Data

        //- Collapse location
        point collapsePoint_;

        //- Collapse string index
        label collapseIndex_;

        //- Priority of the collapse
        label collapsePriority_;


    // Private Member Functions

        //- Evaluate distance to point.
        template<class TrackingData>
        inline bool update
        (
            const pointEdgeCollapse& w2,
            const scalar tol,
            TrackingData& td
        );

        //- Check for same coordinate
        inline bool samePoint(const point& pt) const;

public:

    // Constructors

        //- Default construct
        inline pointEdgeCollapse();

        //- Construct from components
        inline pointEdgeCollapse
        (
            const point& collapsePoint,
            const label collapseIndex,
            const label collapsePriority
        );


    // Member Functions

        // Access

            const point& collapsePoint() const
            {
                return collapsePoint_;
            }

            label collapseIndex() const
            {
                return collapseIndex_;
            }

            label collapsePriority() const
            {
                return collapsePriority_;
            }


        // Needed by MeshWave

            //- Changed or contains original (invalid) value
            template<class TrackingData>
            inline bool valid(TrackingData& td) const;

            //- Convert origin to relative vector to leaving point
            //  (= point coordinate)
            template<class TrackingData>
            inline void leaveDomain
            (
                const polyPatch& patch,
                const label patchPointi,
                const point& pos,
                TrackingData& td
            );

            //- Convert relative origin to absolute by adding entering point
            template<class TrackingData>
            inline void enterDomain
            (
                const polyPatch& patch,
                const label patchPointi,
                const point& pos,
                TrackingData& td
            );

            //- Apply rotation matrix to origin
            template<class TrackingData>
            inline void transform
            (
                const tensor& rotTensor,
                TrackingData& td
            );

            //- Influence of edge on point
            template<class TrackingData>
            inline bool updatePoint
            (
                const polyMesh& mesh,
                const label pointi,
                const label edgeI,
                const pointEdgeCollapse& edgeInfo,
                const scalar tol,
                TrackingData& td
            );

            //- Influence of different value on same point.
            //  Merge new and old info.
            template<class TrackingData>
            inline bool updatePoint
            (
                const polyMesh& mesh,
                const label pointi,
                const pointEdgeCollapse& newPointInfo,
                const scalar tol,
                TrackingData& td
            );

            //- Influence of different value on same point.
            //  No information about current position whatsoever.
            template<class TrackingData>
            inline bool updatePoint
            (
                const pointEdgeCollapse& newPointInfo,
                const scalar tol,
                TrackingData& td
            );

            //- Influence of point on edge.
            template<class TrackingData>
            inline bool updateEdge
            (
                const polyMesh& mesh,
                const label edgeI,
                const label pointi,
                const pointEdgeCollapse& pointInfo,
                const scalar tol,
                TrackingData& td
            );

            //- Test for equality, with TrackingData
            template<class TrackingData>
            inline bool equal(const pointEdgeCollapse&, TrackingData&) const;


    // Member Operators

        //- Test for equality
        inline bool operator==(const pointEdgeCollapse&) const;

        //- Test for inequality
        inline bool operator!=(const pointEdgeCollapse&) const;


    // IOstream Operators

        friend Ostream& operator<<(Ostream&, const pointEdgeCollapse&);
        friend Istream& operator>>(Istream&, pointEdgeCollapse&);
};


// * * * * * * * * * * * * * * * * * Traits  * * * * * * * * * * * * * * * * //

//- Contiguous data for pointEdgeCollapse
template<> struct is_contiguous<pointEdgeCollapse> : std::true_type {};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "polyTopoChange/polyTopoChange/pointEdgeCollapse/pointEdgeCollapseI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
