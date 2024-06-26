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
    Foam::errorManip

Description
    Error stream manipulators for exit and abort which may terminate the
    program or throw an exception depending if the exception
    handling has been switched on (off by default).

Usage
    \code
        error << "message " << someType << abort(error);
        error << "message " << someType << exit(error, errNo);
    \endcode

Class
    Foam::errorManipArg

Description
    Error stream manipulators for functions with an argument.

\*---------------------------------------------------------------------------*/

#ifndef errorManip_H
#define errorManip_H

#include "db/error/error.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
template<class Err> class errorManip;
template<class Err> Ostream& operator<<(Ostream&, errorManip<Err>);

template<class Err, class T> class errorManipArg;
template<class Err, class T>
Ostream& operator<<(Ostream&, errorManipArg<Err, T>);


/*---------------------------------------------------------------------------*\
                         Class errorManip Declaration
\*---------------------------------------------------------------------------*/

template<class Err>
class errorManip
{
    void (Err::*fPtr_)();
    Err& err_;

public:

    errorManip(void (Err::*fPtr)(), Err& t)
    :
        fPtr_(fPtr),
        err_(t)
    {}

    friend Ostream& operator<< <Err>(Ostream& os, errorManip<Err> m);
};


template<class Err>
inline Ostream& operator<<(Ostream& os, errorManip<Err> m)
{
    (m.err_.*m.fPtr_)();
    return os;
}


/*---------------------------------------------------------------------------*\
                        Class errorManipArg Declaration
\*---------------------------------------------------------------------------*/

template<class Err, class T>
class errorManipArg
{
    void (Err::*fPtr_)(const T);
    Err& err_;
    T arg_;

public:

    errorManipArg(void (Err::*fPtr)(const T), Err& t, const T i)
    :
        fPtr_(fPtr),
        err_(t),
        arg_(i)
    {}

    friend Ostream& operator<< <Err, T>(Ostream& os, errorManipArg<Err, T> m);
};


template<class Err, class T>
inline Ostream& operator<<(Ostream& os, errorManipArg<Err, T> m)
{
    (m.err_.*m.fPtr_)(m.arg_);
    return os;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

inline errorManipArg<error, int>
exit(error& err, const int errNo = 1)
{
    return errorManipArg<error, int>(&error::exit, err, errNo);
}


inline errorManipArg<IOerror, int>
exit(IOerror& err, const int errNo = 1)
{
    return errorManipArg<IOerror, int>(&IOerror::exit, err, errNo);
}


inline errorManip<error>
abort(error& err)
{
    return errorManip<error>(&error::abort, err);
}


inline errorManip<IOerror>
abort(IOerror& err)
{
    return errorManip<IOerror>(&IOerror::abort, err);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
