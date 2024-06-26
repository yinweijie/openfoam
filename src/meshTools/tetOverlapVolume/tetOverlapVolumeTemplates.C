/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015 OpenFOAM Foundation
    Copyright (C) 2016 OpenCFD Ltd.
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

#include "tetOverlapVolume/tetOverlapVolume.H"
#include "meshes/primitiveMesh/primitiveMesh.H"

// * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * * //

template<class tetPointsOp>
void Foam::tetOverlapVolume::tetTetOverlap
(
    const tetPoints& tetA,
    const tetPoints& tetB,
    tetPointsOp& insideOp
)
{
    static tetPointRef::tetIntersectionList insideTets;
    label nInside = 0;
    static tetPointRef::tetIntersectionList cutInsideTets;
    label nCutInside = 0;

    tetPointRef::storeOp inside(insideTets, nInside);
    tetPointRef::storeOp cutInside(cutInsideTets, nCutInside);
    tetPointRef::dummyOp outside;

    // Precompute the tet face areas and exit early if any face area is
    // too small
    static FixedList<vector, 4> tetAFaceAreas;
    static FixedList<scalar, 4> tetAMag2FaceAreas;
    tetPointRef tetATet = tetA.tet();
    for (label facei = 0; facei < 4; ++facei)
    {
        tetAFaceAreas[facei] = -tetATet.tri(facei).areaNormal();
        tetAMag2FaceAreas[facei] = magSqr(tetAFaceAreas[facei]);
        if (tetAMag2FaceAreas[facei] < ROOTVSMALL)
        {
            return;
        }
    }

    static FixedList<vector, 4> tetBFaceAreas;
    static FixedList<scalar, 4> tetBMag2FaceAreas;
    tetPointRef tetBTet = tetB.tet();
    for (label facei = 0; facei < 4; ++facei)
    {
        tetBFaceAreas[facei] = -tetBTet.tri(facei).areaNormal();
        tetBMag2FaceAreas[facei] = magSqr(tetBFaceAreas[facei]);
        if (tetBMag2FaceAreas[facei] < ROOTVSMALL)
        {
            return;
        }
    }


    // Face 0
    {
        vector n = tetBFaceAreas[0]/Foam::sqrt(tetBMag2FaceAreas[0]);
        plane pl0(tetBTet.tri(0).a(), n, false);

        tetA.tet().sliceWithPlane(pl0, cutInside, outside);
        if (nCutInside == 0)
        {
            return;
        }
    }

    // Face 1
    {
        vector n = tetBFaceAreas[1]/Foam::sqrt(tetBMag2FaceAreas[1]);
        plane pl1(tetBTet.tri(1).a(), n, false);

        nInside = 0;
        for (label i = 0; i < nCutInside; i++)
        {
            const tetPointRef t = cutInsideTets[i].tet();
            t.sliceWithPlane(pl1, inside, outside);
        }
        if (nInside == 0)
        {
            return;
        }
    }

    // Face 2
    {
        vector n = tetBFaceAreas[2]/Foam::sqrt(tetBMag2FaceAreas[2]);
        plane pl2(tetBTet.tri(2).a(), n, false);

        nCutInside = 0;
        for (label i = 0; i < nInside; i++)
        {
            const tetPointRef t = insideTets[i].tet();
            t.sliceWithPlane(pl2, cutInside, outside);
        }
        if (nCutInside == 0)
        {
            return;
        }
    }

    // Face 3
    {
        vector n = tetBFaceAreas[3]/Foam::sqrt(tetBMag2FaceAreas[3]);
        plane pl3(tetBTet.tri(3).a(), n, false);
        for (label i = 0; i < nCutInside; i++)
        {
            const tetPointRef t = cutInsideTets[i].tet();
            t.sliceWithPlane(pl3, insideOp, outside);
        }
    }
}


template<class tetsOp>
void Foam::tetOverlapVolume::cellCellOverlapMinDecomp
(
    const primitiveMesh& meshA,
    const label cellAI,

    const primitiveMesh& meshB,
    const label cellBI,
    const treeBoundBox& cellBbB,
    tetsOp& combineTetsOp
)
{
    const cell& cFacesA = meshA.cells()[cellAI];
    const point& ccA = meshA.cellCentres()[cellAI];

    const cell& cFacesB = meshB.cells()[cellBI];
    const point& ccB = meshB.cellCentres()[cellBI];

    forAll(cFacesA, cFA)
    {
        label faceAI = cFacesA[cFA];

        const face& fA = meshA.faces()[faceAI];
        const treeBoundBox pyrA = pyrBb(meshA.points(), fA, ccA);
        if (!pyrA.overlaps(cellBbB))
        {
            continue;
        }

        bool ownA = (meshA.faceOwner()[faceAI] == cellAI);

        label tetBasePtAI = 0;

        const point& tetBasePtA = meshA.points()[fA[tetBasePtAI]];

        for (label tetPtI = 1; tetPtI < fA.size() - 1; tetPtI++)
        {
            label facePtAI = (tetPtI + tetBasePtAI) % fA.size();
            label otherFacePtAI = fA.fcIndex(facePtAI);

            label pt0I = -1;
            label pt1I = -1;

            if (ownA)
            {
                pt0I = fA[facePtAI];
                pt1I = fA[otherFacePtAI];
            }
            else
            {
                pt0I = fA[otherFacePtAI];
                pt1I = fA[facePtAI];
            }

            const tetPoints tetA
            (
                ccA,
                tetBasePtA,
                meshA.points()[pt0I],
                meshA.points()[pt1I]
            );

            const treeBoundBox tetAbb(tetA.bounds());

            // Loop over tets of cellB
            forAll(cFacesB, cFB)
            {
                label faceBI = cFacesB[cFB];

                const face& fB = meshB.faces()[faceBI];
                const treeBoundBox pyrB = pyrBb(meshB.points(), fB, ccB);
                if (!pyrB.overlaps(pyrA))
                {
                    continue;
                }

                bool ownB = (meshB.faceOwner()[faceBI] == cellBI);

                label tetBasePtBI = 0;

                const point& tetBasePtB = meshB.points()[fB[tetBasePtBI]];

                for (label tetPtI = 1; tetPtI < fB.size() - 1; tetPtI++)
                {
                    label facePtBI = (tetPtI + tetBasePtBI) % fB.size();
                    label otherFacePtBI = fB.fcIndex(facePtBI);

                    label pt0I = -1;
                    label pt1I = -1;

                    if (ownB)
                    {
                        pt0I = fB[facePtBI];
                        pt1I = fB[otherFacePtBI];
                    }
                    else
                    {
                        pt0I = fB[otherFacePtBI];
                        pt1I = fB[facePtBI];
                    }

                    const tetPoints tetB
                    (
                        ccB,
                        tetBasePtB,
                        meshB.points()[pt0I],
                        meshB.points()[pt1I]
                    );

                    const treeBoundBox tetBbb(tetB.bounds());

                    if (!tetBbb.overlaps(tetAbb))
                    {
                        continue;
                    }

                    if (combineTetsOp(tetA, tetB))
                    {
                        return;
                    }
                }
            }
        }
    }
}


// ************************************************************************* //
