/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2019 OpenFOAM Foundation
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
    Foam::diameterModel

Description
    Abstract base-class for dispersed-phase particle diameter models.

SourceFiles
    diameterModel.C

\*---------------------------------------------------------------------------*/

#ifndef diameterModel_H
#define diameterModel_H

#include "db/dictionary/dictionary.H"
#include "phaseModel/phaseModel/reactingEuler_phaseModel.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class diameterModel Declaration
\*---------------------------------------------------------------------------*/

class diameterModel
{
protected:

    // Protected Data

        dictionary diameterProperties_;

        const phaseModel& phase_;


public:

    //- Runtime type information
    TypeName("diameterModel");


    // Declare runtime construction

        declareRunTimeSelectionTable
        (
            autoPtr,
            diameterModel,
            dictionary,
            (
                const dictionary& dict,
                const phaseModel& phase
            ),
            (dict, phase)
        );


    // Constructors

        diameterModel
        (
            const dictionary& dict,
            const phaseModel& phase
        );


    //- Destructor
    virtual ~diameterModel() = default;


    // Selectors

        static autoPtr<diameterModel> New
        (
            const dictionary& diameterProperties,
            const phaseModel& phase
        );


    // Member Functions

        //- Return the phase diameter properties dictionary
        const dictionary& diameterProperties() const
        {
            return diameterProperties_;
        }

        //- Return the phase
        const phaseModel& phase() const
        {
            return phase_;
        }

        //- Return the phase mean diameter field
        virtual tmp<volScalarField> d() const = 0;

        //- Correct the diameter field
        virtual void correct();

        //- Read phaseProperties dictionary
        virtual bool read(const dictionary& phaseProperties) = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
