/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

Description
    Test PatchEdgeFaceWave.

\*---------------------------------------------------------------------------*/

#include "global/argList/argList.H"
#include "db/Time/TimeOpenFOAM.H"
#include "fvMesh/fvMesh.H"
#include "algorithms/PatchEdgeFaceWave/PatchEdgeFaceWave.H"
#include "algorithms/PatchEdgeFaceWave/patchEdgeFaceInfo.H"
#include "algorithms/PatchEdgeFaceWave/patchPatchDist.H"
#include "cfdTools/general/include/fvCFD.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addArgument("patch");

    #include "include/setRootCase.H"
    #include "include/createTime.H"
    #include "include/createMesh.H"

    const polyBoundaryMesh& patches = mesh.boundaryMesh();

    // Get name of patch
    const word patchName = args[1];
    const polyPatch& patch = patches[patchName];

    // 1. Walk from a single edge
    {
        // Data on all edges and faces
        List<patchEdgeFaceInfo> allEdgeInfo(patch.nEdges());
        List<patchEdgeFaceInfo> allFaceInfo(patch.size());

        // Initial seed
        DynamicList<label> initialEdges;
        DynamicList<patchEdgeFaceInfo> initialEdgesInfo;


        // Just set an edge on the master
        if (Pstream::master())
        {
            label edgeI = 0;
            Info<< "Starting walk on edge " << edgeI << endl;

            initialEdges.push_back(edgeI);
            const edge& e = patch.edges()[edgeI];
            initialEdgesInfo.emplace_back(e.centre(patch.localPoints()), 0);
        }


        // Walk
        PatchEdgeFaceWave
        <
            primitivePatch,
            patchEdgeFaceInfo
        > calc
        (
            mesh,
            patch,
            initialEdges,
            initialEdgesInfo,
            allEdgeInfo,
            allFaceInfo,
            returnReduce(patch.nEdges(), sumOp<label>())
        );


        // Extract as patchField
        volScalarField vsf
        (
            IOobject
            (
                "patchDist",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),
            mesh,
            dimensionedScalar(dimLength, Zero)
        );
        scalarField pf(vsf.boundaryField()[patch.index()].size());
        forAll(pf, facei)
        {
            pf[facei] = Foam::sqrt(allFaceInfo[facei].distSqr());
        }
        vsf.boundaryFieldRef()[patch.index()] = pf;

        Info<< "Writing patchDist volScalarField to " << runTime.value()
            << endl;

        vsf.write();
    }


    // 2. Use a wrapper to walk from all boundary edges on selected patches
    {
        labelHashSet otherPatchIDs(identity(mesh.boundaryMesh().size()));
        otherPatchIDs.erase(patch.index());

        Info<< "Walking on patch " << patch.index()
            << " from edges shared with patches " << otherPatchIDs
            << endl;

        patchPatchDist pwd(patch, otherPatchIDs);

        // Extract as patchField
        volScalarField vsf
        (
            IOobject
            (
                "otherPatchDist",
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),
            mesh,
            dimensionedScalar(dimLength, Zero)
        );
        vsf.boundaryFieldRef()[patch.index()] = pwd;

        Info<< "Writing otherPatchDist volScalarField to " << runTime.value()
            << endl;

        vsf.write();
    }

    Info<< "\nEnd\n" << endl;
    return 0;
}


// ************************************************************************* //
