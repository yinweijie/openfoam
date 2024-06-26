/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 OpenFOAM Foundation
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
    pendulumAndSpring

Description
    Simple 2-DoF pendulum and spring simulation.

\*---------------------------------------------------------------------------*/

#include "rigidBodyMotion/rigidBodyMotion.H"
#include "bodies/masslessBody/masslessBody.H"
#include "bodies/sphere/sphere.H"
#include "joints/joints.H"
#include "restraints/restraint/rigidBodyRestraint.H"
#include "rigidBodyModelState/rigidBodyModelState.H"
#include "db/IOstreams/Fstreams/Fstream.H"
#include "global/constants/constants.H"
#include "global/argList/argList.H"
#include "db/Time/TimeOpenFOAM.H"

using namespace Foam;
using namespace RBD;
using namespace Foam::constant::mathematical;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "include/setRootCase.H"
    #include "include/createTime.H"
    dictionary pendulumAndSpringDict(IFstream("pendulumAndSpring")());

    // Create the pendulumAndSpring model from dictionary
    rigidBodyMotion pendulumAndSpring(runTime, pendulumAndSpringDict);

    label nIter(pendulumAndSpringDict.get<label>("nIter"));

    Info<< pendulumAndSpring << endl;
    Info<< "// Joint state " << endl;
    pendulumAndSpring.state().write(Info);

    // Create the joint-space force field
    scalarField tau(pendulumAndSpring.nDoF(), Zero);

    // Create the external body force field
    Field<spatialVector> fx(pendulumAndSpring.nBodies(), Zero);

    OFstream xFile("xVsTime");
    OFstream omegaFile("omegaVsTime");

    // Integrate the motion of the pendulumAndSpring for 100s
    scalar deltaT = 0.01;
    for (scalar t=0; t<20; t+=deltaT)
    {
        pendulumAndSpring.newTime();

        for (label i=0; i<nIter; i++)
        {
            pendulumAndSpring.solve(t + deltaT, deltaT, tau, fx);
        }

        // Write the results for graph generation
        xFile << t << " " << pendulumAndSpring.state().q()[0] << endl;
        omegaFile << t << " " << pendulumAndSpring.state().q()[1] << endl;
    }

    Info<< "\nEnd\n" << endl;

    return 0;
}


// ************************************************************************* //
