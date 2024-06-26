/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2017-2023 OpenCFD Ltd.
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
    Foam::OFstream

Description
    Output to file stream, using an OSstream

SourceFiles
    OFstream.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_OFstream_H
#define Foam_OFstream_H

#include "db/IOstreams/Sstreams/OSstream.H"
#include "db/typeInfo/className.H"
#include "db/IOstreams/Fstreams/fstreamPointer.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                          Class OFstream Declaration
\*---------------------------------------------------------------------------*/

class OFstream
:
    private Foam::ofstreamPointer,
    public OSstream
{
public:

    //- Declare type-name (with debug switch)
    ClassName("OFstream");


    // Constructors

        //- Construct a null output file stream.
        //  Behaves like \c /dev/null and is named accordingly
        explicit OFstream(std::nullptr_t);

        //- Construct with specified atomic behaviour
        //- from pathname, stream option, optional append
        OFstream
        (
            IOstreamOption::atomicType atomic,
            const fileName& pathname,
            IOstreamOption streamOpt = IOstreamOption(),
            IOstreamOption::appendType append = IOstreamOption::NON_APPEND
        );

        //- Construct from pathname and other specifications
        explicit OFstream
        (
            const fileName& pathname,
            IOstreamOption streamOpt = IOstreamOption(),
            IOstreamOption::appendType append = IOstreamOption::NON_APPEND
        )
        :
            OFstream(IOstreamOption::NON_ATOMIC, pathname, streamOpt, append)
        {}

        //- Construct from pathname, format (uncompressed), optional append,
        //- atomic behaviour as per system default
        OFstream
        (
            const fileName& pathname,
            IOstreamOption::streamFormat fmt,
            IOstreamOption::compressionType cmp = IOstreamOption::UNCOMPRESSED,
            IOstreamOption::appendType append = IOstreamOption::NON_APPEND
        )
        :
            OFstream(pathname, IOstreamOption(fmt, cmp), append)
        {}

        //- Construct with specified atomic behaviour
        //- from pathname, format (uncompressed), optional append
        OFstream
        (
            IOstreamOption::atomicType atomic,
            const fileName& pathname,
            IOstreamOption::streamFormat fmt,
            IOstreamOption::compressionType cmp = IOstreamOption::UNCOMPRESSED,
            IOstreamOption::appendType append = IOstreamOption::NON_APPEND
        )
        :
            OFstream(atomic, pathname, IOstreamOption(fmt, cmp), append)
        {}


    //- Destructor. Possibly invokes an atomic rename
    //- (preference defined during construction)
    ~OFstream();


    // Member Functions

        //- Read/write access to the name of the stream
        using OSstream::name;


    // STL stream

        //- Const access to underlying std::ostream
        virtual const std::ostream& stdStream() const override;

        //- Access to underlying std::ostream
        virtual std::ostream& stdStream() override;

        //- Rewind the stream so that it may be written again.
        //- Reopens the file (truncation)
        virtual void rewind();


    // Print

        //- Print stream description
        void print(Ostream& os) const override;


    // Additional constructors and methods
    #ifdef Foam_IOstream_extras

        //- Construct from pathname, format (version, compression)
        FOAM_DEPRECATED_FOR(2022-09, "Construct without specifying version")
        OFstream
        (
            const fileName& pathname,
            IOstreamOption::streamFormat fmt,
            IOstreamOption::versionNumber ver,
            IOstreamOption::compressionType cmp = IOstreamOption::UNCOMPRESSED,
            IOstreamOption::appendType append = IOstreamOption::NON_APPEND
        )
        :
            OFstream(pathname, IOstreamOption(fmt, ver, cmp), append)
        {}

    #endif /* Foam_IOstream_extras */
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

//- Global predefined null output stream "/dev/null"
extern OFstream Snull;

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
