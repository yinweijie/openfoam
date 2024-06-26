/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2021 OpenCFD Ltd.
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

#include "primitives/functions/Function1/None/NoneFunction1.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
Foam::Function1Types::None<Type>::None
(
    const word& entryName,
    const dictionary& dict,
    const objectRegistry* obrPtr
)
:
    Function1<Type>(entryName, dict, obrPtr),
    context_(dict.relativeName())
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
Type Foam::Function1Types::None<Type>::value(const scalar) const
{
    FatalErrorInFunction
        << "Function " << this->name() << " is 'none' in " << context_ << nl
        << exit(FatalError);

    return pTraits<Type>::zero;
}


template<class Type>
Type Foam::Function1Types::None<Type>::integral
(
    const scalar x1,
    const scalar x2
)
const
{
    FatalErrorInFunction
        << "Function " << this->name() << " is 'none' in " << context_ << nl
        << exit(FatalError);

    return pTraits<Type>::zero;
}


template<class Type>
Foam::tmp<Foam::Field<Type>> Foam::Function1Types::None<Type>::value
(
    const scalarField& x
) const
{
    FatalErrorInFunction
        << "Function " << this->name() << " is 'none' in " << context_ << nl
        << exit(FatalError);

    return nullptr;
}


template<class Type>
void Foam::Function1Types::None<Type>::writeData(Ostream& os) const
{
    // OR:  os.writeEntry(this->name(), this->type());
    Function1<Type>::writeData(os);
    os.endEntry();
}


// ************************************************************************* //
