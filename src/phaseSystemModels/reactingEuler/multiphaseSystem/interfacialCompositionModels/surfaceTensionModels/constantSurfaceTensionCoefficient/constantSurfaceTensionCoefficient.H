/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2018 OpenFOAM Foundation
    Copyright (C) 2022 OpenCFD Ltd.
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
    Foam::surfaceTensionModels::constantSurfaceTensionCoefficient

Description
    Constant value surface tension model.

SourceFiles
    constantSurfaceTensionCoefficient.C

\*---------------------------------------------------------------------------*/

#ifndef constantSurfaceTensionCoefficient_H
#define constantSurfaceTensionCoefficient_H

#include "interfacialCompositionModels/surfaceTensionModels/surfaceTensionModel/surfaceTensionModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace reactingMultiphaseEuler
{
namespace surfaceTensionModels
{

/*---------------------------------------------------------------------------*\
              Class constantSurfaceTensionCoefficient Declaration
\*---------------------------------------------------------------------------*/

class constantSurfaceTensionCoefficient
:
    public surfaceTensionModel
{
    // Private data

        //- Constant surface tension value
        const dimensionedScalar sigma_;


public:

    //- Runtime type information
    TypeName("constant");


    // Constructors

        //- Construct from a dictionary and a phase pair
        constantSurfaceTensionCoefficient
        (
            const dictionary& dict,
            const phasePair& pair,
            const bool registerObject
        );


    //- Destructor
    virtual ~constantSurfaceTensionCoefficient();


    // Member Functions

        //- Aspect ratio
        virtual tmp<volScalarField> sigma() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceTensionModels
} // End namespace reactingMultiphaseEuler
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
