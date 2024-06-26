/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2018-2020 OpenCFD Ltd.
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

#include "sampledSurface/thresholdCellFaces/sampledThresholdCellFaces.H"

#include "surface/thresholdCellFaces/thresholdCellFaces.H"
#include "fields/volFields/volFieldsFwd.H"
#include "fields/GeometricFields/pointFields/pointFields.H"
#include "interpolation/volPointInterpolation/volPointInterpolation.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Type>
Foam::tmp<Foam::Field<Type>>
Foam::sampledThresholdCellFaces::sampleOnFaces
(
    const interpolation<Type>& sampler
) const
{
    updateGeometry();  // Recreate geometry if time has changed

    return sampledSurface::sampleOnFaces
    (
        sampler,
        meshCells_,
        faces(),
        points()
    );
}


template<class Type>
Foam::tmp<Foam::Field<Type>>
Foam::sampledThresholdCellFaces::sampleOnPoints
(
    const interpolation<Type>& interpolator
) const
{
    updateGeometry();  // Recreate geometry if time has changed

    return sampledSurface::sampleOnPoints
    (
        interpolator,
        meshCells_,
        faces(),
        points()
    );
}


// ************************************************************************* //
