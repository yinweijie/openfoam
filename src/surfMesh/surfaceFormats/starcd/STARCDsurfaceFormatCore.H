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
    Foam::fileFormats::STARCDsurfaceFormatCore

Description
    Internal class used by the STARCDsurfaceFormat

SourceFiles
    STARCDsurfaceFormatCore.C

\*---------------------------------------------------------------------------*/

#ifndef STARCDsurfaceFormatCore_H
#define STARCDsurfaceFormatCore_H

#include "db/IOstreams/Fstreams/Fstream.H"
#include "db/IOstreams/IOstreams/Ostream.H"
#include "MeshedSurface/MeshedSurface.H"
#include "starcd/STARCDCore.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fileFormats
{

/*---------------------------------------------------------------------------*\
            Class fileFormats::STARCDsurfaceFormatCore Declaration
\*---------------------------------------------------------------------------*/

class STARCDsurfaceFormatCore
:
    public STARCDCore
{
protected:

    // Protected Static Member Functions

    static Map<word> readInpCellTable(ISstream& is);

    static void writeCase
    (
        Ostream& os,
        const UList<point>& pts,
        const label nFaces,
        const UList<surfZone>& zoneLst
    );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fileFormats
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
