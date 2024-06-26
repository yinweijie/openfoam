/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
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
    Foam::pointPatchFieldMapperPatchRef

Description
    Foam::pointPatchFieldMapperPatchRef

\*---------------------------------------------------------------------------*/

#ifndef pointPatchFieldMapperPatchRef_H
#define pointPatchFieldMapperPatchRef_H

#include "fields/pointPatchFields/pointPatchField/pointPatchFieldMapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class pointPatchFieldMapperPatchRef Declaration
\*---------------------------------------------------------------------------*/

class pointPatchFieldMapperPatchRef
:
    public pointPatchFieldMapper
{
    // Private data

        const pointPatch& sourcePatch_;
        const pointPatch& targetPatch_;


    // Private Member Functions

        //- No copy construct
        pointPatchFieldMapperPatchRef
        (
            const pointPatchFieldMapperPatchRef&
        ) = delete;

        //- No copy assignment
        void operator=(const pointPatchFieldMapperPatchRef&) = delete;


public:

    // Constructors

        //- Construct from components
        pointPatchFieldMapperPatchRef
        (
            const pointPatch& source,
            const pointPatch& target
        )
        :
            sourcePatch_(source),
            targetPatch_(target)
        {}


    // Member functions

        const pointPatch& sourcePatch() const
        {
            return sourcePatch_;
        }

        const pointPatch& targetPatch() const
        {
            return targetPatch_;
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
