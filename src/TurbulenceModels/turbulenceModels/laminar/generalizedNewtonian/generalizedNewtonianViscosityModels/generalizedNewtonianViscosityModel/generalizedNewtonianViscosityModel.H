/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018-2020 OpenFOAM Foundation
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

Namespace
    Foam::laminarModels::generalizedNewtonianViscosityModels

Description
    A namespace for various generalized Newtonian viscosity model
    implementations.

Class
    Foam::laminarModels::generalizedNewtonianViscosityModel

Description
    An abstract base class for generalized Newtonian viscosity models.

SourceFiles
    generalizedNewtonianViscosityModel.C
    generalizedNewtonianViscosityModelNew.C

\*---------------------------------------------------------------------------*/

#ifndef generalizedNewtonianViscosityModel_H
#define generalizedNewtonianViscosityModel_H

#include "db/dictionary/dictionary.H"
#include "fields/volFields/volFieldsFwd.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace laminarModels
{

/*---------------------------------------------------------------------------*\
                Class generalizedNewtonianViscosityModel Declaration
\*---------------------------------------------------------------------------*/

class generalizedNewtonianViscosityModel
{
protected:

    // Protected Data

        dictionary viscosityProperties_;


public:

    //- Runtime type information
    TypeName("generalizedNewtonianViscosityModel");


    // Declare run-time constructor selection table

        declareRunTimeSelectionTable
        (
            autoPtr,
            generalizedNewtonianViscosityModel,
            dictionary,
            (
                const dictionary& viscosityProperties
            ),
            (viscosityProperties)
        );


    // Generated Methods

        //- No copy construct
        generalizedNewtonianViscosityModel
        (
            const generalizedNewtonianViscosityModel&
        ) = delete;

        //- No copy assignment
        void operator=(const generalizedNewtonianViscosityModel&) = delete;


    // Selectors

        //- Select a viscosity model
        static autoPtr<generalizedNewtonianViscosityModel> New
        (
            const dictionary& dict
        );


    // Constructors

        //- Construct from dictionary (components)
        explicit generalizedNewtonianViscosityModel
        (
            const dictionary& viscosityProperties
        );


    //- Destructor
    virtual ~generalizedNewtonianViscosityModel() = default;


    // Member Functions

        //- Return the phase transport properties dictionary
        const dictionary& viscosityProperties() const
        {
            return viscosityProperties_;
        }

        //- Return the laminar viscosity
        virtual tmp<volScalarField> nu
        (
            const volScalarField& nu0,
            const volScalarField& strainRate
        ) const = 0;

        //- Read transportProperties dictionary
        virtual bool read(const dictionary& viscosityProperties) = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace laminarModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
