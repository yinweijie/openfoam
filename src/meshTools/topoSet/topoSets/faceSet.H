/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
    Copyright (C) 2016-2022 OpenCFD Ltd.
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
    Foam::faceSet

Description
    A list of face labels.

SourceFiles
    faceSet.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_faceSet_H
#define Foam_faceSet_H

#include "topoSet/topoSets/topoSet.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class faceSet Declaration
\*---------------------------------------------------------------------------*/

class faceSet
:
    public topoSet
{
public:

    //- Runtime type information
    TypeName("faceSet");


    // Constructors

        //- Construct from IOobject
        explicit faceSet(const IOobject& io);

        //- Construct from objectRegistry and name
        faceSet
        (
            const polyMesh& mesh,
            const word& name,
            IOobjectOption::readOption rOpt = IOobjectOption::MUST_READ,
            IOobjectOption::writeOption wOpt = IOobjectOption::NO_WRITE
        );

        //- Construct empty with initial size for labelHashSet
        faceSet
        (
            const polyMesh& mesh,
            const word& name,
            const label size,
            IOobjectOption::writeOption wOpt = IOobjectOption::NO_WRITE
        );

        //- Construct from existing set
        faceSet
        (
            const polyMesh& mesh,
            const word& name,
            const topoSet& set,
            IOobjectOption::writeOption wOpt = IOobjectOption::NO_WRITE
        );

        //- Construct (no-read) with copy of labelHashSet
        faceSet
        (
            const polyMesh& mesh,
            const word& name,
            const labelHashSet& labels,
            IOobjectOption::writeOption wOpt = IOobjectOption::NO_WRITE
        );

        //- Construct (no-read) with moving labelHashSet
        faceSet
        (
            const polyMesh& mesh,
            const word& name,
            labelHashSet&& labels,
            IOobjectOption::writeOption wOpt = IOobjectOption::NO_WRITE
        );

        //- Construct (no-read) with copy of labels
        faceSet
        (
            const polyMesh& mesh,
            const word& name,
            const labelUList& labels,
            IOobjectOption::writeOption wOpt = IOobjectOption::NO_WRITE
        );


    //- Destructor
    virtual ~faceSet() = default;


    // Member functions

        //- Sync faceSet across coupled patches.
        virtual void sync(const polyMesh& mesh);

        //- Return max index+1.
        virtual label maxSize(const polyMesh& mesh) const;

        //- Update any stored data for new labels
        virtual void updateMesh(const mapPolyMesh& morphMap);

        //- Update any stored data for mesh redistribution.
        virtual void distribute(const mapDistributePolyMesh& map);

        //- Write maxLen items with label and coordinates.
        virtual void writeDebug
        (
            Ostream& os,
            const primitiveMesh&,
            const label maxLen
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
