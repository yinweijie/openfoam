/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 Wikki Ltd
    Copyright (C) 2023 OpenCFD Ltd.
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

#include "finiteArea/gradSchemes/leastSquaresFaGrad/leastSquaresFaGrad.H"
#include "finiteArea/gradSchemes/leastSquaresFaGrad/leastSquaresFaVectors.H"
#include "finiteArea/gradSchemes/gaussFaGrad/gaussFaGrad.H"
#include "faMesh/faMesh.H"
#include "areaMesh/areaFaMesh.H"
#include "edgeMesh/edgeFaMesh.H"
#include "fields/GeometricFields/GeometricField/GeometricField.H"
#include "fields/faPatchFields/basic/zeroGradient/zeroGradientFaPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace fa
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
template<class Type>
tmp
<
    GeometricField
    <
        typename outerProduct<vector, Type>::type,
        faPatchField,
        areaMesh
    >
>
leastSquaresFaGrad<Type>::calcGrad
(
    const GeometricField<Type, faPatchField, areaMesh>& vsf,
    const word& name
) const
{
    typedef typename outerProduct<vector, Type>::type GradType;

    const faMesh& mesh = vsf.mesh();

    tmp<GeometricField<GradType, faPatchField, areaMesh>> tlsGrad
    (
        new GeometricField<GradType, faPatchField, areaMesh>
        (
            IOobject
            (
                name,
                vsf.instance(),
                vsf.db(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh,
            dimensioned<GradType>(vsf.dimensions()/dimLength, Zero),
            faPatchFieldBase::zeroGradientType()
        )
    );
    GeometricField<GradType, faPatchField, areaMesh>& lsGrad = tlsGrad.ref();

    // Get reference to least square vectors
    const leastSquaresFaVectors& lsv = leastSquaresFaVectors::New(mesh);

    const edgeVectorField& ownLs = lsv.pVectors();
    const edgeVectorField& neiLs = lsv.nVectors();

    const labelUList& own = mesh.owner();
    const labelUList& nei = mesh.neighbour();

    forAll(own, edgei)
    {
        label ownEdgeI = own[edgei];
        label neiEdgeI = nei[edgei];

        Type deltaVsf = vsf[neiEdgeI] - vsf[ownEdgeI];

        lsGrad[ownEdgeI] += ownLs[edgei]*deltaVsf;
        lsGrad[neiEdgeI] -= neiLs[edgei]*deltaVsf;
    }

    // Boundary edges
    forAll(vsf.boundaryField(), patchi)
    {
        const faePatchVectorField& patchOwnLs = ownLs.boundaryField()[patchi];

        const labelUList& edgeFaces =
            lsGrad.boundaryField()[patchi].patch().edgeFaces();

        if (vsf.boundaryField()[patchi].coupled())
        {
            Field<Type> neiVsf
            (
                vsf.boundaryField()[patchi].patchNeighbourField()
            );

            forAll(neiVsf, patchEdgeI)
            {
                lsGrad[edgeFaces[patchEdgeI]] +=
                    patchOwnLs[patchEdgeI]
                   *(neiVsf[patchEdgeI] - vsf[edgeFaces[patchEdgeI]]);
            }
        }
        else
        {
            const faPatchField<Type>& patchVsf = vsf.boundaryField()[patchi];

            forAll(patchVsf, patchEdgeI)
            {
                lsGrad[edgeFaces[patchEdgeI]] +=
                     patchOwnLs[patchEdgeI]
                    *(patchVsf[patchEdgeI] - vsf[edgeFaces[patchEdgeI]]);
            }
        }
    }

    // Remove component of gradient normal to surface (area)
    lsGrad.correctBoundaryConditions();

    gaussGrad<Type>::correctBoundaryConditions(vsf, lsGrad);

    return tlsGrad;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fa

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
