/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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
    Foam::regionModels::surfaceFilmModels::constantHeatTransfer

Description
    Constant heat transfer model

SourceFiles
    constantHeatTransfer.C

\*---------------------------------------------------------------------------*/

#ifndef constantHeatTransfer_H
#define constantHeatTransfer_H

#include "submodels/thermo/heatTransferModel/heatTransferModel/heatTransferModel.H"
#include "fields/volFields/volFieldsFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{
namespace surfaceFilmModels
{

/*---------------------------------------------------------------------------*\
                        Class constantHeatTransfer Declaration
\*---------------------------------------------------------------------------*/

class constantHeatTransfer
:
    public heatTransferModel
{
private:

    // Private data

        //- Constant heat transfer coefficient [W/m2/K]
        scalar c0_;


    // Private member functions

        //- No copy construct
        constantHeatTransfer(const constantHeatTransfer&) = delete;

        //- No copy assignment
        void operator=(const constantHeatTransfer&) = delete;


public:

    //- Runtime type information
    TypeName("constant");


    // Constructors

        //- Construct from surface film model and dictionary
        constantHeatTransfer
        (
            surfaceFilmRegionModel& film,
            const dictionary& dict
        );


    //- Destructor
    virtual ~constantHeatTransfer();


    // Member Functions

        // Evolution

            //- Correct
            virtual void correct();

            //- Return the heat transfer coefficient [W/m2/K]
            virtual tmp<volScalarField> h() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceFilmModels
} // End namespace regionModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
