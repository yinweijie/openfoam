/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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

Application
    XiEngineFoam

Description
    Solver for internal combustion engines.

    Combusting RANS code using the b-Xi two-equation model.
    Xi may be obtained by either the solution of the Xi transport
    equation or from an algebraic expression.  Both approaches are
    based on Gulder's flame speed correlation which has been shown
    to be appropriate by comparison with the results from the
    spectral model.

    Strain effects are encorporated directly into the Xi equation
    but not in the algebraic approximation.  Further work need to be
    done on this issue, particularly regarding the enhanced removal rate
    caused by flame compression.  Analysis using results of the spectral
    model will be required.

    For cases involving very lean Propane flames or other flames which are
    very strain-sensitive, a transport equation for the laminar flame
    speed is present.  This equation is derived using heuristic arguments
    involving the strain time scale and the strain-rate at extinction.
    the transport velocity is the same as that for the Xi equation.

\*---------------------------------------------------------------------------*/

#include "cfdTools/general/include/fvCFD.H"
#include "engineTime/engineTime/engineTime.H"
#include "engineMesh/engineMesh/engineMesh.H"
#include "psiuReactionThermo/psiuReactionThermo.H"
#include "turbulentFluidThermoModels/turbulentFluidThermoModel.H"
#include "laminarFlameSpeed/laminarFlameSpeed.H"
#include "ignition/ignition.H"
#include "primitives/bools/Switch/Switch.H"
#include "db/IOstreams/Fstreams/OFstream.H"
#include "global/constants/mathematical/mathematicalConstants.H"
#include "cfdTools/general/solutionControl/pimpleControl/pimpleControl.H"
#include "cfdTools/general/fvOptions/fvOptions.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Solver for compressible premixed/partially-premixed combustion with"
        " turbulence modelling in internal combustion engines."
    );

    #define CREATE_TIME createEngineTime.H
    #define CREATE_MESH createEngineMesh.H
    #include "db/functionObjects/functionObjectList/postProcess.H"

    #include "include/setRootCaseLists.H"
    #include "include/createEngineTime.H"
    #include "include/createEngineMesh.H"
    #include "cfdTools/general/solutionControl/createControl.H"
    #include "readCombustionProperties.H"
    #include "createFields.H"
    #include "createFieldRefs.H"
    #include "cfdTools/compressible/createRhoUf.H"
    #include "fluid/initContinuityErrs.H"
    #include "include/readEngineTimeControls.H"
    #include "fluid/compressibleCourantNo.H"
    #include "cfdTools/general/include/setInitialDeltaT.H"
    #include "startSummary.H"

    turbulence->validate();

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;

    while (runTime.run())
    {
        #include "include/readEngineTimeControls.H"
        #include "fluid/compressibleCourantNo.H"
        #include "cfdTools/general/include/setDeltaT.H"

        ++runTime;

        Info<< "Crank angle = " << runTime.theta() << " CA-deg" << endl;

        mesh.move();

        #include "cfdTools/compressible/rhoEqn.H"

        // --- Pressure-velocity PIMPLE corrector loop
        while (pimple.loop())
        {
            #include "fluid/UEqn.H"

            #include "ftEqn.H"
            #include "bEqn.H"
            #include "EauEqn.H"
            #include "EaEqn.H"

            if (!ign.ignited())
            {
                thermo.heu() == thermo.he();
            }

            // --- Pressure corrector loop
            while (pimple.correct())
            {
                #include "fluid/pEqn.H"
            }

            if (pimple.turbCorr())
            {
                turbulence->correct();
            }
        }

        #include "logSummary.H"

        rho = thermo.rho();

        runTime.write();

        runTime.printExecutionTime(Info);
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
