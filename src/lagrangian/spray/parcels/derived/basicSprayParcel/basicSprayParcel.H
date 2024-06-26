/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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
    Foam::basicSprayParcel

Description
    Definition of spray parcel

SourceFiles
    basicSprayParcel.C
    basicSprayParcelIO.C

\*---------------------------------------------------------------------------*/

#ifndef basicSprayParcel_H
#define basicSprayParcel_H

#include "primitives/traits/contiguous.H"
#include "particle/particle.H"
#include "parcels/Templates/KinematicParcel/KinematicParcel.H"
#include "parcels/Templates/ThermoParcel/ThermoParcel.H"
#include "parcels/Templates/ReactingParcel/ReactingParcel.H"
#include "parcels/Templates/SprayParcel/SprayParcel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    typedef SprayParcel
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
    > basicSprayParcel;

    //- Non-contiguous data for basicSprayParcel
    template<> struct is_contiguous<basicSprayParcel> : std::false_type {};
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
