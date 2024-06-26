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

InClass
    Foam::slicedSurfaceFields

\*---------------------------------------------------------------------------*/

#ifndef Foam_slicedSurfaceFields_H
#define Foam_slicedSurfaceFields_H

#include "fields/GeometricFields/SlicedGeometricField/SlicedGeometricField.H"
#include "fields/fvsPatchFields/basic/sliced/slicedFvsPatchField.H"
#include "surfaceMesh/surfaceMesh.H"
#include "fields/surfaceFields/slicedSurfaceFieldsFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<>
inline void
SlicedGeometricField<scalar, fvsPatchField, slicedFvsPatchField, surfaceMesh>::
correctBoundaryConditions()
{}

template<>
inline void
SlicedGeometricField<vector, fvsPatchField, slicedFvsPatchField, surfaceMesh>::
correctBoundaryConditions()
{}

template<>
inline void
SlicedGeometricField
<
    sphericalTensor,
    fvsPatchField,
    slicedFvsPatchField,
    surfaceMesh
>::
correctBoundaryConditions()
{}

template<>
inline void
SlicedGeometricField
<
    symmTensor,
    fvsPatchField,
    slicedFvsPatchField,
    surfaceMesh
>::
correctBoundaryConditions()
{}

template<>
inline void
SlicedGeometricField<tensor, fvsPatchField, slicedFvsPatchField, surfaceMesh>::
correctBoundaryConditions()
{}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
