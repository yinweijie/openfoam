/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "chemistryModel/TDACChemistryModel/reduction/chemistryReductionMethod/chemistryReductionMethod.H"
#include "chemistryModel/TDACChemistryModel/TDACChemistryModel.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CompType, class ThermoType>
Foam::chemistryReductionMethod<CompType, ThermoType>::chemistryReductionMethod
(
    const Foam::IOdictionary& dict,
    Foam::TDACChemistryModel<CompType, ThermoType>& chemistry
)
:
    dict_(dict),
    coeffsDict_(dict.subDict("reduction")),
    active_(coeffsDict_.getOrDefault<Switch>("active", false)),
    log_(coeffsDict_.getOrDefault<Switch>("log", false)),
    chemistry_(chemistry),
    activeSpecies_(chemistry.nSpecie(), false),
    NsSimp_(chemistry.nSpecie()),
    nSpecie_(chemistry.nSpecie()),
    tolerance_(coeffsDict_.getOrDefault<scalar>("tolerance", 1e-4))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CompType, class ThermoType>
Foam::chemistryReductionMethod<CompType, ThermoType>::
~chemistryReductionMethod()
{}


// ************************************************************************* //
