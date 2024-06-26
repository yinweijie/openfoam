/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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

#include "global/argList/argList.H"
#include "db/Time/TimeOpenFOAM.H"
#include "fvMesh/fvMesh.H"
#include "algorithms/indexedOctree/indexedOctree.H"
#include "algorithms/indexedOctree/treeDataEdge.H"
#include "db/IOstreams/Fstreams/OFstream.H"
#include "edgeMesh/extendedFeatureEdgeMesh/extendedFeatureEdgeMesh.H"
#include "cfdTools/general/include/fvCFD.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{

    #include "include/setRootCase.H"
    #include "include/createTime.H"
    #include "include/createMesh.H"

    fileName sFeatFileName("unit_cube_rotated.extendedFeatureEdgeMesh");

    extendedFeatureEdgeMesh efem
    (
        IOobject
        (
            sFeatFileName,
            runTime.time().constant(),
            "extendedFeatureEdgeMesh",
            runTime.time(),
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    // Slightly extended bb. Slightly off-centred just so on symmetric
    // geometry there are less face/edge aligned items.
    treeBoundBox bb(efem.points());
    bb.grow(ROOTVSMALL);

    indexedOctree<treeDataEdge> edgeTree
    (
        treeDataEdge(efem.edges(), efem.points()),   // All edges

        bb,     // bb
        8,      // maxLevel
        10,     // leafsize
        3.0     // duplicity
    );

    Info<< "Points: " << efem.points() << nl << endl;
    Info<< "Edges: " << efem.edges() << nl << endl;

    Info<< "Find edge labels within sphere from point (0, 0, 0):" << endl;

    Info<< "    Radius = 0            : "
        << edgeTree.findSphere(point(0, 0, 0), 0) << endl;

    Info<< "    Radius = 1            : "
        << edgeTree.findSphere(point(0, 0, 0), 1) << endl;

    Info<< "    Radius = root(1.5)    : "
        << edgeTree.findSphere(point(0, 0, 0), 1.5) << endl;

    Info<< "    Radius = root(2)      : "
        << edgeTree.findSphere(point(0, 0, 0), 2) << endl;

    Info<< "    Radius = root(0.5)    : "
        << edgeTree.findSphere(point(1, 0, 0), 0.5) << endl;

    Info<< "    Radius = root(0.25)   : "
        << edgeTree.findSphere(point(0, 0, 0.5), 0.25) << endl;

    treeBoundBox tbb(point(0,0,0), point(0.1,0.1,0.1));
    Info<< "    Box = " << tbb << "    : "
        << edgeTree.findBox(tbb) << endl;

    treeBoundBox tbb1(point(0,0,0), point(1,1,0.1));
    Info<< "    Box = " << tbb1 << "        : "
        << edgeTree.findBox(tbb1) << endl;

    treeBoundBox tbb2(point(0.3,0,0), point(1,0.3,1));
    Info<< "    Box = " << tbb2 << "      : "
        << edgeTree.findBox(tbb2) << endl;

    treeBoundBox tbb3(point(-0.2,0.5,0), point(0.3,0.9,1));
    Info<< "    Box = " << tbb3 << " : "
        << edgeTree.findBox(tbb3) << endl;

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
