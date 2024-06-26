/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2016-2020 OpenCFD Ltd.
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

Application
    parallelTest

Description
    Test for various parallel routines.

\*---------------------------------------------------------------------------*/

#include "containers/Lists/List/List.H"
#include "meshes/polyMesh/mapPolyMesh/mapDistribute/mapDistribute.H"
#include "global/argList/argList.H"
#include "db/Time/TimeOpenFOAM.H"
#include "db/IOstreams/Pstreams/IPstream.H"
#include "db/IOstreams/Pstreams/OPstream.H"
#include "primitives/Vector/floats/vector.H"
#include "db/IOstreams/IOstreams.H"
#include "primitives/random/Random/Random.H"
#include "primitives/tuples/Tuple2.H"

using namespace Foam;


void testMapDistribute()
{
    Random rndGen(43544*Pstream::myProcNo());

    // Generate random data.
    List<Tuple2<label, List<scalar>>> complexData(100);
    forAll(complexData, i)
    {
        complexData[i].first() = rndGen.position(0, Pstream::nProcs()-1);
        complexData[i].second().setSize(3);
        complexData[i].second()[0] = 1;
        complexData[i].second()[1] = 2;
        complexData[i].second()[2] = 3;
    }

    // Send all ones to processor indicated by .first()

    // Count how many to send
    labelList nSend(Pstream::nProcs(), Zero);
    forAll(complexData, i)
    {
        const label proci = complexData[i].first();
        nSend[proci]++;
    }

    // Collect items to be sent
    labelListList sendMap(Pstream::nProcs());
    forAll(sendMap, proci)
    {
        sendMap[proci].resize_nocopy(nSend[proci]);
        nSend[proci] = 0;
    }
    forAll(complexData, i)
    {
        const label proci = complexData[i].first();
        sendMap[proci][nSend[proci]++] = i;
    }

    mapDistribute map(std::move(sendMap));

    // Distribute complexData
    map.distribute(complexData);

    Pout<< "complexData:" << complexData << endl;
}


// Print to Perr
template<class T>
Ostream& perrInfo(const T& data)
{
    Perr<< data;
    return Perr;
}


// Print to Perr
template<>
Ostream& perrInfo(const string& data)
{
    Perr<< data << " (size: " << data.size() << ")";
    return Perr;
}


template<class T>
void testTransfer(const T& input)
{
    T data = input;

    if (Pstream::master())
    {
        Perr<<"test transfer (" << (typeid(T).name()) << "): ";
        perrInfo(data) << nl << endl;
    }

    if (Pstream::master())
    {
        for (const int slave : Pstream::subProcs())
        {
            Perr<< "master receiving from slave " << slave << endl;
            IPstream fromSlave(Pstream::commsTypes::blocking, slave);
            fromSlave >> data;
            perrInfo(data) << endl;
        }

        for (const int slave : Pstream::subProcs())
        {
            Perr<< "master sending to slave " << slave << endl;
            OPstream toSlave(Pstream::commsTypes::blocking, slave);
            toSlave << data;
        }
    }
    else
    {
        {
            Perr<< "slave sending to master " << Pstream::masterNo() << endl;
            OPstream toMaster(Pstream::commsTypes::blocking, Pstream::masterNo());
            toMaster << data;
        }

        Perr<< "slave receiving from master " << Pstream::masterNo() << endl;
        IPstream fromMaster(Pstream::commsTypes::blocking, Pstream::masterNo());
        fromMaster >> data;
        perrInfo(data) << endl;
    }
}


template<class T>
void testTokenized(const T& data)
{
    token tok;

    if (Pstream::master())
    {
        Perr<<"test tokenized \"" << data << "\"" << nl << endl;
    }

    if (Pstream::master())
    {
        for (const int slave : Pstream::subProcs())
        {
            Perr<< "master receiving from slave " << slave << endl;
            IPstream fromSlave(Pstream::commsTypes::blocking, slave);
            fromSlave >> tok;
            Perr<< tok.info() << endl;
        }

        for (const int slave : Pstream::subProcs())
        {
            Perr<< "master sending to slave " << slave << endl;
            OPstream toSlave(Pstream::commsTypes::blocking, slave);
            toSlave << data;
        }
    }
    else
    {
        {
            Perr<< "slave sending to master " << Pstream::masterNo() << endl;
            OPstream toMaster
            (
                Pstream::commsTypes::blocking,
                Pstream::masterNo()
            );

            toMaster << data;
        }

        Perr<< "slave receiving from master " << Pstream::masterNo() << endl;
        IPstream fromMaster
        (
            Pstream::commsTypes::blocking,
            Pstream::masterNo()
        );

        fromMaster >> tok;
        Perr<< tok.info() << endl;
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "include/setRootCase.H"
    #include "include/createTime.H"

    testMapDistribute();

    if (!Pstream::parRun())
    {
        Info<< "\nWarning: not parallel - skipping further tests\n" << endl;
        return 0;
    }

    Info<< "\nStarting transfers\n\n" << endl;

    testTransfer(vector(0, 1, 2));
    testTransfer(label(1234));
    testTransfer(scalar(3.14159));
    testTransfer(string("test   string"));
    testTransfer(string("  x "));

    {
        // Slightly roundabout way to construct with a nul in string
        string str1("embedded. nul character in string");
        str1[8] = '\0';

        Info<< "len: " << str1.size() << endl;
        testTransfer(str1);
    }
    testTransfer(word("3.141 59"));  // bad word, but transfer doesn't care

    testTokenized(label(1234));
    testTokenized(scalar(3.14159));
    testTokenized('a');
    testTokenized('$');  // will not tokenize well

    testTokenized(string("test   string1"));
    testTokenized("test   string1");
    testTokenized(word("3.141 59"));  // bad word, but transfer doesn't care

    testTokenized(string("  a "));
    testTokenized("  a ");

    testTokenized(string("  $ "));
    testTokenized("  $ ");  // reduces to 'char' and will not tokenize well

    testTokenized(string("  $$ "));
    testTokenized("  $$ "); // reduces to 'word' and is tagged as such


    Info<< "End\n" << endl;
    return 0;
}


// ************************************************************************* //
