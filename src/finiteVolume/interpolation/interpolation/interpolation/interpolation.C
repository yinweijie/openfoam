/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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

#include "interpolation/interpolation/interpolation/interpolation.H"
#include "fields/volFields/volFields.H"
#include "meshes/polyMesh/polyMesh.H"
#include "fields/pointPatchFields/basic/calculated/calculatedPointPatchFields.H"

// * * * * * * * * * * * * * * * * Constructor * * * * * * * * * * * * * * * //

template<class Type>
Foam::interpolation<Type>::interpolation
(
    const GeometricField<Type, fvPatchField, volMesh>& psi
)
:
    psi_(psi),
    pMesh_(psi.mesh()),
    pMeshPoints_(pMesh_.points()),
    pMeshFaces_(pMesh_.faces()),
    pMeshFaceCentres_(pMesh_.faceCentres()),
    pMeshFaceAreas_(pMesh_.faceAreas())
{}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "interpolation/interpolation/interpolation/interpolationNew.C"

// ************************************************************************* //
