/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2021 OpenCFD Ltd.
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

#include "finiteVolume/gradSchemes/fourthGrad/fourthGrad.H"
#include "finiteVolume/gradSchemes/leastSquaresGrad/leastSquaresGrad.H"
#include "finiteVolume/gradSchemes/gaussGrad/gaussGrad.H"
#include "fvMesh/fvMesh.H"
#include "volMesh/volMesh.H"
#include "surfaceMesh/surfaceMesh.H"
#include "fields/GeometricFields/GeometricField/GeometricField.H"
#include "fields/fvPatchFields/basic/zeroGradient/zeroGradientFvPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
Foam::tmp
<
    Foam::GeometricField
    <
        typename Foam::outerProduct<Foam::vector, Type>::type,
        Foam::fvPatchField,
        Foam::volMesh
    >
>
Foam::fv::fourthGrad<Type>::calcGrad
(
    const GeometricField<Type, fvPatchField, volMesh>& vsf,
    const word& name
) const
{
    // The fourth-order gradient is calculated in two passes.  First,
    // the standard least-square gradient is assembled.  Then, the
    // fourth-order correction is added to the second-order accurate
    // gradient to complete the accuracy.

    typedef typename outerProduct<vector, Type>::type GradType;
    typedef GeometricField<GradType, fvPatchField, volMesh> GradFieldType;

    const fvMesh& mesh = vsf.mesh();

    // Assemble the second-order least-square gradient
    // Calculate the second-order least-square gradient
    tmp<GradFieldType> tsecondfGrad
      = leastSquaresGrad<Type>(mesh).grad
        (
            vsf,
            "leastSquaresGrad(" + vsf.name() + ")"
        );
    const GradFieldType& secondfGrad =
        tsecondfGrad();

    tmp<GradFieldType> tfGrad
    (
        new GradFieldType
        (
            IOobject
            (
                name,
                vsf.instance(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            secondfGrad
        )
    );
    GradFieldType& fGrad = tfGrad.ref();

    const vectorField& C = mesh.C();

    const surfaceScalarField& lambda = mesh.weights();

    // Get reference to least square vectors
    const leastSquaresVectors& lsv = leastSquaresVectors::New(mesh);
    const surfaceVectorField& ownLs = lsv.pVectors();
    const surfaceVectorField& neiLs = lsv.nVectors();

    // owner/neighbour addressing
    const labelUList& own = mesh.owner();
    const labelUList& nei = mesh.neighbour();

    // Assemble the fourth-order gradient

    // Internal faces
    forAll(own, facei)
    {
        Type dDotGradDelta = 0.5*
        (
           (C[nei[facei]] - C[own[facei]])
         & (secondfGrad[nei[facei]] - secondfGrad[own[facei]])
        );

        fGrad[own[facei]] -= lambda[facei]*ownLs[facei]*dDotGradDelta;
        fGrad[nei[facei]] -= (1.0 - lambda[facei])*neiLs[facei]*dDotGradDelta;
    }

    // Boundary faces
    forAll(vsf.boundaryField(), patchi)
    {
        if (secondfGrad.boundaryField()[patchi].coupled())
        {
            const fvsPatchVectorField& patchOwnLs =
                ownLs.boundaryField()[patchi];

            const scalarField& lambdap = lambda.boundaryField()[patchi];

            const fvPatch& p = vsf.boundaryField()[patchi].patch();

            const labelUList& faceCells = p.faceCells();

            // Build the d-vectors
            const vectorField pd(p.delta());

            const Field<GradType> neighbourSecondfGrad
            (
                secondfGrad.boundaryField()[patchi].patchNeighbourField()
            );

            forAll(faceCells, patchFacei)
            {
                fGrad[faceCells[patchFacei]] -=
                    0.5*lambdap[patchFacei]*patchOwnLs[patchFacei]
                   *(
                        pd[patchFacei]
                      & (
                            neighbourSecondfGrad[patchFacei]
                          - secondfGrad[faceCells[patchFacei]]
                        )
                    );
            }
        }
    }

    fGrad.correctBoundaryConditions();
    gaussGrad<Type>::correctBoundaryConditions(vsf, fGrad);

    return tfGrad;
}


// ************************************************************************* //
