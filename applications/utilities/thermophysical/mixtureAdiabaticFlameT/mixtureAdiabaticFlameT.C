/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2021 OpenCFD Ltd.
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
    mixtureAdiabaticFlameT

Group
    grpThermophysicalUtilities

Description
    Calculate adiabatic flame temperature for a given mixture
    at a given temperature.

\*---------------------------------------------------------------------------*/

#include "global/argList/argList.H"
#include "db/dictionary/dictionary.H"
#include "db/IOstreams/Fstreams/IFstream.H"
#include "include/OSspecific.H"
#include "global/etcFiles/etcFiles.H"

#include "specie/specie.H"
#include "equationOfState/perfectGas/perfectGas.H"
#include "thermo/thermo/thermo.H"
#include "thermo/janaf/janafThermo.H"
#include "thermo/absoluteEnthalpy/absoluteEnthalpy.H"
#include "mixture.H"

using namespace Foam;

typedef species::thermo<janafThermo<perfectGas<specie>>, absoluteEnthalpy>
    thermo;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Calculate adiabatic flame temperature for a given mixture"
        " at a given temperature"
    );

    argList::noParallel();
    argList::noFunctionObjects();  // Never use function objects
    argList::addArgument("controlFile");

    argList args(argc, argv);

    const auto controlFileName = args.get<fileName>(1);

    // Construct control dictionary
    IFstream controlFile(controlFileName);

    // Check controlFile stream is OK
    if (!controlFile.good())
    {
        FatalErrorInFunction
            << "Cannot read file " << controlFileName
            << abort(FatalError);
    }

    dictionary control(controlFile);


    const scalar P(control.get<scalar>("P"));
    const scalar T0(control.get<scalar>("T0"));
    mixture rMix(control.lookup("reactants"));
    mixture pMix(control.lookup("products"));


    Info<< nl << "Reading thermodynamic data dictionary" << endl;

    fileName thermoDataFileName(findEtcFile("thermoData/thermoData"));

    // Construct control dictionary
    IFstream thermoDataFile(thermoDataFileName);

    // Check thermoData stream is OK
    if (!thermoDataFile.good())
    {
        FatalErrorInFunction
            << "Cannot read file " << thermoDataFileName
            << abort(FatalError);
    }

    dictionary thermoData(thermoDataFile);


    thermo reactants
    (
        rMix[0].volFrac()*thermo(thermoData.subDict(rMix[0].name()))
    );

    for (label i = 1; i < rMix.size(); i++)
    {
        reactants = reactants
            + rMix[i].volFrac()*thermo(thermoData.subDict(rMix[i].name()));
    }


    thermo products
    (
        2*pMix[0].volFrac()*thermo(thermoData.subDict(pMix[0].name()))
    );

    for (label i = 1; i < pMix.size(); i++)
    {
        products = products
            + 2*pMix[i].volFrac()*thermo(thermoData.subDict(pMix[i].name()));
    }

    Info<< "Adiabatic flame temperature of mixture " << rMix.name() << " = "
         << products.THa(reactants.Ha(P, T0), P, 1000.0) << " K" << endl;

    return 0;
}


// ************************************************************************* //
