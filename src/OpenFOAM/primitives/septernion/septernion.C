/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2020 OpenCFD Ltd.
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

#include "primitives/septernion/septernion.H"
#include "db/IOstreams/IOstreams.H"
#include "db/IOstreams/StringStreams/StringStream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

const char* const Foam::septernion::typeName = "septernion";
const Foam::septernion Foam::septernion::zero(Zero);

const Foam::septernion Foam::septernion::I
(
    vector(Zero),
    quaternion(scalar(1))
);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::septernion::septernion(Istream& is)
{
    is >> *this;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

Foam::word Foam::name(const septernion& s)
{
    OStringStream buf;
    buf << '(' << s.t() << ',' << s.r() << ')';
    return buf.str();
}


Foam::septernion Foam::slerp
(
    const septernion& sa,
    const septernion& sb,
    const scalar t
)
{
    return septernion((1 - t)*sa.t() + t*sb.t(), slerp(sa.r(), sb.r(), t));
}


Foam::septernion Foam::average
(
    const UList<septernion>& ss,
    const UList<scalar> w
)
{
    septernion sa(w[0]*ss[0]);

    for (label i=1; i<ss.size(); i++)
    {
        sa.t() += w[i]*ss[i].t();

        // Invert quaternion if it has the opposite sign to the average
        if ((sa.r() & ss[i].r()) > 0)
        {
            sa.r() += w[i]*ss[i].r();
        }
        else
        {
            sa.r() -= w[i]*ss[i].r();
        }
    }

    sa.r().normalise();

    return sa;
}


// * * * * * * * * * * * * * * * IOstream Operators  * * * * * * * * * * * * //

Foam::Istream& Foam::operator>>(Istream& is, septernion& s)
{
    is.readBegin("septernion");

    is  >> s.t() >> s.r();

    is.readEnd("septernion");

    is.check(FUNCTION_NAME);
    return is;
}


Foam::Ostream& Foam::operator<<(Ostream& os, const septernion& s)
{
    os  << token::BEGIN_LIST
        << s.t() << token::SPACE << s.r()
        << token::END_LIST;

    return os;
}


// ************************************************************************* //
