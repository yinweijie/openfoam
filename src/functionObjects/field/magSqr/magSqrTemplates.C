/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
    Copyright (C) 2016-2019 OpenCFD Ltd.
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

#include "fields/volFields/volFields.H"
#include "fields/surfaceFields/surfaceFields.H"
#include "polySurface/fields/polySurfaceFields.H"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
bool Foam::functionObjects::magSqr::calcMagSqr()
{
    typedef GeometricField<Type, fvPatchField, volMesh> VolFieldType;
    typedef GeometricField<Type, fvsPatchField, surfaceMesh> SurfaceFieldType;
    typedef DimensionedField<Type, polySurfaceGeoMesh> SurfFieldType;

    if (foundObject<VolFieldType>(fieldName_, false))
    {
        return store
        (
            resultName_,
            Foam::magSqr(lookupObject<VolFieldType>(fieldName_))
        );
    }
    else if (foundObject<SurfaceFieldType>(fieldName_, false))
    {
        return store
        (
            resultName_,
            Foam::magSqr(lookupObject<SurfaceFieldType>(fieldName_))
        );
    }
    else if (foundObject<SurfFieldType>(fieldName_, false))
    {
        return store
        (
            resultName_,
            Foam::magSqr(lookupObject<SurfFieldType>(fieldName_))
        );
    }

    return false;
}


// ************************************************************************* //
