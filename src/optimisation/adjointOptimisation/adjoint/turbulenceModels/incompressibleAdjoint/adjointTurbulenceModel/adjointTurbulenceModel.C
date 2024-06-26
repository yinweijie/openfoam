/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2007-2019 PCOpt/NTUA
    Copyright (C) 2013-2019 FOSS GP
    Copyright (C) 2019-2021 OpenCFD Ltd.
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

#include "turbulenceModels/incompressibleAdjoint/adjointTurbulenceModel/adjointTurbulenceModel.H"
#include "fields/volFields/volFields.H"
#include "fields/surfaceFields/surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace incompressibleAdjoint
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

defineTypeNameAndDebug(adjointTurbulenceModel, 0);
defineRunTimeSelectionTable(adjointTurbulenceModel, adjointTurbulenceModel);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

adjointTurbulenceModel::adjointTurbulenceModel
(
    incompressibleVars& primalVars,
    incompressibleAdjointMeanFlowVars& adjointVars,
    objectiveManager& objManager,
    const word& adjointTurbulenceModelName
)
:
    regIOobject
    (
        IOobject
        (
            adjointTurbulenceModelName,
            primalVars.U().time().constant(),
            primalVars.U().db(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        )
    ),
    primalVars_(primalVars),
    adjointVars_(adjointVars),
    runTime_(primalVars.U().time()),
    mesh_(primalVars.U().mesh())
{}


// * * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * //

autoPtr<adjointTurbulenceModel> adjointTurbulenceModel::New
(
    incompressibleVars& primalVars,
    incompressibleAdjointMeanFlowVars& adjointVars,
    objectiveManager& objManager,
    const word& adjointTurbulenceModelName
)
{
    const word modelType
    (
        IOdictionary
        (
            IOobject
            (
                "turbulenceProperties",
                primalVars.U().time().constant(),
                primalVars.U().db(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            )
        ).get<word>("simulationType")
    );

    Info<< "Selecting turbulence model type " << modelType << endl;

    auto* ctorPtr = adjointTurbulenceModelConstructorTable(modelType);

    if (!ctorPtr)
    {
        FatalErrorInLookup
        (
            "adjointTurbulenceModel",
            modelType,
            *adjointTurbulenceModelConstructorTablePtr_
        ) << exit(FatalError);
    }

    return autoPtr<adjointTurbulenceModel>
    (
        ctorPtr
        (
            primalVars,
            adjointVars,
            objManager,
            adjointTurbulenceModelName
        )
    );
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void adjointTurbulenceModel::correct()
{
    primalVars_.laminarTransport().correct();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace incompressibleAdjoint
} // End namespace Foam

// ************************************************************************* //
