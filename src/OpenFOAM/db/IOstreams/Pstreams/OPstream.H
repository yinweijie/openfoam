/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2013 OpenFOAM Foundation
    Copyright (C) 2021-2023 OpenCFD Ltd.
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
    Foam::OPstream

Description
    Output inter-processor communications stream.

SourceFiles
    OPstreams.C

\*---------------------------------------------------------------------------*/

#include "db/IOstreams/Pstreams/Pstream.H"

#ifndef Foam_OPstream_H
#define Foam_OPstream_H

#include "db/IOstreams/Pstreams/UOPstream.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                          Class OPstream Declaration
\*---------------------------------------------------------------------------*/

class OPstream
:
    public Pstream,
    public UOPstream
{
public:

    // Constructors

        //- Construct for given process index to send to
        //- and optional buffer size, write format
        OPstream
        (
            const UPstream::commsTypes commsType,
            const int toProcNo,
            const label bufSize = 0,
            const int tag = UPstream::msgType(),
            const label comm = UPstream::worldComm,
            IOstreamOption::streamFormat fmt = IOstreamOption::BINARY
        );
};


/*---------------------------------------------------------------------------*\
                        Class OPBstream Declaration
\*---------------------------------------------------------------------------*/

//- Output inter-processor communications stream
//- using MPI broadcast.
class OPBstream
:
    public Pstream,
    public UOPBstream
{
public:

    // Constructors

        //- Construct for broadcast root, optional buffer size, write format
        OPBstream
        (
            const UPstream::commsTypes,     //!< ignored
            const int rootProcNo,           //!< normally UPstream::masterNo()
            const label bufSize = 0,
            const int tag = UPstream::msgType(),  //!< ignored
            const label comm = UPstream::worldComm,
            IOstreamOption::streamFormat fmt = IOstreamOption::BINARY
        );

        //- Construct for broadcast root with optional communicator,
        //- write format
        explicit OPBstream
        (
            const int rootProcNo,           //!< normally UPstream::masterNo()
            const label comm = UPstream::worldComm,
            IOstreamOption::streamFormat fmt = IOstreamOption::BINARY
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
