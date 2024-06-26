/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2023 OpenCFD Ltd.
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

#include "fields/fvsPatchFields/basic/sliced/slicedFvsPatchField.H"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
Foam::slicedFvsPatchField<Type>::slicedFvsPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, surfaceMesh>& iF,
    const Field<Type>& completeOrBoundaryField,
    const bool isBoundaryOnly
)
:
    fvsPatchField<Type>(p, iF, Field<Type>())
{
    if (isBoundaryOnly)
    {
        // Set to a slice of the boundary field
        UList<Type>::shallowCopy(p.boundarySlice(completeOrBoundaryField));
    }
    else
    {
        // Set to a slice of the complete field
        UList<Type>::shallowCopy(p.patchSlice(completeOrBoundaryField));
    }
}


template<class Type>
Foam::slicedFvsPatchField<Type>::slicedFvsPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, surfaceMesh>& iF
)
:
    fvsPatchField<Type>(p, iF)
{}


template<class Type>
Foam::slicedFvsPatchField<Type>::slicedFvsPatchField
(
    const fvPatch& p,
    const DimensionedField<Type, surfaceMesh>& iF,
    const dictionary& dict
)
:
    fvsPatchField<Type>(p, iF)  // bypass dictionary constructor
{
    fvsPatchFieldBase::readDict(dict);
    // Read "value" if present...

    NotImplemented;
}


template<class Type>
Foam::slicedFvsPatchField<Type>::slicedFvsPatchField
(
    const slicedFvsPatchField<Type>& ptf,
    const fvPatch& p,
    const DimensionedField<Type, surfaceMesh>& iF,
    const fvPatchFieldMapper& mapper
)
:
    fvsPatchField<Type>(ptf, p, iF, mapper)
{
    NotImplemented;
}


template<class Type>
Foam::slicedFvsPatchField<Type>::slicedFvsPatchField
(
    const slicedFvsPatchField<Type>& ptf,
    const DimensionedField<Type, surfaceMesh>& iF
)
:
    fvsPatchField<Type>(ptf.patch(), iF, Field<Type>())
{
    // Transfer the slice from the argument
    UList<Type>::shallowCopy(ptf);
}


template<class Type>
Foam::tmp<Foam::fvsPatchField<Type>>
Foam::slicedFvsPatchField<Type>::clone() const
{
    return tmp<fvsPatchField<Type>>
    (
        new slicedFvsPatchField<Type>(*this)
    );
}


template<class Type>
Foam::slicedFvsPatchField<Type>::slicedFvsPatchField
(
    const slicedFvsPatchField<Type>& ptf
)
:
    fvsPatchField<Type>
    (
        ptf.patch(),
        ptf.internalField(),
        Field<Type>()
    )
{
    // Transfer the slice from the argument
    UList<Type>::shallowCopy(ptf);
}


template<class Type>
Foam::tmp<Foam::fvsPatchField<Type>>
Foam::slicedFvsPatchField<Type>::clone
(
    const DimensionedField<Type, surfaceMesh>& iF
) const
{
    return tmp<fvsPatchField<Type>>
    (
        new slicedFvsPatchField<Type>(*this, iF)
    );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
Foam::slicedFvsPatchField<Type>::~slicedFvsPatchField()
{
    // Set to nullptr to avoid deletion of underlying field
    UList<Type>::shallowCopy(UList<Type>());
}


// ************************************************************************* //
