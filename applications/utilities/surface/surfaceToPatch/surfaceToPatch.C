/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2020-2022 OpenCFD Ltd.
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

Application
    surfaceToPatch

Group
    grpSurfaceUtilities

Description
    Reads surface and applies surface regioning to a mesh. Uses boundaryMesh
    to do the hard work.

\*---------------------------------------------------------------------------*/

#include "global/argList/argList.H"
#include "db/Time/TimeOpenFOAM.H"
#include "boundaryMesh/boundaryMesh.H"
#include "meshes/polyMesh/polyMesh.H"
#include "topoSet/topoSets/faceSet.H"
#include "polyTopoChange/polyTopoChange.H"
#include "polyTopoChange/modifyObject/polyModifyFace.H"
#include "meshes/polyMesh/globalMeshData/globalMeshData.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Adds empty patch if not yet there. Returns patchID.
label addPatch(polyMesh& mesh, const word& patchName)
{
    label patchi = mesh.boundaryMesh().findPatchID(patchName);

    if (patchi == -1)
    {
        const polyBoundaryMesh& patches = mesh.boundaryMesh();

        polyPatchList newPatches(patches.size() + 1);

        patchi = 0;

        // Copy all old patches
        forAll(patches, i)
        {
            const polyPatch& pp = patches[i];

            newPatches.set
            (
                patchi,
                pp.clone
                (
                    patches,
                    patchi,
                    pp.size(),
                    pp.start()
                )
            );

            patchi++;
        }

        // Add zero-sized patch
        newPatches.set
        (
            patchi,
            new polyPatch
            (
                patchName,
                0,
                mesh.nFaces(),
                patchi,
                patches,
                polyPatch::typeName
            )
        );

        mesh.removeBoundary();
        mesh.addPatches(newPatches);

        Pout<< "Created patch " << patchName << " at " << patchi << endl;
    }
    else
    {
        Pout<< "Reusing patch " << patchName << " at " << patchi << endl;
    }

    return patchi;
}


// Repatch single face. Return true if patch changed.
bool repatchFace
(
    const polyMesh& mesh,
    const boundaryMesh& bMesh,
    const labelList& nearest,
    const labelList& surfToMeshPatch,
    const label facei,
    polyTopoChange& meshMod
)
{
    bool changed = false;

    label bFacei = facei - mesh.nInternalFaces();

    if (nearest[bFacei] != -1)
    {
        // Use boundary mesh one.
        label bMeshPatchID = bMesh.whichPatch(nearest[bFacei]);

        label patchID = surfToMeshPatch[bMeshPatchID];

        if (patchID != mesh.boundaryMesh().whichPatch(facei))
        {
            label own = mesh.faceOwner()[facei];

            label zoneID = mesh.faceZones().whichZone(facei);

            bool zoneFlip = false;

            if (zoneID >= 0)
            {
                const faceZone& fZone = mesh.faceZones()[zoneID];

                zoneFlip = fZone.flipMap()[fZone.whichFace(facei)];
            }

            meshMod.setAction
            (
                polyModifyFace
                (
                    mesh.faces()[facei],// modified face
                    facei,              // label of face being modified
                    own,                // owner
                    -1,                 // neighbour
                    false,              // face flip
                    patchID,            // patch for face
                    false,              // remove from zone
                    zoneID,             // zone for face
                    zoneFlip            // face flip in zone
                )
            );

            changed = true;
        }
    }
    else
    {
        changed = false;
    }
    return changed;
}



int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Reads surface and applies surface regioning to a mesh"
    );

    argList::noParallel();
    argList::addArgument("surfaceFile");
    argList::addOption
    (
        "faceSet",
        "name",
        "Only repatch the faces in specified faceSet"
    );
    argList::addOption
    (
        "tol",
        "scalar",
        "Search tolerance as fraction of mesh size (default 1e-3)"
    );

    #include "include/setRootCase.H"
    #include "include/createTime.H"
    #include "include/createPolyMesh.H"

    const auto surfName = args.get<fileName>(1);

    Info<< "Reading surface from " << surfName << " ..." << endl;

    word setName;
    const bool readSet = args.readIfPresent("faceSet", setName);

    if (readSet)
    {
        Info<< "Repatching only the faces in faceSet " << setName
            << " according to nearest surface triangle ..." << endl;
    }
    else
    {
        Info<< "Patching all boundary faces according to nearest surface"
            << " triangle ..." << endl;
    }

    const scalar searchTol = args.getOrDefault<scalar>("tol", 1e-3);

    // Get search box. Anything not within this box will not be considered.
    const boundBox& meshBb = mesh.bounds();
    const vector searchSpan = searchTol * meshBb.span();

    Info<< "All boundary faces further away than " << searchTol
        << " of mesh bounding box " << meshBb
        << " will keep their patch label ..." << endl;


    Info<< "Before patching:" << nl
        << "    patch\tsize" << endl;

    forAll(mesh.boundaryMesh(), patchi)
    {
        Info<< "    " << mesh.boundaryMesh()[patchi].name() << '\t'
            << mesh.boundaryMesh()[patchi].size() << nl;
    }
    Info<< endl;


    boundaryMesh bMesh;

    // Load in the surface.
    bMesh.readTriSurface(surfName);

    // Add all the boundaryMesh patches to the mesh.
    const PtrList<boundaryPatch>& bPatches = bMesh.patches();

    // Map from surface patch ( = boundaryMesh patch) to polyMesh patch
    labelList patchMap(bPatches.size());

    forAll(bPatches, i)
    {
        patchMap[i] = addPatch(mesh, bPatches[i].name());
    }

    // Obtain nearest face in bMesh for each boundary face in mesh that
    // is within search span.
    // Note: should only determine for faceSet if working with that.
    labelList nearest(bMesh.getNearest(mesh, searchSpan));

    {
        // Dump unmatched faces to faceSet for debugging.
        faceSet unmatchedFaces(mesh, "unmatchedFaces", nearest.size()/100);

        forAll(nearest, bFacei)
        {
            if (nearest[bFacei] == -1)
            {
                unmatchedFaces.insert(mesh.nInternalFaces() + bFacei);
            }
        }

        Pout<< "Writing all " << unmatchedFaces.size()
            << " unmatched faces to faceSet "
            << unmatchedFaces.name()
            << endl;

        unmatchedFaces.write();
    }


    polyTopoChange meshMod(mesh);

    label nChanged = 0;

    if (readSet)
    {
        faceSet faceLabels(mesh, setName);
        Info<< "Read " << faceLabels.size() << " faces to repatch ..." << endl;

        for (const label facei : faceLabels)
        {
            if (repatchFace(mesh, bMesh, nearest, patchMap, facei, meshMod))
            {
                nChanged++;
            }
        }
    }
    else
    {
        forAll(nearest, bFacei)
        {
            const label facei = mesh.nInternalFaces() + bFacei;

            if (repatchFace(mesh, bMesh, nearest, patchMap, facei, meshMod))
            {
                ++nChanged;
            }
        }
    }

    Pout<< "Changed " << nChanged << " boundary faces." << nl << endl;

    if (nChanged > 0)
    {
        meshMod.changeMesh(mesh, false);

        Info<< "After patching:" << nl
            << "    patch\tsize" << endl;

        forAll(mesh.boundaryMesh(), patchi)
        {
            Info<< "    " << mesh.boundaryMesh()[patchi].name() << '\t'
                << mesh.boundaryMesh()[patchi].size() << endl;
        }
        Info<< endl;


        ++runTime;

        // Write resulting mesh
        Info<< "Writing modified mesh to time " << runTime.value() << endl;
        mesh.write();
    }


    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
