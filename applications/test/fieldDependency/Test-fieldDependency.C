/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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
    Test field dependencies.

\*---------------------------------------------------------------------------*/

#include "global/argList/argList.H"
#include "db/Time/TimeOpenFOAM.H"
#include "fields/volFields/volFields.H"
#include "cfdTools/general/include/fvCFD.H"

using namespace Foam;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// Main program:

int main(int argc, char *argv[])
{
    #include "include/setRootCase.H"
    #include "include/createTime.H"
    #include "include/createMesh.H"

    Info<< "Creating field T\n" << endl;
    volScalarField T
    (
        IOobject
        (
            "T",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar(dimless, Zero)
    );

    Info<< "Creating field p\n" << endl;
    volScalarField p
    (
        IOobject
        (
            "p",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar(dimless, Zero)
    );


    Info<< "p.eventNo:" << p.eventNo() << endl;
    Info<< "p.uptodate:" << p.upToDate(T)<< endl;

    // Change T and mark as uptodate.
    Info<< "Changing T" << endl;
    T = 0.0;
    T.setUpToDate();
    Info<< "T.eventNo:" << T.eventNo() << endl;

    // Check p dependency:
    Info<< "p.uptodate:" << p.upToDate(T)<< endl;

    // Change p and mark as uptodate.
    Info<< "Changing p." << endl;
    p.setUpToDate();
    Info<< "p.uptodate:" << p.upToDate(T)<< endl;
    Info<< "p.eventNo:" << p.eventNo() << endl;


    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
