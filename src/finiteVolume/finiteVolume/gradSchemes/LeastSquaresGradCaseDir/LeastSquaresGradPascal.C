/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2016 OpenFOAM Foundation
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

#include "finiteVolume/gradSchemes/LeastSquaresGradCaseDir/LeastSquaresGradPascal.H"
#include "finiteVolume/gradSchemes/LeastSquaresGradCaseDir/LeastSquaresVectorsPascal.H"
#include "finiteVolume/gradSchemes/gaussGrad/gaussGrad.H"
#include "fvMesh/fvMesh.H"
#include "volMesh/volMesh.H"
#include "fields/fvPatchFields/basic/extrapolatedCalculated/extrapolatedCalculatedFvPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type, class Stencil>
Foam::tmp
<
    Foam::GeometricField
    <
        typename Foam::outerProduct<Foam::vector, Type>::type,
        Foam::fvPatchField,
    Foam::volMesh
    >
>
Foam::fv::LeastSquaresGrad<Type, Stencil>::calcGrad
(
    const GeometricField<Type, fvPatchField, volMesh>& vtf,
    const word& name
) const
{
    typedef typename outerProduct<vector, Type>::type GradType;
    typedef GeometricField<GradType, fvPatchField, volMesh> GradFieldType;

    const fvMesh& mesh = vtf.mesh();

    // Get reference to least square vectors
    const LeastSquaresVectors<Stencil>& lsv = LeastSquaresVectors<Stencil>::New
    (
        mesh
    );

    tmp<GradFieldType> tlsGrad
    (
        new GradFieldType
        (
            IOobject
            (
                name,
                vtf.instance(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh,
            dimensioned<GradType>(vtf.dimensions()/dimLength, Zero),
            fvPatchFieldBase::extrapolatedCalculatedType()
        )
    );
    GradFieldType& lsGrad = tlsGrad.ref();
    Field<GradType>& lsGradIf = lsGrad;

    const extendedCentredCellToCellStencil& stencil = lsv.stencil();
    const List<List<label>>& stencilAddr = stencil.stencil();
    const List<List<vector>>& lsvs = lsv.vectors();

    // Construct flat version of vtf
    // including all values referred to by the stencil
    List<Type> flatVtf(stencil.map().constructSize(), Zero);

    // Insert internal values
    forAll(vtf, celli)
    {
        flatVtf[celli] = vtf[celli];
    }

    // Insert boundary values
    forAll(vtf.boundaryField(), patchi)
    {
        const fvPatchField<Type>& ptf = vtf.boundaryField()[patchi];

        label nCompact =
            ptf.patch().start()
          - mesh.nInternalFaces()
          + mesh.nCells();

        forAll(ptf, i)
        {
            flatVtf[nCompact++] = ptf[i];
        }
    }

    // Do all swapping to complete flatVtf
    stencil.map().distribute(flatVtf);

    // Accumulate the cell-centred gradient from the
    // weighted least-squares vectors and the flattened field values
    forAll(stencilAddr, celli)
    {
        const labelList& compactCells = stencilAddr[celli];
        const List<vector>& lsvc = lsvs[celli];

        forAll(compactCells, i)
        {
            lsGradIf[celli] += lsvc[i]*flatVtf[compactCells[i]];
        }
    }

    // Correct the boundary conditions
    lsGrad.correctBoundaryConditions();
    gaussGrad<Type>::correctBoundaryConditions(vtf, lsGrad);

    return tlsGrad;
}


// ************************************************************************* //
