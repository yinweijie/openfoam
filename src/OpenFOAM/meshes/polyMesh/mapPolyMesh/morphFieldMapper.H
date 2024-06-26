/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
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
    Foam::morphFieldMapper

Description
    Abstract base class to hold the Field mapping for mesh morphs.

\*---------------------------------------------------------------------------*/

#ifndef Foam_morphFieldMapper_H
#define Foam_morphFieldMapper_H

#include "fields/Fields/Field/Field.H"
#include "containers/HashTables/Map/Map.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class morphFieldMapper Declaration
\*---------------------------------------------------------------------------*/

class morphFieldMapper
:
    public FieldMapper
{
public:

    // Constructors

        //- Default construct
        morphFieldMapper() = default;


    //- Destructor
    virtual ~morphFieldMapper() = default;


    // Member Functions

        //- Return size of field before mapping
        virtual label sizeBeforeMapping() const = 0;

        //- Are there any inserted objects
        virtual bool insertedObjects() const = 0;

        //- Return list of inserted objects
        virtual const labelList& insertedObjectLabels() const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
