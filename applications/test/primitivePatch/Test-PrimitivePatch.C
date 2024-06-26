/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2020-2021 OpenCFD Ltd.
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
    Test new primitive patches.

\*---------------------------------------------------------------------------*/

#include "global/argList/argList.H"
#include "db/Time/TimeOpenFOAM.H"
#include "meshes/polyMesh/polyMesh.H"
#include "meshes/primitiveMesh/primitivePatch/primitivePatch.H"
#include "containers/IndirectLists/IndirectList/IndirectList.H"
#include "db/IOstreams/Fstreams/Fstream.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

void writeObj(Ostream& os, const pointField& points)
{
    for (const point& pt : points)
    {
        os  << "v " << pt.x() << ' ' << pt.y() << ' ' << pt.z() << endl;
    }
}


void checkFaceEdges
(
    const faceList& localFaces,
    const edgeList& edges,
    const labelListList& faceEdges
)
{
    forAll(faceEdges, facei)
    {
        const face& f = localFaces[facei];

        const labelList& myEdges = faceEdges[facei];

        forAll(f, fp)
        {
            label fp1 = f.fcIndex(fp);

            if (edges[myEdges[fp]] != edge(f[fp], f[fp1]))
            {
                FatalErrorInFunction
                    << "Edges of face not in face point order:"
                    << "face:" << facei << " localF:" << f
                    << " faceEdges:" << myEdges
                    << abort(FatalError);
            }
        }
    }
}


void writeEdges
(
    const pointField& localPoints,
    const edgeList& edges,
    const label nInternalEdges
)
{
    Info<< "Writing internal edges to internalEdges.obj" << nl << endl;

    OFstream intStream("internalEdges.obj");

    writeObj(intStream, localPoints);

    for (label edgeI = 0; edgeI < nInternalEdges; edgeI++)
    {
        const edge& e = edges[edgeI];

        intStream << "l " << e.start()+1 << ' ' << e.end()+1 << endl;
    }

    Info<< "Writing boundary edges to boundaryEdges.obj" << nl << endl;

    OFstream bndStream("boundaryEdges.obj");

    writeObj(bndStream, localPoints);

    for (label edgeI = nInternalEdges; edgeI < edges.size(); edgeI++)
    {
        const edge& e = edges[edgeI];

        bndStream << "l " << e.start()+1 << ' ' << e.end()+1 << endl;
    }
}


void writeFaceEdges
(
    const pointField& localPoints,
    const edgeList& edges,
    const labelListList& faceEdges
)
{
    Info<< "Writing faceEdges per face to faceEdges.obj" << nl << endl;

    OFstream feStream("faceEdges.obj");

    writeObj(feStream, localPoints);

    forAll(faceEdges, facei)
    {
        const labelList& myEdges = faceEdges[facei];

        forAll(myEdges, i)
        {
            const edge& e = edges[myEdges[i]];

            feStream<< "l " << e.start()+1 << ' ' << e.end()+1 << endl;
        }
    }
}


void writeEdgeFaces
(
    const pointField& localPoints,
    const faceList& localFaces,
    const edgeList& edges,
    const labelListList& edgeFaces
)
{
    Info<< "Writing edgeFaces per face to edgeFaces.obj" << nl << endl;

    OFstream efStream("edgeFaces.obj");

    pointField ctrs(localFaces.size(), Zero);

    forAll(localFaces, facei)
    {
        ctrs[facei] = localFaces[facei].centre(localPoints);
    }
    writeObj(efStream, ctrs);


    forAll(edgeFaces, edgeI)
    {
        const labelList& myFaces = edgeFaces[edgeI];

        forAll(myFaces, i)
        {
            efStream<< "l " << myFaces[0]+1 << ' ' << myFaces[i]+1 << endl;
        }
    }
}


void writeFaceFaces
(
    const pointField& localPoints,
    const faceList& localFaces,
    const labelListList& faceFaces
)
{
    Info<< "Writing faceFaces per face to faceFaces.obj" << nl << endl;

    OFstream ffStream("faceFaces.obj");

    pointField ctrs(localFaces.size(), Zero);

    forAll(localFaces, facei)
    {
        ctrs[facei] = localFaces[facei].centre(localPoints);
    }
    writeObj(ffStream, ctrs);

    forAll(faceFaces, facei)
    {
        const labelList& nbrs = faceFaces[facei];

        forAll(nbrs, nbI)
        {
            ffStream << "l " << facei+1 << ' ' << nbrs[nbI]+1 << endl;
        }
    }
}


// Main program:

int main(int argc, char *argv[])
{
    argList::noParallel();
    argList::addArgument("patch");

    #include "include/setRootCase.H"
    #include "include/createTime.H"
    #include "include/createPolyMesh.H"

    const word patchName = args[1];
    const polyPatch& patch = mesh.boundaryMesh()[patchName];

    Info<< "Patch:" << patch.name() << endl;

    // Test addressing
    {
        primitiveFacePatch pp(patch, patch.points());

        const pointField& localPoints = pp.localPoints();
        const faceList& localFaces = pp.localFaces();
        const labelListList& faceFaces = pp.faceFaces();
        const edgeList& edges = pp.edges();
        const labelListList& edgeFaces = pp.edgeFaces();
        const labelListList& faceEdges = pp.faceEdges();

        Pout<< "box: " << pp.box() << endl;

        checkFaceEdges(localFaces, edges, faceEdges);

        writeEdges(localPoints, edges, pp.nInternalEdges());

        writeFaceEdges(localPoints, edges, faceEdges);

        writeEdgeFaces(localPoints, localFaces, edges, edgeFaces);

        writeFaceFaces(localPoints, localFaces, faceFaces);

        const labelList bndFaceIds(pp.uniqBoundaryFaces());

        Info<< "Have: " << bndFaceIds.size()
            << " unique boundary faces" << nl;

        // Can calculate by hand
        if (!pp.hasFaceCentres())
        {
            pointField boundaryCentres(bndFaceIds.size());

            forAll(bndFaceIds, facei)
            {
                boundaryCentres[facei] =
                    pp[bndFaceIds[facei]].centre(pp.points());
            }
            Info << "calc faceCentres:"
                << boundaryCentres << nl;
        }

        // But will often use the cached information
        Info<< "faceCentres:"
            << UIndirectList<point>(pp.faceCentres(), bndFaceIds) << nl;
    }

    // Move construct
    {
        faceList patchFaces(patch);
        pointField allPoints(patch.points());

        PrimitivePatch<List<face>, pointField> storedPatch
        (
            patchFaces,
            allPoints,
            true
        );
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
