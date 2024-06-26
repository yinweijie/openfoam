/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2014-2016 OpenFOAM Foundation
    Copyright (C) 2015-2020 OpenCFD Ltd.
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

inline Foam::minData::minData()
:
    data_(labelMax)
{}


inline Foam::minData::minData(const label data)
:
    data_(data)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class TrackingData>
inline bool Foam::minData::valid(TrackingData& td) const
{
    return data_ != labelMax;
}


template<class TrackingData>
inline bool Foam::minData::sameGeometry
(
    const polyMesh&,
    const minData&,
    const scalar,
    TrackingData&
) const
{
    return true;
}


template<class TrackingData>
inline void Foam::minData::leaveDomain
(
    const polyMesh&,
    const polyPatch& patch,
    const label patchFacei,
    const point& faceCentre,
    TrackingData&
)
{}


template<class TrackingData>
inline void Foam::minData::transform
(
    const polyMesh&,
    const tensor& rotTensor,
    TrackingData&
)
{}


template<class TrackingData>
inline void Foam::minData::enterDomain
(
    const polyMesh&,
    const polyPatch& patch,
    const label patchFacei,
    const point& faceCentre,
    TrackingData&
)
{}


template<class TrackingData>
inline bool Foam::minData::updateCell
(
    const polyMesh&,
    const label thisCelli,
    const label neighbourFacei,
    const minData& neighbourInfo,
    const scalar tol,
    TrackingData&
)
{
    if (neighbourInfo.data_ < data_)
    {
        operator=(neighbourInfo);
        return true;
    }

    return false;
}


template<class TrackingData>
inline bool Foam::minData::updateFace
(
    const polyMesh& mesh,
    const label thisFacei,
    const label neighbourCelli,
    const minData& neighbourInfo,
    const scalar tol,
    TrackingData&
)
{
    // From cell to its faces.
    if (neighbourInfo.data_ < data_)
    {
        operator=(neighbourInfo);
        return true;
    }

    return false;
}


template<class TrackingData>
inline bool Foam::minData::updateFace
(
    const polyMesh&,
    const label thisFacei,
    const minData& neighbourInfo,
    const scalar tol,
    TrackingData&
)
{
    // From face to face (e.g. coupled faces)
    if (neighbourInfo.data_ < data_)
    {
        operator=(neighbourInfo);
        return true;
    }

    return false;
}


template<class TrackingData>
inline bool Foam::minData::equal
(
    const minData& rhs,
    TrackingData& td
) const
{
    return operator==(rhs);
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

inline bool Foam::minData::operator==
(
    const minData& rhs
) const
{
    return data_ == rhs.data_;
}


inline bool Foam::minData::operator!=
(
    const minData& rhs
) const
{
    return !(*this == rhs);
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

inline Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const minData& rhs
)
{
    return os << rhs.data_;
}


inline Foam::Istream& Foam::operator>>
(
    Istream& is,
    minData& rhs
)
{
    return is >> rhs.data_;
}


// ************************************************************************* //
