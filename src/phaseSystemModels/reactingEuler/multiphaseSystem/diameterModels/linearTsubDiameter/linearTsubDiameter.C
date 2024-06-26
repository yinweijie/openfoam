/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018-2019 OpenFOAM Foundation
    Copyright (C) 2020-2022 OpenCFD Ltd.
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

#include "diameterModels/linearTsubDiameter/linearTsubDiameter.H"
#include "phaseSystem/phaseSystem.H"
#include "saturationModel/saturationModel.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace diameterModels
{
    defineTypeNameAndDebug(linearTsub, 0);

    addToRunTimeSelectionTable
    (
        diameterModel,
        linearTsub,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::diameterModels::linearTsub::linearTsub
(
    const dictionary& diameterProperties,
    const phaseModel& phase
)
:
    diameterModel(diameterProperties, phase),
    liquidPhaseName_(diameterProperties.lookup("liquidPhase")),
    d2_("d2", dimLength, diameterProperties.getOrDefault("d2", 0.0015)),
    Tsub2_
    (
        "Tsub2",
         dimTemperature,
         diameterProperties.getOrDefault("Tsub2", 0)
    ),
    d1_
    (
        "d1",
        dimLength,
        diameterProperties.getOrDefault("d1", 0.00015)
    ),
    Tsub1_
    (
        "Tsub1",
        dimTemperature,
        diameterProperties.getOrDefault("Tsub1", 13.5)
    ),
    d_
    (
        IOobject
        (
            IOobject::groupName("d", phase.name()),
            phase_.time().timeName(),
            phase_.mesh(),
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        phase_.mesh(),
        d1_
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::diameterModels::linearTsub::~linearTsub()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField> Foam::diameterModels::linearTsub::d() const
{
    return d_;
}


void Foam::diameterModels::linearTsub::correct()
{
    // Lookup the fluid model
    const phaseSystem& fluid =
        refCast<const phaseSystem>
        (
            phase_.mesh().lookupObject<phaseSystem>("phaseProperties")
        );

    const phaseModel& liquid(fluid.phases()[liquidPhaseName_]);

    if (phase_.mesh().foundObject<saturationModel>("saturationModel"))
    {
        const saturationModel& satModel =
            phase_.mesh().lookupObject<saturationModel>("saturationModel");

        const volScalarField Tsub
        (
            satModel.Tsat(liquid.thermo().p()) - liquid.thermo().T()
        );

        d_ = max
        (
            d1_,
            min
            (
                d2_,
                (d1_*(Tsub - Tsub2_) - d2_*(Tsub - Tsub1_))/(Tsub1_ - Tsub2_)
            )
        );
    }
}


bool Foam::diameterModels::linearTsub::read(const dictionary& phaseProperties)
{
    diameterModel::read(phaseProperties);
    diameterProperties_.readEntry("liquidPhase", liquidPhaseName_);
    diameterProperties_.readEntry("d2", d2_);
    diameterProperties_.readEntry("Tsub2", Tsub2_);
    diameterProperties_.readEntry("d1", d1_);
    diameterProperties_.readEntry("Tsub1", Tsub1_);

    return true;
}


// ************************************************************************* //
