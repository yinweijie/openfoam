/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2023 OpenCFD Ltd.
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
    Foam::ensightReadFile

Description
    A variant of IFstream with specialised read() for
    strings, integers and floats.
    Correctly handles binary read as well.

\*---------------------------------------------------------------------------*/

#ifndef Foam_ensightReadFile_H
#define Foam_ensightReadFile_H

#include "db/IOstreams/Fstreams/IFstream.H"
#include "db/IOstreams/IOstreams/IOstream.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class ensightReadFile Declaration
\*---------------------------------------------------------------------------*/

class ensightReadFile
:
    public IFstream
{
public:

    // Generated Methods

        //- No copy construct
        ensightReadFile(const ensightReadFile&) = delete;

        //- No copy assignment
        void operator=(const ensightReadFile&) = delete;


    // Constructors

        //- Construct from pathname, auto-detect the format
        explicit ensightReadFile(const fileName& pathname);

        //- Construct from pathname, use the specified (ascii/binary) format
        ensightReadFile
        (
            const fileName& pathname,
            IOstreamOption::streamFormat fmt
        );


    //- Destructor
    ~ensightReadFile() = default;


    // Static Functions

        //- Detect if the file is \em binary by testing for initial
        //- "(C|Fortran) Binary"
        static IOstreamOption::streamFormat detectBinaryHeader
        (
            const fileName& pathname
        );


    // Read Functions

        //- Inherit read from Istream
        using Istream::read;

        //- Binary read
        virtual Istream& read(char* buf, std::streamsize count) override;

        //- Read string as "%80s" or as binary
        virtual Istream& read(string& value) override;

        //- Read integer as "%10d" or as binary
        virtual Istream& read(label& value) override;

        //- Read floating-point as "%12.5e" or as binary
        virtual Istream& read(float& value) override;

        //- Read floating-point as "%12.5e" or as a binary (narrowed) float
        virtual Istream& read(double& value) override;

        //- Read element keyword. Currently the same as read(string)
        Istream& readKeyword(string& key);

        //- Read "C Binary" for binary files (eg, geometry/measured)
        Istream& readBinaryHeader();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
