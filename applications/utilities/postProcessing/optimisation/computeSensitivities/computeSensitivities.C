/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2007-2019 PCOpt/NTUA
    Copyright (C) 2013-2019 FOSS GP
    Copyright (C) 2019 OpenCFD Ltd.
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
    computeSensitivities

Description
    Computes the sensitivities wrt what is defined in the optimisationDict

\*---------------------------------------------------------------------------*/

#include "cfdTools/general/include/fvCFD.H"
#include "optimisation/optimisationManager/optimisationManager/optimisationManager.H"
#include "solvers/primalSolvers/primalSolver/primalSolver.H"
#include "solvers/adjointSolvers/adjointSolver/adjointSolver.H"
#include "solvers/variablesSet/incompressible/incompressibleVars.H"
#include "solvers/variablesSet/incompressibleAdjoint/incompressibleAdjointVars.H"
#include "adjointBoundaryConditions/adjointBoundaryCondition/adjointBoundaryCondition.H"
#include "solvers/adjointSolverManager/adjointSolverManager.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "include/setRootCase.H"
    #include "include/createTime.H"
    #include "include/createMesh.H"
    #include "createFields.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    forAll(adjointSolverManagers, amI)
    {
        PtrList<adjointSolver>& adjointSolvers =
            adjointSolverManagers[amI].adjointSolvers();
        forAll(adjointSolvers, asI)
        {
            adjointSolvers[asI].getObjectiveManager().updateAndWrite();
            adjointSolvers[asI].
                computeObjectiveSensitivities(om.getDesignVariables());
        }
    }

    runTime.printExecutionTime(Info);

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
