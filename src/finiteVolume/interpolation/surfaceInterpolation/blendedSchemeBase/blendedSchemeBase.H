/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013 OpenFOAM Foundation
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
    Foam::blendedSchemeBase

Description
    Base class for blended schemes to provide access to the blending factor
    surface field

\*---------------------------------------------------------------------------*/

#ifndef Foam_blendedSchemeBase_H
#define Foam_blendedSchemeBase_H

#include "db/typeInfo/className.H"
#include "memory/tmp/tmp.H"
#include "fields/surfaceFields/surfaceFieldsFwd.H"
#include "fields/volFields/volFieldsFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

TemplateName(blendedSchemeBase);

/*---------------------------------------------------------------------------*\
                      Class blendedSchemeBase Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class blendedSchemeBase
:
    public blendedSchemeBaseName
{
public:

    //- Constructor
    blendedSchemeBase() = default;

    //- Destructor
    virtual ~blendedSchemeBase() = default;


    // Member Functions

        //- Return the face-based blending factor
        virtual tmp<surfaceScalarField> blendingFactor
        (
             const GeometricField<Type, fvPatchField, volMesh>& vf
        ) const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
