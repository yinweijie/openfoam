/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
    Copyright (C) 2018-2020 OpenCFD Ltd.
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

#include "containers/Lists/ListOps/ListOps.H"
#include "containers/HashTables/HashSet/HashSet.H"
#include <numeric>

// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

Foam::labelList Foam::invert
(
    const label len,
    const labelUList& map
)
{
    labelList inverse(len, -1);

    label i = 0;
    for (const label newIdx : map)
    {
        if (newIdx >= 0)
        {
            #ifdef FULLDEBUG
            if (newIdx >= len)
            {
                FatalErrorInFunction
                    << "Inverse location " << newIdx
                    << " is out of range. List has size " << len
                    << abort(FatalError);
            }
            #endif

            if (inverse[newIdx] >= 0)
            {
                FatalErrorInFunction
                    << "Map is not one-to-one. At index " << i
                    << " element " << newIdx << " has already occurred\n"
                    << "Please use invertOneToMany instead"
                    << abort(FatalError);
            }

            inverse[newIdx] = i;
        }

        ++i;
    }

    return inverse;
}


Foam::labelList Foam::invert
(
    const label len,
    const bitSet& map
)
{
    labelList inverse(len, -1);

    label i = 0;
    for (const label newIdx : map)
    {
        #ifdef FULLDEBUG
        if (newIdx >= len)
        {
            FatalErrorInFunction
                << "Inverse location " << newIdx
                << " is out of range. List has size " << len
                << abort(FatalError);
        }
        #endif

        inverse[newIdx] = i;

        ++i;
    }

    return inverse;
}


Foam::labelList Foam::invert(const bitSet& map)
{
    return invert(map.size(), map);
}


Foam::labelListList Foam::invertOneToMany
(
    const label len,
    const labelUList& map
)
{
    labelList sizes(len, Zero);

    for (const label newIdx : map)
    {
        if (newIdx >= 0)
        {
            ++sizes[newIdx];
        }
    }

    labelListList inverse(len);

    for (label i=0; i < len; ++i)
    {
        inverse[i].resize(sizes[i]);
        sizes[i] = 0; // reset size counter
    }

    label i = 0;
    for (const label newIdx : map)
    {
        if (newIdx >= 0)
        {
            inverse[newIdx][sizes[newIdx]++] = i;
        }

        ++i;
    }

    return inverse;
}


Foam::bitSet Foam::reorder
(
    const labelUList& oldToNew,
    const bitSet& input,
    const bool prune
)
{
    const label len = input.size();

    bitSet output;
    output.reserve(len);

    for
    (
        label pos = input.find_first();
        pos >= 0 && pos < len;
        pos = input.find_next(pos)
    )
    {
        const label newIdx = oldToNew[pos];

        if (newIdx >= 0)
        {
            output.set(newIdx);
        }
        else if (!prune)
        {
            output.set(pos);
        }
    }

    if (prune)
    {
        output.trim();
    }

    return output;
}


void Foam::inplaceReorder
(
    const labelUList& oldToNew,
    bitSet& input,
    const bool prune
)
{
    input = reorder(oldToNew, input, prune);
}


void Foam::ListOps::unionEqOp::operator()
(
    labelList& x,
    const labelList& y
) const
{
    if (y.size())
    {
        if (x.size())
        {
            labelHashSet set(x);
            set.insert(y);
            x = set.toc();
        }
        else
        {
            x = y;
        }
    }
}


// ************************************************************************* //
