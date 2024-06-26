/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "mixtures/reactingMixture/reactingMixture.H"
#include "fvMesh/fvMesh.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class ThermoType>
Foam::autoPtr<Foam::chemistryReader<ThermoType>>&
Foam::reactingMixture<ThermoType>::reader()
{
    return static_cast<autoPtr<chemistryReader<ThermoType>>&>(*this);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ThermoType>
Foam::reactingMixture<ThermoType>::reactingMixture
(
    const dictionary& thermoDict,
    const fvMesh& mesh,
    const word& phaseName
)
:
    speciesTable(),
    autoPtr<chemistryReader<ThermoType>>
    (
        chemistryReader<ThermoType>::New(thermoDict, *this)
    ),
    multiComponentMixture<ThermoType>
    (
        thermoDict,
        *this,
        this->reader()->speciesThermo(),
        mesh,
        phaseName
    ),
    PtrList<Reaction<ThermoType>>
    (
        this->reader()->reactions()
    ),
    speciesComposition_
    (
        this->reader()->specieComposition()
    )
{
    // Done with reader
    reader().clear();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class ThermoType>
void Foam::reactingMixture<ThermoType>::read(const dictionary& thermoDict)
{}


// ************************************************************************* //
