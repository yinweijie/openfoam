/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2019-2022 OpenCFD Ltd.
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

#ifndef Foam_polySurfaceFieldsFwd_H
#define Foam_polySurfaceFieldsFwd_H

#include "fields/Fields/fieldTypes.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

template<class Type, class GeoMesh> class DimensionedField;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Face Fields

class polySurfaceGeoMesh;

//- A field on polySurface faces for a given type
template<class Type>
using PolyFaceField = DimensionedField<Type, polySurfaceGeoMesh>;


// Typedefs
typedef DimensionedField<label, polySurfaceGeoMesh>
    polySurfaceLabelField;

typedef Foam::DimensionedField<scalar, polySurfaceGeoMesh>
    polySurfaceScalarField;

typedef Foam::DimensionedField<vector, polySurfaceGeoMesh>
    polySurfaceVectorField;

typedef Foam::DimensionedField<sphericalTensor, polySurfaceGeoMesh>
    polySurfaceSphericalTensorField;

typedef Foam::DimensionedField<symmTensor, polySurfaceGeoMesh>
    polySurfaceSymmTensorField;

typedef Foam::DimensionedField<tensor, polySurfaceGeoMesh>
    polySurfaceTensorField;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Point Fields

class polySurfacePointGeoMesh;

//- A field on polySurface face points for a given type
template<class Type>
using PolyPointField = DimensionedField<Type, polySurfacePointGeoMesh>;


// Typedefs
typedef Foam::DimensionedField<label, polySurfacePointGeoMesh>
    polySurfacePointLabelField;

typedef Foam::DimensionedField<scalar, polySurfacePointGeoMesh>
    polySurfacePointScalarField;

typedef Foam::DimensionedField<vector, polySurfacePointGeoMesh>
    polySurfacePointVectorField;

typedef Foam::DimensionedField<sphericalTensor, polySurfacePointGeoMesh>
    polySurfacePointSphericalTensorField;

typedef Foam::DimensionedField<symmTensor, polySurfacePointGeoMesh>
    polySurfacePointSymmTensorField;

typedef Foam::DimensionedField<tensor, polySurfacePointGeoMesh>
    polySurfacePointTensorField;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
