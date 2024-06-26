/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
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

#include "blockVertices/namedVertex/namedVertex.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace blockVertices
{
    defineTypeNameAndDebug(namedVertex, 0);
    addToRunTimeSelectionTable(blockVertex, namedVertex, Istream);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::blockVertices::namedVertex::namedVertex
(
    const dictionary& dict,
    const label index,
    const searchableSurfaces& geometry,
    Istream& is
)
:
    name_(is),
    vertexPtr_(blockVertex::New(dict, index, geometry, is))
{
    dictionary& d = const_cast<dictionary&>(dict);

    dictionary* varDictPtr = d.findDict("namedVertices");
    if (varDictPtr)
    {
        varDictPtr->add(name_, index);
    }
    else
    {
        dictionary varDict;
        varDict.add(name_, index);
        d.add("namedVertices", varDict);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::blockVertices::namedVertex::operator point() const
{
    return vertexPtr_().operator point();
}


// ************************************************************************* //
