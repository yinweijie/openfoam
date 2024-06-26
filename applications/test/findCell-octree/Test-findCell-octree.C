/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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
#include "algorithms/indexedOctree/treeDataCell.H"
#include "db/IOstreams/Fstreams/OFstream.H"
#include "cfdTools/general/include/fvCFD.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{
    argList::addArgument("point (x y z)");

    #include "include/setRootCase.H"
    #include "include/createTime.H"
    #include "include/createMesh.H"

    label nReps = 10000;

    const point sample = args.get<point>(1);

    const polyMesh::cellDecomposition decompMode = polyMesh::CELL_TETS;

    treeBoundBox meshBb(mesh.bounds());
    treeBoundBox shiftedBb(meshBb);

    // Calculate typical cell related size to shift bb by.
    scalar typDim = meshBb.avgDim()/(2.0*Foam::cbrt(scalar(mesh.nCells())));

    shiftedBb.max() += vector::uniform(typDim);

    Info<< "Mesh" << endl;
    Info<< "   bounding box           : " << meshBb << endl;
    Info<< "   bounding box (shifted) : " << shiftedBb << endl;
    Info<< "   typical dimension      : " << shiftedBb.avgDim() << endl;

    Info<< "Initialised mesh in "
        << runTime.cpuTimeIncrement() << " s" << endl;

    {
        indexedOctree<treeDataCell> ioc
        (
            treeDataCell(true, mesh, decompMode), //FACEDIAGTETS),
            shiftedBb,
            10,         // maxLevel
            100,        // leafsize
            10.0        // duplicity
        );

        for (label i = 0; i < nReps - 1 ; i++)
        {
            if ((i % 100) == 0)
            {
                Info<< "indexed octree for " << i << endl;
            }
            ioc.findInside(sample);
        }

        Info<< "Point:" << sample << " is in shape "
            << ioc.findInside(sample)
            << ", where the possible cells were:" << nl
            << ioc.findIndices(sample)
            << endl;

        Info<< "Found in indexedOctree " << nReps << " times in "
            << runTime.cpuTimeIncrement() << " s" << endl;
    }

    {
        for (label i = 0; i < nReps - 1 ; i++)
        {
            if ((i % 100) == 0)
            {
                Info<< "linear search for " << i << endl;
            }
            mesh.findCell(sample, decompMode);
        }

        Info<< "Point:" << sample << " is in cell  "
            << mesh.findCell(sample, decompMode) << endl;

        Info<< "Found in mesh.findCell " << nReps << " times in "
            << runTime.cpuTimeIncrement() << " s" << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
