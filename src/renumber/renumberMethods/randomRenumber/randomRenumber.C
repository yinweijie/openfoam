/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2012 OpenFOAM Foundation
    Copyright (C) 2021-2022 OpenCFD Ltd.
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

#include "randomRenumber/randomRenumber.H"
#include "primitives/random/Random/Random.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(randomRenumber, 0);

    addToRunTimeSelectionTable
    (
        renumberMethod,
        randomRenumber,
        dictionary
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::randomRenumber::randomRenumber(const dictionary& dict)
:
    renumberMethod(dict)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::labelList Foam::randomRenumber::renumber
(
    const pointField& points
) const
{
    Random rndGen(0);

    labelList newToOld(identity(points.size()));

    for (label iter = 0; iter < 10; iter++)
    {
        forAll(newToOld, i)
        {
            label j = rndGen.position<label>(0, newToOld.size()-1);
            std::swap(newToOld[i], newToOld[j]);
        }
    }
    return newToOld;
}


Foam::labelList Foam::randomRenumber::renumber
(
    const polyMesh& mesh,
    const pointField& points
) const
{
    return renumber(points);
}


Foam::labelList Foam::randomRenumber::renumber
(
    const CompactListList<label>& cellCells,
    const pointField& points
) const
{
    return renumber(points);
}


Foam::labelList Foam::randomRenumber::renumber
(
    const labelListList& cellCells,
    const pointField& points
) const
{
    return renumber(points);
}


// ************************************************************************* //
