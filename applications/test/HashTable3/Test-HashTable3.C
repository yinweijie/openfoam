/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

Description
    Test speeds for some HashTable operations

\*---------------------------------------------------------------------------*/

#include "global/argList/argList.H"
#include "containers/HashTables/HashTable/HashTable.H"
#include "containers/HashTables/HashPtrTable/HashPtrTable.H"
#include "containers/HashTables/Map/Map.H"
#include "cpuTime/cpuTime.H"

using namespace Foam;

template<class T>
Ostream& printInfo(Ostream& os, const HashTable<T, T, Hash<T>>& ht)
{
    os << " (size " << ht.size() << " capacity " << ht.capacity() << ") ";
    return os;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{
    const label nLoops = 300;
    const label nBase  = 100000;
    const label nSize  = nLoops * nBase;

    cpuTime timer;

    // ie, a
    // Map<label> map(2 * nSize);
    // HashTable<label, label, Hash<label>> map(2 * nSize);
    HashTable<label, label, Hash<label>> map(2 * nSize);

    Info<< "Constructed map of size: " << nSize;
    printInfo(Info, map);
    Info<< timer.cpuTimeIncrement() << " s\n\n";

    for (label i = 0; i < nSize; i++)
    {
        map.insert(i, i);
    }

    Info<< "Inserted " << nSize << " elements";
    printInfo(Info, map);
    Info<< timer.cpuTimeIncrement() << " s\n\n";

    label elemI = 0;
    for (label iLoop = 0; iLoop < nLoops; iLoop++)
    {
        for (label i = 0; i < nBase; i++)
        {
            map.erase(elemI++);
        }

        // map.shrink();
        Info<< "loop " << iLoop << " - Erased " << nBase << " elements";
        printInfo(Info, map);
        Info << nl;
    }

    Info<< timer.cpuTimeIncrement() << " s\n";

    return 0;
}

// ************************************************************************* //
