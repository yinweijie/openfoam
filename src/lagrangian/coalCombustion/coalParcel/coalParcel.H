/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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

Class
    Foam::coalParcel

Description
    Definition of coal parcel

SourceFiles
    CoalParcel.C

\*---------------------------------------------------------------------------*/

#ifndef coalParcel_H
#define coalParcel_H

#include "primitives/traits/contiguous.H"
#include "particle/particle.H"
#include "parcels/Templates/KinematicParcel/KinematicParcel.H"
#include "parcels/Templates/ThermoParcel/ThermoParcel.H"
#include "parcels/Templates/ReactingParcel/ReactingParcel.H"
#include "parcels/Templates/ReactingMultiphaseParcel/ReactingMultiphaseParcel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    typedef ReactingMultiphaseParcel
    <
        ReactingParcel
        <
            ThermoParcel
            <
                KinematicParcel
                <
                    particle
                >
            >
        >
    > coalParcel;

    //- Non-contiguous data for coalParcel
    template<> struct is_contiguous<coalParcel> : std::false_type {};
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
