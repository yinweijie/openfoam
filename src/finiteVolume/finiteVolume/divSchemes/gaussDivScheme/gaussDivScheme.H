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

Class
    Foam::fv::gaussDivScheme

Group
    grpFvDivSchemes

Description
    Basic second-order div using face-gradients and Gauss' theorem.

SourceFiles
    gaussDivScheme.C

\*---------------------------------------------------------------------------*/

#ifndef gaussDivScheme_H
#define gaussDivScheme_H

#include "finiteVolume/divSchemes/divScheme/divScheme.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace fv
{

/*---------------------------------------------------------------------------*\
                       Class gaussDivScheme Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class gaussDivScheme
:
    public fv::divScheme<Type>
{
    // Private Member Functions

        //- No copy construct
        gaussDivScheme(const gaussDivScheme&) = delete;

        //- No copy assignment
        void operator=(const gaussDivScheme&) = delete;


public:

    //- Runtime type information
    TypeName("Gauss");


    // Constructors

        //- Construct null
        gaussDivScheme(const fvMesh& mesh)
        :
            divScheme<Type>(mesh)
        {}

        //- Construct from Istream
        gaussDivScheme(const fvMesh& mesh, Istream& is)
        :
            divScheme<Type>(mesh, is)
        {}


    // Member Functions

        tmp
        <
            GeometricField
            <typename innerProduct<vector, Type>::type, fvPatchField, volMesh>
        > fvcDiv
        (
            const GeometricField<Type, fvPatchField, volMesh>&
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "finiteVolume/divSchemes/gaussDivScheme/gaussDivScheme.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
