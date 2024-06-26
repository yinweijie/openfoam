/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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
    deformedGeom

Group
    grpMeshManipulationUtilities

Description
    Deforms a polyMesh using a displacement field U and a scaling factor
    supplied as an argument.

\*---------------------------------------------------------------------------*/

#include "global/argList/argList.H"
#include "fvMesh/fvMesh.H"
#include "fields/GeometricFields/pointFields/pointFields.H"
#include "interpolation/volPointInterpolation/volPointInterpolation.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Deforms a polyMesh using a displacement field U and a scaling factor"
        " supplied as an argument"
    );

    argList::addArgument("factor", "The deformation scaling factor");

    #include "include/setRootCase.H"

    const scalar scaleFactor = args.get<scalar>(1);

    #include "include/createTime.H"
    #include "include/createNamedMesh.H"

    volPointInterpolation pInterp(mesh);

    // Get times list
    instantList Times = runTime.times();

    pointField zeroPoints(mesh.points());

    // skip "constant" time
    for (label timeI = 1; timeI < Times.size(); ++timeI)
    {
        runTime.setTime(Times[timeI], timeI);

        Info<< "Time = " << runTime.timeName() << endl;

        IOobject Uheader
        (
            "U",
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ
        );

        // Check U exists
        if (Uheader.typeHeaderOk<volVectorField>(true))
        {
            Info<< "    Reading U" << endl;
            volVectorField U(Uheader, mesh);

            pointField newPoints
            (
                zeroPoints
              + scaleFactor*pInterp.interpolate(U)().primitiveField()
            );

            mesh.polyMesh::movePoints(newPoints);
            mesh.write();
        }
        else
        {
            Info<< "    No U" << endl;
        }

        Info<< endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
