/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2017 OpenFOAM Foundation
    Copyright (C) 2017 OpenCFD Ltd.
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
    Calculate and write the distance-to-wall field for a moving mesh.

\*---------------------------------------------------------------------------*/

#include "global/argList/argList.H"
#include "db/Time/TimeOpenFOAM.H"
#include "dynamicFvMesh/dynamicFvMesh.H"
#include "fvMesh/wallDist/wallDist/wallDist.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{
    #include "include/setRootCase.H"
    #include "include/createTime.H"
    #include "include/createDynamicFvMesh.H"

    Info<< "Mesh read in = "
        << runTime.cpuTimeIncrement()
        << " s\n" << endl << endl;

    // Calculate initial mesh-to-mesh mapping. Note that this should be
    // done under the hood, e.g. as a MeshObject
    mesh.update();

    Info<< "Time now = " << runTime.timeName() << endl;

    // Wall-reflection vectors
    const volVectorField& n = wallDist::New(mesh).n();
    n.write();

    // Wall distance
    const volScalarField& y = wallDist::New(mesh).y();
    y.write();

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
