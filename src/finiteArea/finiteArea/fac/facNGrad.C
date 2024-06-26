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

#include "finiteArea/fac/facNGrad.H"
#include "fields/areaFields/areaFields.H"
#include "fields/edgeFields/edgeFields.H"
#include "finiteArea/fac/facEdgeIntegrate.H"
#include "faMesh/faMesh.H"
#include "finiteArea/gradSchemes/faGradScheme/faGradScheme.H"
#include "fields/Fields/transformField/transformField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace fac
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
tmp
<
    GeometricField
    <
        typename outerProduct<vector, Type>::type, faPatchField, areaMesh
    >
>
ngrad
(
    const GeometricField<Type, faePatchField, edgeMesh>& ssf
)
{
    const areaVectorField& n = ssf.mesh().faceAreaNormals();
    typedef typename outerProduct<vector,Type>::type GradType;

    tmp<GeometricField<GradType, faPatchField, areaMesh>> tgGrad =
        fac::edgeIntegrate(ssf.mesh().Sf()*ssf);

    GeometricField<GradType, faPatchField, areaMesh>& gGrad = tgGrad.ref();

    gGrad = n*(n & gGrad);
    gGrad.correctBoundaryConditions();

    return tgGrad;
}


template<class Type>
tmp
<
    GeometricField
    <
        typename outerProduct<vector,Type>::type, faPatchField, areaMesh
    >
>
ngrad
(
    const tmp<GeometricField<Type, faePatchField, edgeMesh>>& tssf
)
{
    typedef typename outerProduct<vector, Type>::type GradType;
    tmp<GeometricField<GradType, faPatchField, areaMesh>> Grad
    (
        fac::ngrad(tssf())
    );
    tssf.clear();

    return Grad;
}


template<class Type>
tmp
<
    GeometricField
    <
        typename outerProduct<vector,Type>::type, faPatchField, areaMesh
    >
>
ngrad
(
    const GeometricField<Type, faPatchField, areaMesh>& vf,
    const word& name
)
{
    const areaVectorField& n = vf.mesh().faceAreaNormals();
    typedef typename outerProduct<vector,Type>::type GradType;

    tmp<GeometricField<GradType, faPatchField, areaMesh>> tgGrad =
        fa::gradScheme<Type>::New
        (
            vf.mesh(),
            vf.mesh().gradScheme(name)
        ).ref().grad(vf);

    GeometricField<GradType, faPatchField, areaMesh>& gGrad = tgGrad.ref();

    gGrad = n*(n & gGrad);
    gGrad.correctBoundaryConditions();

    return tgGrad;
}


template<class Type>
tmp
<
    GeometricField
    <
        typename outerProduct<vector,Type>::type, faPatchField, areaMesh
    >
>
ngrad
(
    const tmp<GeometricField<Type, faPatchField, areaMesh>>& tvf,
    const word& name
)
{
    tmp
    <
        GeometricField
        <
            typename outerProduct<vector, Type>::type, faPatchField, areaMesh
        >
    > tGrad
    (
        fac::ngrad(tvf(), name)
    );
    tvf.clear();

    return tGrad;
}


template<class Type>
tmp
<
    GeometricField
    <
        typename outerProduct<vector,Type>::type, faPatchField, areaMesh
    >
>
ngrad
(
    const GeometricField<Type, faPatchField, areaMesh>& vf
)
{
    return fac::ngrad(vf, "grad(" + vf.name() + ')');
}


template<class Type>
tmp
<
    GeometricField
    <
        typename outerProduct<vector,Type>::type, faPatchField, areaMesh
    >
>
ngrad
(
    const tmp<GeometricField<Type, faPatchField, areaMesh>>& tvf
)
{
    typedef typename outerProduct<vector, Type>::type GradType;
    tmp<GeometricField<GradType, faPatchField, areaMesh>> Grad
    (
        fac::ngrad(tvf())
    );
    tvf.clear();

    return Grad;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fac

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
