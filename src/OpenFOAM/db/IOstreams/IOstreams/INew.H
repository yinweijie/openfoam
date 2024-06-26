/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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
    Foam::INew

Description
    A helper class when constructing from an Istream or dictionary

\*---------------------------------------------------------------------------*/

#ifndef Foam_INew_H
#define Foam_INew_H

#include "memory/autoPtr/autoPtr.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class dictionary;
class Istream;

/*---------------------------------------------------------------------------*\
                           Class INew Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class INew
{
public:

    //- Default construct
    INew() noexcept = default;

    //- New from Istream
    autoPtr<T> operator()(Istream& is) const
    {
        return T::New(is);
    }

    //- New from word (unused) and Istream
    autoPtr<T> operator()(const word&, Istream& is) const
    {
        return T::New(is);
    }

    //- New from dictionary
    autoPtr<T> operator()(const dictionary& dict) const
    {
        return T::New(dict);
    }

    //- New from word (unused) and dictionary
    autoPtr<T> operator()(const word&, const dictionary& dict) const
    {
        return T::New(dict);
    }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
