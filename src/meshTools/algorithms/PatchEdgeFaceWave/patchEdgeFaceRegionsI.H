/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2016 OpenFOAM Foundation
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

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

inline Foam::patchEdgeFaceRegions::patchEdgeFaceRegions()
{}


inline Foam::patchEdgeFaceRegions::patchEdgeFaceRegions
(
    const labelList& regions
)
:
    regions_(regions)
{}


inline Foam::patchEdgeFaceRegions::patchEdgeFaceRegions
(
    const labelPair& regions
)
:
    regions_(2)
{
    regions_[0] = regions[0];
    regions_[1] = regions[1];
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class TrackingData>
inline bool Foam::patchEdgeFaceRegions::valid(TrackingData& td) const
{
    return regions_.size() && !regions_.found(labelMax);
}


template<class Patch, class TrackingData>
inline void Foam::patchEdgeFaceRegions::transform
(
    const polyMesh& mesh,
    const Patch& patch,
    const tensor& rotTensor,
    const scalar tol,
    TrackingData& td
)
{}


template<class Patch, class TrackingData>
inline bool Foam::patchEdgeFaceRegions::updateEdge
(
    const polyMesh& mesh,
    const Patch& patch,
    const label edgeI,
    const label facei,
    const patchEdgeFaceRegions& faceInfo,
    const scalar tol,
    TrackingData& td
)
{
    const face& f = patch.localFaces()[facei];
    const edge& e = patch.edges()[edgeI];

    label index = patch.faceEdges()[facei].find(edgeI);
    bool sameOrientation = (f[index] == e.start());

    // Get information in edge-order
    edge orientedInfo
    (
        faceInfo.regions_[index],
        faceInfo.regions_[f.fcIndex(index)]
    );
    if (!sameOrientation)
    {
        orientedInfo.flip();
    }

    if (!faceInfo.valid(td))
    {
        FatalErrorInFunction
            << "problem." << abort(FatalError);
    }

    if (orientedInfo.found(-1) || regions_.found(-1))
    {
        // Blocked edge/face
        return false;
    }


    bool changed = false;

    regions_.setSize(orientedInfo.size(), labelMax);
    forAll(orientedInfo, i)
    {
        if (orientedInfo[i] != -1 && orientedInfo[i] < regions_[i])
        {
            regions_[i] = orientedInfo[i];
            changed = true;
        }
    }
    return changed;
}


template<class Patch, class TrackingData>
inline bool Foam::patchEdgeFaceRegions::updateEdge
(
    const polyMesh& mesh,
    const Patch& patch,
    const patchEdgeFaceRegions& edgeInfo,
    const bool sameOrientation,
    const scalar tol,
    TrackingData& td
)
{
    // Get information in edge-order
    edge orientedInfo(edgeInfo.regions_[0], edgeInfo.regions_[1]);
    if (!sameOrientation)
    {
        orientedInfo.flip();
    }


    if (!edgeInfo.valid(td))
    {
        FatalErrorInFunction
            << "problem." << abort(FatalError);
    }

    if (orientedInfo.found(-1) || regions_.found(-1))
    {
        // Blocked edge/face
        return false;
    }


    bool changed = false;

    regions_.setSize(orientedInfo.size(), labelMax);
    forAll(orientedInfo, i)
    {
        if (orientedInfo[i] != -1 && orientedInfo[i] < regions_[i])
        {
            regions_[i] = orientedInfo[i];
            changed = true;
        }
    }
    return changed;
}


template<class Patch, class TrackingData>
inline bool Foam::patchEdgeFaceRegions::updateFace
(
    const polyMesh& mesh,
    const Patch& patch,
    const label facei,
    const label edgeI,
    const patchEdgeFaceRegions& edgeInfo,
    const scalar tol,
    TrackingData& td
)
{
    const face& f = patch.localFaces()[facei];
    const edge& e = patch.edges()[edgeI];

    // Find starting point of edge on face.
    label index0 = patch.faceEdges()[facei].find(edgeI);
    label index1 = f.fcIndex(index0);
    bool sameOrientation = (f[index0] == e.start());


    // Get information in face-order
    edge orientedInfo
    (
        edgeInfo.regions_[0],
        edgeInfo.regions_[1]
    );
    if (!sameOrientation)
    {
        orientedInfo.flip();
    }

    if (!edgeInfo.valid(td))
    {
        FatalErrorInFunction
            << "problem." << abort(FatalError);
    }

    if (orientedInfo.found(-1) || regions_.found(-1))
    {
        // Blocked edge/face
        return false;
    }


    bool changed = false;

    // Scale if needed
    regions_.setSize(f.size(), labelMax);

    if (orientedInfo[0] < regions_[index0])
    {
        regions_[index0] = orientedInfo[0];
        changed = true;
    }
    if (orientedInfo[1] < regions_[index1])
    {
        regions_[index1] = orientedInfo[1];
        changed = true;
    }

    return changed;
}


template<class TrackingData>
inline bool Foam::patchEdgeFaceRegions::equal
(
    const patchEdgeFaceRegions& rhs,
    TrackingData& td
) const
{
    return operator==(rhs);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

inline bool Foam::patchEdgeFaceRegions::operator==
(
    const patchEdgeFaceRegions& rhs
) const
{
    return regions_ == rhs.regions_;
}


inline bool Foam::patchEdgeFaceRegions::operator!=
(
    const patchEdgeFaceRegions& rhs
) const
{
    return !(*this == rhs);
}


// ************************************************************************* //
