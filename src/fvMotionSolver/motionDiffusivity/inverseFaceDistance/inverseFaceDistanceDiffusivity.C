/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2018 OpenCFD Ltd.
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

#include "motionDiffusivity/inverseFaceDistance/inverseFaceDistanceDiffusivity.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "containers/HashTables/HashSet/HashSet.H"
#include "cellDist/wallPoint/wallPoint.H"
#include "algorithms/MeshWave/MeshWave.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(inverseFaceDistanceDiffusivity, 0);

    addToRunTimeSelectionTable
    (
        motionDiffusivity,
        inverseFaceDistanceDiffusivity,
        Istream
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::inverseFaceDistanceDiffusivity::inverseFaceDistanceDiffusivity
(
    const fvMesh& mesh,
    Istream& mdData
)
:
    uniformDiffusivity(mesh, mdData),
    patchNames_(mdData)
{
    correct();
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::inverseFaceDistanceDiffusivity::~inverseFaceDistanceDiffusivity()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::inverseFaceDistanceDiffusivity::correct()
{
    const polyBoundaryMesh& bdry = mesh().boundaryMesh();

    labelHashSet patchSet(bdry.size());

    label nPatchFaces = 0;

    for (const word& patchName : patchNames_)
    {
        const label patchi = bdry.findPatchID(patchName);

        if (patchi >= 0)
        {
            patchSet.insert(patchi);
            nPatchFaces += bdry[patchi].size();
        }
    }

    List<wallPoint> faceDist(nPatchFaces);
    labelList changedFaces(nPatchFaces);

    nPatchFaces = 0;

    for (const label patchi : patchSet)
    {
        const polyPatch& patch = bdry[patchi];

        const vectorField::subField fc(patch.faceCentres());

        forAll(fc, patchFacei)
        {
            changedFaces[nPatchFaces] = patch.start() + patchFacei;

            faceDist[nPatchFaces] = wallPoint(fc[patchFacei], 0);

            nPatchFaces++;
        }
    }
    faceDist.setSize(nPatchFaces);
    changedFaces.setSize(nPatchFaces);

    MeshWave<wallPoint> waveInfo
    (
        mesh(),
        changedFaces,
        faceDist,
        mesh().globalData().nTotalCells()+1   // max iterations
    );

    const List<wallPoint>& faceInfo = waveInfo.allFaceInfo();
    const List<wallPoint>& cellInfo = waveInfo.allCellInfo();

    for (label facei=0; facei<mesh().nInternalFaces(); facei++)
    {
        scalar dist = faceInfo[facei].distSqr();

        faceDiffusivity_[facei] = 1.0/sqrt(dist);
    }

    surfaceScalarField::Boundary& faceDiffusivityBf =
        faceDiffusivity_.boundaryFieldRef();

    forAll(faceDiffusivityBf, patchi)
    {
        fvsPatchScalarField& bfld = faceDiffusivityBf[patchi];

        const labelUList& faceCells = bfld.patch().faceCells();

        if (patchSet.found(patchi))
        {
            forAll(bfld, i)
            {
                scalar dist = cellInfo[faceCells[i]].distSqr();
                bfld[i] = 1.0/sqrt(dist);
            }
        }
        else
        {
            const label start = bfld.patch().start();

            forAll(bfld, i)
            {
                scalar dist = faceInfo[start+i].distSqr();
                bfld[i] = 1.0/sqrt(dist);
            }
        }
    }
}


// ************************************************************************* //
