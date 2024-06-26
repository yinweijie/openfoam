/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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

#include "fields/faePatchFields/basic/sliced/slicedFaePatchField.H"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
Foam::slicedFaePatchField<Type>::slicedFaePatchField
(
    const faPatch& p,
    const DimensionedField<Type, edgeMesh>& iF,
    const Field<Type>& completeOrBoundaryField,
    const bool isBoundaryOnly
)
:
    faePatchField<Type>(p, iF, Field<Type>())
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
Foam::slicedFaePatchField<Type>::slicedFaePatchField
(
    const faPatch& p,
    const DimensionedField<Type, edgeMesh>& iF
)
:
    faePatchField<Type>(p, iF)
{}


template<class Type>
Foam::slicedFaePatchField<Type>::slicedFaePatchField
(
    const faPatch& p,
    const DimensionedField<Type, edgeMesh>& iF,
    const dictionary& dict
)
:
    faePatchField<Type>(p, iF)  // bypass dictionary constructor
{
    faePatchFieldBase::readDict(dict);
    // Read "value" if present...

    NotImplemented;
}


template<class Type>
Foam::slicedFaePatchField<Type>::slicedFaePatchField
(
    const slicedFaePatchField<Type>& ptf,
    const faPatch& p,
    const DimensionedField<Type, edgeMesh>& iF,
    const faPatchFieldMapper& mapper
)
:
    faePatchField<Type>(ptf, p, iF, mapper)
{
    NotImplemented;
}


template<class Type>
Foam::slicedFaePatchField<Type>::slicedFaePatchField
(
    const slicedFaePatchField<Type>& ptf,
    const DimensionedField<Type, edgeMesh>& iF
)
:
    faePatchField<Type>(ptf.patch(), iF, Field<Type>())
{
    // Transfer the slice from the argument
    UList<Type>::shallowCopy(ptf);
}


template<class Type>
Foam::tmp<Foam::faePatchField<Type>>
Foam::slicedFaePatchField<Type>::clone() const
{
    return tmp<faePatchField<Type>>
    (
        new slicedFaePatchField<Type>(*this)
    );
}


template<class Type>
Foam::slicedFaePatchField<Type>::slicedFaePatchField
(
    const slicedFaePatchField<Type>& ptf
)
:
    faePatchField<Type>
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
Foam::tmp<Foam::faePatchField<Type>>
Foam::slicedFaePatchField<Type>::clone
(
    const DimensionedField<Type, edgeMesh>& iF
) const
{
    return tmp<faePatchField<Type>>
    (
        new slicedFaePatchField<Type>(*this, iF)
    );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
Foam::slicedFaePatchField<Type>::~slicedFaePatchField()
{
    // Set to nullptr to avoid deletion of underlying field
    UList<Type>::shallowCopy(UList<Type>());
}


// ************************************************************************* //
