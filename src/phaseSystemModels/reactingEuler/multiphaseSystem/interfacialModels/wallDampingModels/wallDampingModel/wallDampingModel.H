/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2018 OpenFOAM Foundation
    Copyright (C) 2020 OpenCFD Ltd.
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
    Foam::wallDampingModel

Description

SourceFiles
    wallDampingModel.C

\*---------------------------------------------------------------------------*/

#ifndef wallDampingModel_H
#define wallDampingModel_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "interfacialModels/wallDependentModel/wallDependentModel.H"
#include "fields/volFields/volFields.H"
#include "db/dictionary/dictionary.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"

namespace Foam
{

// Forward Declarations
class phasePair;

/*---------------------------------------------------------------------------*\
                      Class wallDampingModel Declaration
\*---------------------------------------------------------------------------*/

class wallDampingModel
:
    public wallDependentModel
{
protected:

    // Protected Data

        //- Phase pair
        const phasePair& pair_;


public:

    //- Runtime type information
    TypeName("wallDampingModel");


    // Declare runtime construction

        declareRunTimeSelectionTable
        (
            autoPtr,
            wallDampingModel,
            dictionary,
            (
                const dictionary& dict,
                const phasePair& pair
            ),
            (dict, pair)
        );


    // Static Data Members

        //- Coefficient dimensions
        static const dimensionSet dimF;


    // Constructors

        //- Construct from components
        wallDampingModel
        (
            const dictionary& dict,
            const phasePair& pair
        );


    //- Destructor
    virtual ~wallDampingModel() = default;


    // Selectors

        static autoPtr<wallDampingModel> New
        (
            const dictionary& dict,
            const phasePair& pair
        );


    // Member Functions

        //- Return damped coefficient
        virtual tmp<volScalarField> damp
        (
            const tmp<volScalarField>&
        ) const = 0;

        //- Return damped force
        virtual tmp<volVectorField> damp
        (
            const tmp<volVectorField>&
        ) const = 0;

        //- Return damped face force
        virtual tmp<surfaceScalarField> damp
        (
            const tmp<surfaceScalarField>&
        ) const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
