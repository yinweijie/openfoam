/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "ensight/file/ensightFile.H"
#include "db/error/error.H"
#include "containers/Lists/List/List.H"
#include <cstring>
#include <sstream>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

bool Foam::ensightFile::allowUndef_ = false;

float Foam::ensightFile::undefValue_ = Foam::floatScalarVGREAT;

const char* const Foam::ensightFile::coordinates = "coordinates";


// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

bool Foam::ensightFile::hasUndef(const UList<float>& field)
{
    for (const float val : field)
    {
        if (std::isnan(val))
        {
            return true;
        }
    }

    return true;
}


bool Foam::ensightFile::hasUndef(const UList<double>& field)
{
    for (const double val : field)
    {
        if (std::isnan(val))
        {
            return true;
        }
    }

    return true;
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::ensightFile::init()
{
    // The ASCII formatting specs for ensight files
    setf
    (
        std::ios_base::scientific,
        std::ios_base::floatfield
    );
    precision(5);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::ensightFile::ensightFile
(
    const fileName& pathname,
    IOstreamOption::streamFormat fmt
)
:
    OFstream(IOstreamOption::ATOMIC, ensight::FileName(pathname), fmt)
{
    init();
}


Foam::ensightFile::ensightFile
(
    const fileName& path,
    const fileName& name,
    IOstreamOption::streamFormat fmt
)
:
    OFstream(IOstreamOption::ATOMIC, path/ensight::FileName(name), fmt)
{
    init();
}


// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

bool Foam::ensightFile::allowUndef() noexcept
{
    return allowUndef_;
}


// float Foam::ensightFile::undefValue() noexcept
// {
//     return undefValue_;
// }


bool Foam::ensightFile::allowUndef(bool on) noexcept
{
    bool old = allowUndef_;
    allowUndef_ = on;
    return old;
}


float Foam::ensightFile::undefValue(float value) noexcept
{
    // enable its use too
    allowUndef_ = true;

    float old = undefValue_;
    undefValue_ = value;
    return old;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::ensightFile::writeString(const char* str, size_t len)
{
    // Output 79 chars (ASCII) or 80 chars (BINARY)
    char buf[80];
    if (len > 80) len = 80;

    // TBD: truncate at newline? (shouldn't really occur anyhow)

    std::copy_n(str, len, buf);
    std::fill_n(buf + len, (80 - len), '\0');  // Pad trailing with nul

    if (format() == IOstreamOption::BINARY)
    {
        write(buf, 80);
    }
    else
    {
        buf[79] = 0;  // Max 79 in ASCII

        // TBD: Extra safety - trap newline in ASCII?
        // char* p = ::strchr(buf, '\n');
        // if (p) *p = 0;

        stdStream() << buf;
        syncState();
    }
}


void Foam::ensightFile::writeString(const char* str)
{
    writeString(str, strlen(str));
}


void Foam::ensightFile::writeString(const std::string& str)
{
    writeString(str.data(), str.size());
}


Foam::Ostream& Foam::ensightFile::write(const char* str)
{
    writeString(str, strlen(str));
    return *this;
}


Foam::Ostream& Foam::ensightFile::write(const word& str)
{
    writeString(str.data(), str.size());
    return *this;
}


Foam::Ostream& Foam::ensightFile::write(const std::string& str)
{
    writeString(str.data(), str.size());
    return *this;
}


Foam::Ostream& Foam::ensightFile::write
(
    const char* buf,
    std::streamsize count
)
{
    stdStream().write(buf, count);
    syncState();
    return *this;
}


Foam::Ostream& Foam::ensightFile::write(const int32_t val)
{
    if (format() == IOstreamOption::BINARY)
    {
        write
        (
            reinterpret_cast<const char *>(&val),
            sizeof(int32_t)
        );
    }
    else
    {
        stdStream().width(10);
        stdStream() << val;
        syncState();
    }

    return *this;
}


Foam::Ostream& Foam::ensightFile::write(const int64_t val)
{
    int32_t ivalue(narrowInt32(val));

    return write(ivalue);
}


Foam::Ostream& Foam::ensightFile::write(const float val)
{
    if (format() == IOstreamOption::BINARY)
    {
        write
        (
            reinterpret_cast<const char *>(&val),
            sizeof(float)
        );
    }
    else
    {
        stdStream().width(12);
        stdStream() << val;
        syncState();
    }

    return *this;
}


Foam::Ostream& Foam::ensightFile::write(const double val)
{
    float fvalue(narrowFloat(val));

    return write(fvalue);
}


Foam::Ostream& Foam::ensightFile::write
(
    const label value,
    const label fieldWidth
)
{
    if (format() == IOstreamOption::BINARY)
    {
        write(value);
    }
    else
    {
        stdStream().width(fieldWidth);
        stdStream() << value;
        syncState();
    }

    return *this;
}


void Foam::ensightFile::newline()
{
    if (format() == IOstreamOption::ASCII)
    {
        stdStream() << nl;
        syncState();
    }
}


void Foam::ensightFile::writeUndef()
{
    write(undefValue_);
}


Foam::Ostream& Foam::ensightFile::writeKeyword(const keyType& key)
{
    if (allowUndef_)
    {
        writeString(key + " undef");
        newline();
        write(undefValue_);
        newline();
    }
    else
    {
        writeString(key);
        newline();
    }

    return *this;
}


void Foam::ensightFile::writeBinaryHeader()
{
    if (format() == IOstreamOption::BINARY)
    {
        writeString("C Binary");
        // Is binary: newline() is a no-op
    }
}


void Foam::ensightFile::beginTimeStep()
{
    writeString("BEGIN TIME STEP");
    newline();
}


void Foam::ensightFile::endTimeStep()
{
    writeString("END TIME STEP");
    newline();
}


//
// Convenience Output Methods
//

void Foam::ensightFile::beginPart(const label index)
{
    writeString("part");
    newline();
    write(index+1); // Ensight starts with 1
    newline();
}


void Foam::ensightFile::beginParticleCoordinates(const label nparticles)
{
    writeString("particle coordinates");
    newline();
    write(nparticles, 8); // unusual width
    newline();
}


void Foam::ensightFile::writeLabels(const UList<label>& list)
{
    for (const label val : list)
    {
        write(val);
        newline();
    }
}


void Foam::ensightFile::writeList(const UList<label>& field)
{
    for (const label val : field)
    {
        write(float(val));
        newline();
    }
}


void Foam::ensightFile::writeList(const UList<float>& field)
{
    for (const float val : field)
    {
        if (std::isnan(val))
        {
            writeUndef();
        }
        else
        {
            write(val);
        }
        newline();
    }
}


void Foam::ensightFile::writeList(const UList<double>& field)
{
    for (const double val : field)
    {
        if (std::isnan(val))
        {
            writeUndef();
        }
        else
        {
            write(val);
        }
        newline();
    }
}


// ************************************************************************* //
