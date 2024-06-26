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

#include "BlendedInterfacialModel/BlendedInterfacialModel.H"
#include "interfacialModels/dragModels/dragModel/dragModel.H"
#include "interfacialModels/virtualMassModels/virtualMassModel/virtualMassModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class modelType>
const modelType& twoPhaseSystem::lookupSubModel
(
    const phasePair& key
) const
{
    return
        mesh().lookupObject<modelType>
        (
            IOobject::groupName(modelType::typeName, key.name())
        );
}


template<>
inline const dragModel& twoPhaseSystem::lookupSubModel<dragModel>
(
    const phaseModel& dispersed,
    const phaseModel& continuous
) const
{
    return drag_->phaseModel(dispersed);
}


template<>
inline const virtualMassModel& twoPhaseSystem::lookupSubModel<virtualMassModel>
(
    const phaseModel& dispersed,
    const phaseModel& continuous
) const
{
    return virtualMass_->phaseModel(dispersed);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
