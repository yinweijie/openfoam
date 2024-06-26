/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2018 OpenFOAM Foundation
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

#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

#include "phaseSystem/phaseSystem.H"
#include "twoPhaseSystem.H"
#include "PhaseSystems/MomentumTransferPhaseSystem/MomentumTransferPhaseSystem.H"
#include "PhaseSystems/OneResistanceHeatTransferPhaseSystem/OneResistanceHeatTransferPhaseSystem.H"
#include "PhaseSystems/TwoResistanceHeatTransferPhaseSystem/TwoResistanceHeatTransferPhaseSystem.H"
#include "PhaseSystems/PhaseTransferPhaseSystem/PhaseTransferPhaseSystem.H"
#include "PhaseSystems/PopulationBalancePhaseSystem/PopulationBalancePhaseSystem.H"
#include "PhaseSystems/InterfaceCompositionPhaseChangePhaseSystem/InterfaceCompositionPhaseChangePhaseSystem.H"
#include "PhaseSystems/ThermalPhaseChangePhaseSystem/ThermalPhaseChangePhaseSystem.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    typedef
        PhaseTransferPhaseSystem
        <
            OneResistanceHeatTransferPhaseSystem
            <
                MomentumTransferPhaseSystem<twoPhaseSystem>
            >
        >
        basicTwoPhaseSystem;

    addNamedToRunTimeSelectionTable
    (
        twoPhaseSystem,
        basicTwoPhaseSystem,
        dictionary,
        basicTwoPhaseSystem
    );

    typedef
        InterfaceCompositionPhaseChangePhaseSystem
        <
            PhaseTransferPhaseSystem
            <
                TwoResistanceHeatTransferPhaseSystem
                <
                    MomentumTransferPhaseSystem<twoPhaseSystem>
                >
            >
        >
        interfaceCompositionPhaseChangeTwoPhaseSystem;

    addNamedToRunTimeSelectionTable
    (
        twoPhaseSystem,
        interfaceCompositionPhaseChangeTwoPhaseSystem,
        dictionary,
        interfaceCompositionPhaseChangeTwoPhaseSystem
    );

    typedef
        ThermalPhaseChangePhaseSystem
        <
            PhaseTransferPhaseSystem
            <
                TwoResistanceHeatTransferPhaseSystem
                <
                    MomentumTransferPhaseSystem<twoPhaseSystem>
                >
            >
        >
        thermalPhaseChangeTwoPhaseSystem;

    addNamedToRunTimeSelectionTable
    (
        twoPhaseSystem,
        thermalPhaseChangeTwoPhaseSystem,
        dictionary,
        thermalPhaseChangeTwoPhaseSystem
    );

    typedef
        PopulationBalancePhaseSystem
        <
            PhaseTransferPhaseSystem
            <
                OneResistanceHeatTransferPhaseSystem
                <
                    MomentumTransferPhaseSystem<twoPhaseSystem>
                >
            >
        >
        populationBalanceTwoPhaseSystem;

    addNamedToRunTimeSelectionTable
    (
        twoPhaseSystem,
        populationBalanceTwoPhaseSystem,
        dictionary,
        populationBalanceTwoPhaseSystem
    );

    typedef
        ThermalPhaseChangePhaseSystem
        <
            PopulationBalancePhaseSystem
            <
                PhaseTransferPhaseSystem
                <
                    TwoResistanceHeatTransferPhaseSystem
                    <
                        MomentumTransferPhaseSystem<twoPhaseSystem>
                    >
                >
            >
        >
        thermalPhaseChangePopulationBalanceTwoPhaseSystem;

        addNamedToRunTimeSelectionTable
        (
            twoPhaseSystem,
            thermalPhaseChangePopulationBalanceTwoPhaseSystem,
            dictionary,
            thermalPhaseChangePopulationBalanceTwoPhaseSystem
        );
}


// ************************************************************************* //
