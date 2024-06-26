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

\*---------------------------------------------------------------------------*/

#include "parcels/Templates/CollidingParcel/CollisionRecordList/WallCollisionRecord/WallCollisionRecord.H"
#include "db/IOstreams/IOstreams.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
Foam::WallCollisionRecord<Type>::WallCollisionRecord(Istream& is)
:
    accessed_(is),
    pRel_(is),
    data_(is)
{
    is.check(FUNCTION_NAME);
}


// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

template<class Type>
Foam::Istream& Foam::operator>>(Istream& is, WallCollisionRecord<Type>& wCR)
{
    is  >> wCR.accessed_ >> wCR.pRel_ >> wCR.data_;

    is.check(FUNCTION_NAME);
    return is;
}


template<class Type>
Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const WallCollisionRecord<Type>& wCR
)
{
    os  << wCR.accessed_
        << token::SPACE << wCR.pRel_
        << token::SPACE << wCR.data_;

    os.check(FUNCTION_NAME);
    return os;
}


// ************************************************************************* //
