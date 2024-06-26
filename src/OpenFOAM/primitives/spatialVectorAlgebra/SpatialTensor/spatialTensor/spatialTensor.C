/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
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

#include "primitives/spatialVectorAlgebra/SpatialTensor/spatialTensor/spatialTensor.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

template<>
const char* const Foam::spatialTensor::vsType::typeName = "spatialTensor";

template<>
const char* const Foam::spatialTensor::vsType::componentNames[] =
{
    "Exx",  "Exy",  "Exz",    "Erxx", "Erxy", "Erxz",
    "Eyx",  "Eyy",  "Eyz",    "Eryx", "Eryy", "Eryz",
    "Ezx",  "Ezy",  "Ezz",    "Erzx", "Erzy", "Erzz",

    "Erxx", "Erxy", "Erxz",   "Exx",  "Exy",  "Exz",
    "Eryx", "Eryy", "Eryz",   "Eyx",  "Eyy",  "Eyz",
    "Erzx", "Erzy", "Erzz",   "Ezx",  "Ezy",  "Ezz"
};

template<>
const Foam::spatialTensor Foam::spatialTensor::vsType::zero
(
    Foam::spatialTensor::uniform(0)
);

template<>
const Foam::spatialTensor Foam::spatialTensor::vsType::one
(
    spatialTensor::uniform(1)
);

template<>
const Foam::spatialTensor Foam::spatialTensor::vsType::max
(
    spatialTensor::uniform(VGREAT)
);

template<>
const Foam::spatialTensor Foam::spatialTensor::vsType::min
(
    spatialTensor::uniform(-VGREAT)
);

template<>
const Foam::spatialTensor Foam::spatialTensor::vsType::rootMax
(
    spatialTensor::uniform(ROOTVGREAT)
);

template<>
const Foam::spatialTensor Foam::spatialTensor::vsType::rootMin
(
    spatialTensor::uniform(-ROOTVGREAT)
);

template<>
const Foam::spatialTensor Foam::spatialTensor::I
(
    Foam::spatialTensor::identity()
);


// ************************************************************************* //
