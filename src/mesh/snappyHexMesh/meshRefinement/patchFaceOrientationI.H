/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "meshes/polyMesh/polyMesh.H"
#include "primitives/transform/transform.H"
#include "triSurface/orientedSurface/orientedSurface.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

inline Foam::patchFaceOrientation::patchFaceOrientation()
:
    flipStatus_(orientedSurface::UNVISITED)
{}


inline Foam::patchFaceOrientation::patchFaceOrientation
(
    const label flipStatus
)
:
    flipStatus_(flipStatus)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

inline void Foam::patchFaceOrientation::flip()
{
    if (flipStatus_ == orientedSurface::NOFLIP)
    {
        flipStatus_ = orientedSurface::FLIP;
    }
    else if (flipStatus_ == orientedSurface::FLIP)
    {
        flipStatus_ = orientedSurface::NOFLIP;
    }
}


template<class TrackingData>
inline bool Foam::patchFaceOrientation::valid(TrackingData& td) const
{
    return flipStatus_ != orientedSurface::UNVISITED;
}


template<class TrackingData>
inline void Foam::patchFaceOrientation::transform
(
    const polyMesh& mesh,
    const indirectPrimitivePatch& patch,
    const tensor& rotTensor,
    const scalar tol,
    TrackingData& td
)
{}


template<class TrackingData>
inline bool Foam::patchFaceOrientation::updateEdge
(
    const polyMesh& mesh,
    const indirectPrimitivePatch& patch,
    const label edgeI,
    const label facei,
    const patchFaceOrientation& faceInfo,
    const scalar tol,
    TrackingData& td
)
{
    if (valid(td))
    {
        return false;
    }

    const face& f = patch.localFaces()[facei];
    const edge& e = patch.edges()[edgeI];

    //Pout<< "Updating edge:" << edgeI << " verts:" << e << nl
    //    << "    start:" << patch.localPoints()[e[0]] << nl
    //    << "      end:" << patch.localPoints()[e[1]] << endl;

    patchFaceOrientation consistentInfo(faceInfo);

    // Check how edge relates to face
    if (f.edgeDirection(e) < 0)
    {
        // Create flipped version of faceInfo
        consistentInfo.flip();
    }

    operator=(consistentInfo);
    return true;
}


template<class TrackingData>
inline bool Foam::patchFaceOrientation::updateEdge
(
    const polyMesh& mesh,
    const indirectPrimitivePatch& patch,
    const patchFaceOrientation& edgeInfo,
    const bool sameOrientation,
    const scalar tol,
    TrackingData& td
)
{
    if (valid(td))
    {
        return false;
    }

    // Create (flipped/unflipped) version of edgeInfo
    patchFaceOrientation consistentInfo(edgeInfo);

    if (!sameOrientation)
    {
        consistentInfo.flip();
    }

    operator=(consistentInfo);
    return true;
}


template<class TrackingData>
inline bool Foam::patchFaceOrientation::updateFace
(
    const polyMesh& mesh,
    const indirectPrimitivePatch& patch,
    const label facei,
    const label edgeI,
    const patchFaceOrientation& edgeInfo,
    const scalar tol,
    TrackingData& td
)
{
    if (valid(td))
    {
        return false;
    }

    // Transfer flip to face
    const face& f = patch.localFaces()[facei];
    const edge& e = patch.edges()[edgeI];


    //Pout<< "Updating face:" << facei << nl
    //    << "    verts:" << f << nl
    //    << " with edge:" << edgeI << nl
    //    << "    start:" << patch.localPoints()[e[0]] << nl
    //    << "      end:" << patch.localPoints()[e[1]] << endl;


    // Create (flipped/unflipped) version of edgeInfo
    patchFaceOrientation consistentInfo(edgeInfo);

    if (f.edgeDirection(e) > 0)
    {
        consistentInfo.flip();
    }

    operator=(consistentInfo);
    return true;
}


template<class TrackingData>
inline bool Foam::patchFaceOrientation::equal
(
    const patchFaceOrientation& rhs,
    TrackingData& td
) const
{
    return operator==(rhs);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

inline bool Foam::patchFaceOrientation::operator==
(
    const patchFaceOrientation& rhs
) const
{
    return flipStatus_ == rhs.flipStatus_;
}


inline bool Foam::patchFaceOrientation::operator!=
(
    const patchFaceOrientation& rhs
) const
{
    return !(*this == rhs);
}


// ************************************************************************* //
