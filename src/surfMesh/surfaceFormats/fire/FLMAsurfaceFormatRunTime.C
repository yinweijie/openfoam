/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenCFD Ltd.
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

#include "surfaceFormats/fire/FLMAsurfaceFormat.H"

#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "db/runTimeSelection/memberFunctions/addToMemberFunctionSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace fileFormats
{

// write MeshedSurfaceProxy
addNamedTemplatedToMemberFunctionSelectionTable
(
    MeshedSurfaceProxy,
    FLMAsurfaceFormat,
    face,
    write,
    fileExtension,
    flma
);
addNamedTemplatedToMemberFunctionSelectionTable
(
    MeshedSurfaceProxy,
    FLMAsurfaceFormat,
    triFace,
    write,
    fileExtension,
    flma
);
addNamedTemplatedToMemberFunctionSelectionTable
(
    MeshedSurfaceProxy,
    FLMAsurfaceFormat,
    labelledTri,
    write,
    fileExtension,
    flma
);


// write MeshedSurfaceProxy (comnpressed versions of above)
addNamedTemplatedToMemberFunctionSelectionTable
(
    MeshedSurfaceProxy,
    FLMAZsurfaceFormat,
    face,
    write,
    fileExtension,
    flmaz
);
addNamedTemplatedToMemberFunctionSelectionTable
(
    MeshedSurfaceProxy,
    FLMAZsurfaceFormat,
    triFace,
    write,
    fileExtension,
    flmaz
);
addNamedTemplatedToMemberFunctionSelectionTable
(
    MeshedSurfaceProxy,
    FLMAZsurfaceFormat,
    labelledTri,
    write,
    fileExtension,
    flmaz
);

}
}

// ************************************************************************* //
