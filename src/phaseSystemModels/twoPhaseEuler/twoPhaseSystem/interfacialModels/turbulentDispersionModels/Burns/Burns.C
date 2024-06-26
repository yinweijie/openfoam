/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2014-2017 OpenFOAM Foundation
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

#include "interfacialModels/turbulentDispersionModels/Burns/Burns.H"
#include "phasePair/phasePair.H"
#include "TurbulenceModels/phaseCompressible/PhaseCompressibleTurbulenceModel/PhaseCompressibleTurbulenceModelPascal.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

#include "interfacialModels/dragModels/dragModel/dragModel.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace turbulentDispersionModels
{
    defineTypeNameAndDebug(Burns, 0);
    addToRunTimeSelectionTable
    (
        turbulentDispersionModel,
        Burns,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::turbulentDispersionModels::Burns::Burns
(
    const dictionary& dict,
    const phasePair& pair
)
:
    turbulentDispersionModel(dict, pair),
    sigma_("sigma", dimless, dict),
    residualAlpha_
    (
        "residualAlpha",
        dimless,
        pair_.dispersed().residualAlpha().value(),
        dict
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::turbulentDispersionModels::Burns::~Burns()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::turbulentDispersionModels::Burns::D() const
{
    const fvMesh& mesh(pair_.phase1().mesh());
    const dragModel&
        drag
        (
            mesh.lookupObject<dragModel>
            (
                IOobject::groupName(dragModel::typeName, pair_.name())
            )
        );

    return
        0.75
       *drag.CdRe()
       *pair_.continuous().nu()
       *pair_.continuous().turbulence().nut()
       /(
            sigma_
           *sqr(pair_.dispersed().d())
        )
       *pair_.continuous().rho()
       *pair_.dispersed()
       *(
           1.0/max(pair_.dispersed(), residualAlpha_)
         + 1.0/max(pair_.continuous(), residualAlpha_)
        );
}


// ************************************************************************* //
