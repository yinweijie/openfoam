/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012 OpenFOAM Foundation
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
    Foam::IOporosityModelList

Description
    List of porosity models with IO functionality

SourceFiles
    IOporosityModelList.C

\*---------------------------------------------------------------------------*/

#ifndef IOporosityModelList_H
#define IOporosityModelList_H

#include "db/IOobjects/IOdictionary/IOdictionary.H"
#include "cfdTools/general/porosityModel/porosityModel/porosityModelList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Class IOporosityModelList Declaration
\*---------------------------------------------------------------------------*/

class IOporosityModelList
:
    public IOdictionary,
    public porosityModelList
{
private:

    // Private Member Functions

        //- Create IO object if dictionary is present
        IOobject createIOobject(const fvMesh& mesh) const;

        //- No copy construct
        IOporosityModelList(const IOporosityModelList&) = delete;

        //- No copy assignment
        void operator=(const IOporosityModelList&) = delete;


public:

    // Constructors

        //- Construct from mesh
        explicit IOporosityModelList(const fvMesh& mesh);


        //- Destructor
        virtual ~IOporosityModelList() = default;


    // Member Functions

        //- Read dictionary
        virtual bool read();
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
