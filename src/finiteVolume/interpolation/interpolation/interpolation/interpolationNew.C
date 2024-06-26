/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2019-2021 OpenCFD Ltd.
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

#include "interpolation/interpolation/interpolation/interpolation.H"
#include "fields/volFields/volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
Foam::autoPtr<Foam::interpolation<Type>> Foam::interpolation<Type>::New
(
    const word& interpolationType,
    const GeometricField<Type, fvPatchField, volMesh>& psi
)
{
    auto* ctorPtr = dictionaryConstructorTable(interpolationType);

    if (!ctorPtr)
    {
        FatalErrorInLookup
        (
            "interpolation",
            interpolationType,
            *dictionaryConstructorTablePtr_
        ) << exit(FatalError);
    }

    return autoPtr<interpolation<Type>>(ctorPtr(psi));
}


template<class Type>
Foam::autoPtr<Foam::interpolation<Type>> Foam::interpolation<Type>::New
(
    const dictionary& interpolationSchemes,
    const GeometricField<Type, fvPatchField, volMesh>& psi
)
{
    return New(word(interpolationSchemes.lookup(psi.name())), psi);
}


// ************************************************************************* //
