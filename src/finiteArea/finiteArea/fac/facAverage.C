/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 Wikki Ltd
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

#include "finiteArea/fac/facAverage.H"
#include "finiteArea/fac/facEdgeIntegrate.H"
#include "faMesh/faMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace fac
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
tmp<GeometricField<Type, faPatchField, areaMesh>>
average
(
    const GeometricField<Type,  faePatchField, edgeMesh>& ssf
)
{
    const faMesh& mesh = ssf.mesh();

    tmp<GeometricField<Type, faPatchField, areaMesh>> taverage
    (
        new GeometricField<Type, faPatchField, areaMesh>
        (
            IOobject
            (
                "average("+ssf.name()+')',
                ssf.instance(),
                mesh.thisDb(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh,
            ssf.dimensions()
        )
    );

    GeometricField<Type, faPatchField, areaMesh>& av = taverage.ref();

    av.ref() =
    (
        edgeSum(mesh.magLe()*ssf)/edgeSum(mesh.magLe())
    )().internalField();

    forAll(av.boundaryField(), patchi)
    {
        av.boundaryFieldRef()[patchi] = ssf.boundaryField()[patchi];
    }

    av.correctBoundaryConditions();

    return taverage;
}


template<class Type>
tmp<GeometricField<Type, faPatchField, areaMesh>>
average
(
    const tmp<GeometricField<Type,  faePatchField, edgeMesh>>& tssf
)
{
    tmp<GeometricField<Type, faPatchField, areaMesh>> taverage
    (
        fac::average(tssf())
    );
    tssf.clear();
    return taverage;
}


template<class Type>
tmp<GeometricField<Type, faPatchField, areaMesh>>
average
(
    const GeometricField<Type, faPatchField, areaMesh>& vtf
)
{
    return fac::average(linearEdgeInterpolate(vtf));
}


template<class Type>
tmp<GeometricField<Type, faPatchField, areaMesh>>
average
(
    const tmp<GeometricField<Type, faPatchField, areaMesh>>& tvtf
)
{
    tmp<GeometricField<Type, faPatchField, areaMesh>> taverage
    (
        fac::average(tvtf())
    );
    tvtf.clear();
    return taverage;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fac

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
