/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2020 OpenCFD Ltd.
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

#include "interpolation/surfaceInterpolation/schemes/CentredFitScheme/CentredFitData.H"
#include "fields/surfaceFields/surfaceFields.H"
#include "fields/volFields/volFields.H"
#include "matrices/scalarMatrices/SVD/SVD.H"
#include "meshes/polyMesh/syncTools/syncTools.H"
#include "fvMesh/extendedStencil/cellToFace/extendedCentredCellToFaceStencil.H"

// * * * * * * * * * * * * * * * * Constructors * * * * * * * * * * * * * * //

template<class Polynomial>
Foam::CentredFitData<Polynomial>::CentredFitData
(
    const fvMesh& mesh,
    const extendedCentredCellToFaceStencil& stencil,
    const scalar linearLimitFactor,
    const scalar centralWeight
)
:
    FitData
    <
        CentredFitData<Polynomial>,
        extendedCentredCellToFaceStencil,
        Polynomial
    >
    (
        mesh, stencil, true, linearLimitFactor, centralWeight
    ),
    coeffs_(mesh.nFaces())
{
    DebugInFunction << "Constructing CentredFitData<Polynomial>" << nl;

    calcFit();

    DebugInfo << "Finished constructing polynomialFit data" << endl;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Polynomial>
void Foam::CentredFitData<Polynomial>::calcFit()
{
    const fvMesh& mesh = this->mesh();

    // Get the cell/face centres in stencil order.
    // Centred face stencils no good for triangles or tets.
    // Need bigger stencils
    List<List<point>> stencilPoints(mesh.nFaces());
    this->stencil().collectData(mesh.C(), stencilPoints);

    // find the fit coefficients for every face in the mesh

    const surfaceScalarField& w = mesh.surfaceInterpolation::weights();

    for (label facei = 0; facei < mesh.nInternalFaces(); facei++)
    {
        FitData
        <
            CentredFitData<Polynomial>,
            extendedCentredCellToFaceStencil,
            Polynomial
        >::calcFit(coeffs_[facei], stencilPoints[facei], w[facei], facei);
    }

    const surfaceScalarField::Boundary& bw = w.boundaryField();

    forAll(bw, patchi)
    {
        const fvsPatchScalarField& pw = bw[patchi];

        if (pw.coupled())
        {
            label facei = pw.patch().start();

            forAll(pw, i)
            {
                FitData
                <
                    CentredFitData<Polynomial>,
                    extendedCentredCellToFaceStencil,
                    Polynomial
                >::calcFit(coeffs_[facei], stencilPoints[facei], pw[i], facei);
                facei++;
            }
        }
    }
}


// ************************************************************************* //
