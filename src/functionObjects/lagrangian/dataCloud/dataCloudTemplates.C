/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018-2022 OpenCFD Ltd.
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

#include "fields/cloud/cloud.H"
#include "db/IOobjects/IOField/IOField.H"
#include "db/IOstreams/Fstreams/OFstream.H"
#include "containers/Lists/ListOps/ListOps.H"
#include "meshes/primitiveShapes/point/pointField.H"
#include "fields/Fields/vectorField/vectorField.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class Type>
void Foam::functionObjects::dataCloud::writePointValue
(
    Ostream& os,
    const vector& pt,
    const Type& val
)
{
    os << pt.x() << ' ' << pt.y() << ' ' << pt.z();

    for (direction cmpt=0; cmpt < pTraits<Type>::nComponents; ++cmpt)
    {
        os << ' ' << component(val, cmpt);
    }
    os << nl;
}


template<class Type>
void Foam::functionObjects::dataCloud::writeList
(
    Ostream& os,
    const vectorField& points,
    const List<Type>& field
)
{
    const label len = field.size();

    for (label pointi=0; pointi<len; ++pointi)
    {
        writePointValue(os, points[pointi], field[pointi]);
    }
}


template<class Type>
void Foam::functionObjects::dataCloud::writeListParallel
(
    Ostream& os,
    const vectorField& points,
    const List<Type>& field
)
{
    if (Pstream::master())
    {
        writeList(os, points, field);

        vectorField recvPoints;
        Field<Type> recvField;

        // Receive and write
        for (const int proci : Pstream::subProcs())
        {
            IPstream fromProc(Pstream::commsTypes::blocking, proci);

            fromProc >> recvPoints >> recvField;

            writeList(os, recvPoints, recvField);
        }
    }
    else
    {
        // Send to master
        OPstream toMaster
        (
            Pstream::commsTypes::blocking,
            Pstream::masterNo()
        );

        toMaster
            << points << field;
    }
}


template<class Type>
void Foam::functionObjects::dataCloud::writeList
(
    Ostream& os,
    const vectorField& points,
    const List<Type>& field,
    const bitSet& selected
)
{
    for (const label pointi : selected)
    {
        writePointValue(os, points[pointi], field[pointi]);
    }
}


template<class Type>
void Foam::functionObjects::dataCloud::writeListParallel
(
    Ostream& os,
    const vectorField& points,
    const List<Type>& field,
    const bitSet& selected
)
{
    if (Pstream::master())
    {
        writeList(os, points, field, selected);

        vectorField recvPoints;
        Field<Type> recvField;

        // Receive and write
        for (const int proci : Pstream::subProcs())
        {
            IPstream fromProc(Pstream::commsTypes::blocking, proci);

            fromProc >> recvPoints >> recvField;

            writeList(os, recvPoints, recvField);
        }
    }
    else
    {
        // Send to master
        OPstream toMaster
        (
            Pstream::commsTypes::blocking,
            Pstream::masterNo()
        );

        toMaster
            << subset(selected, points)
            << subset(selected, field);
    }
}


template<class Type>
bool Foam::functionObjects::dataCloud::writeField
(
    const fileName& outputName,
    const objectRegistry& obrTmp
) const
{
    const auto* pointsPtr = cloud::findIOPosition(obrTmp);

    if (!pointsPtr)
    {
        // This should be impossible
        return false;
    }

    // Fields are not always on all processors (eg, multi-component parcels).
    // Thus need to resolve between all processors.

    const List<Type>* fldPtr = obrTmp.findObject<IOField<Type>>(fieldName_);
    const List<Type>& values = (fldPtr ? *fldPtr : List<Type>());

    if (!returnReduceOr(fldPtr != nullptr))
    {
        return false;
    }

    autoPtr<OFstream> osPtr;

    if (Pstream::master())
    {
        osPtr.reset(new OFstream(outputName));
        osPtr->precision(precision_);

        *(osPtr) << "# x y z " << fieldName_ << nl;
    }


    if (applyFilter_)
    {
        writeListParallel(osPtr.ref(), *pointsPtr, values, parcelAddr_);
    }
    else
    {
        writeListParallel(osPtr.ref(), *pointsPtr, values);
    }

    return true;
}


// ************************************************************************* //
