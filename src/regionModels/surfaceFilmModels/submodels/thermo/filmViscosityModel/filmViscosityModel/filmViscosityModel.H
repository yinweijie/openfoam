/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2017 OpenFOAM Foundation
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
    Foam::regionModels::surfaceFilmModels::filmViscosityModel

Description
    Base class for surface film viscosity models

SourceFiles
    filmViscosityModel.C
    filmViscosityModelNew.C

\*---------------------------------------------------------------------------*/

#ifndef filmViscosityModel_H
#define filmViscosityModel_H

#include "submodels/filmSubModelBase.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"
#include "fields/Fields/scalarField/scalarField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{
namespace surfaceFilmModels
{

/*---------------------------------------------------------------------------*\
                     Class filmViscosityModel Declaration
\*---------------------------------------------------------------------------*/

class filmViscosityModel
:
    public filmSubModelBase
{
private:

    // Private Member Functions

        //- No copy construct
        filmViscosityModel(const filmViscosityModel&) = delete;

        //- No copy assignment
        void operator=(const filmViscosityModel&) = delete;


protected:

    // Protected Member Data

        //- Reference to the viscosity field
        volScalarField& mu_;


public:

    //- Runtime type information
    TypeName("filmViscosityModel");


    // Declare runtime constructor selection table

         declareRunTimeSelectionTable
         (
             autoPtr,
             filmViscosityModel,
             dictionary,
             (
                surfaceFilmRegionModel& film,
                const dictionary& dict,
                volScalarField& mu
             ),
             (film, dict, mu)
         );

    // Constructors

        //- Construct from type name, dictionary and surface film model
        filmViscosityModel
        (
            const word& modelType,
            surfaceFilmRegionModel& film,
            const dictionary& dict,
            volScalarField& mu
        );


    // Selectors

        //- Return a reference to the selected phase change model
        static autoPtr<filmViscosityModel> New
        (
            surfaceFilmRegionModel& film,
            const dictionary& dict,
            volScalarField& mu
        );


    //- Destructor
    virtual ~filmViscosityModel();


    // Member Functions

        // Evolution

            //- Correct
            virtual void correct
            (
                const volScalarField& p,
                const volScalarField& T
            ) = 0;


        // I-O

            //- Provide some feedback
            virtual void info(Ostream& os) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceFilmModels
} // End namespace regionModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
