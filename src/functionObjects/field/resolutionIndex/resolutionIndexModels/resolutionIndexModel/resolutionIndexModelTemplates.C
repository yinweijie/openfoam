/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 OpenCFD Ltd.
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

#include "resolutionIndex/resolutionIndexModels/resolutionIndexModel/resolutionIndexModel.H"
#include "fields/volFields/volFields.H"

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class GeoFieldType>
GeoFieldType& Foam::resolutionIndexModel::getOrReadField
(
    const word& fieldName
) const
{
    auto* ptr = mesh_.getObjectPtr<GeoFieldType>(fieldName);

    if (!ptr)
    {
        ptr = new GeoFieldType
        (
            IOobject
            (
                fieldName,
                mesh_.time().timeName(),
                mesh_,
                IOobject::MUST_READ,
                IOobject::AUTO_WRITE,
                IOobject::REGISTER
            ),
            mesh_
        );
        mesh_.objectRegistry::store(ptr);
    }

    return *ptr;
}


// ************************************************************************* //
