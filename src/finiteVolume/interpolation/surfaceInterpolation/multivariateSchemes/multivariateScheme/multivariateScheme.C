/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
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
#include "interpolation/surfaceInterpolation/limitedSchemes/upwind/upwind.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type, class Scheme>
Foam::multivariateScheme<Type, Scheme>::multivariateScheme
(
    const fvMesh& mesh,
    const typename multivariateSurfaceInterpolationScheme<Type>::
        fieldTable& fields,
    const surfaceScalarField& faceFlux,
    Istream& schemeData
)
:
    multivariateSurfaceInterpolationScheme<Type>
    (
        mesh,
        fields,
        faceFlux,
        schemeData
    ),
    Scheme::LimiterType(schemeData),
    faceFlux_(faceFlux),
    weights_
    (
        IOobject
        (
            "multivariateWeights",
            mesh.time().timeName(),
            mesh
        ),
        mesh,
        dimless
    )
{
    auto iter = this->fields().cbegin();

    surfaceScalarField limiter
    (
        Scheme(mesh, faceFlux_, *this).limiter(*iter())
    );

    for (++iter; iter.good(); ++iter)
    {
        limiter = min
        (
            limiter,
            Scheme(mesh, faceFlux_, *this).limiter(*iter())
        );
    }

    weights_ =
        limiter*mesh.surfaceInterpolation::weights()
      + (scalar(1) - limiter)*upwind<Type>(mesh, faceFlux_).weights();
}


// ************************************************************************* //
