/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2017 OpenFOAM Foundation
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
    Foam::regionModels::surfaceFilmModels::constantRadiation

Description
    Film constant radiation model.  The constant radiative flux is specified
    by the user, and operated over a time interval defined by a start time and
    duration.  In addition, a mask can be applied to shield the film from the
    radiation.

SourceFiles
    constantRadiation.C

\*---------------------------------------------------------------------------*/

#ifndef constantRadiation_H
#define constantRadiation_H

#include "submodels/thermo/filmRadiationModel/filmRadiationModel/filmRadiationModel.H"
#include "fields/volFields/volFieldsFwd.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{
namespace surfaceFilmModels
{

/*---------------------------------------------------------------------------*\
                      Class constantRadiation Declaration
\*---------------------------------------------------------------------------*/

class constantRadiation
:
    public filmRadiationModel
{
private:

    // Private data

        //- Constant radiative flux [kg/s3]
        volScalarField qrConst_;

        //- Radiation mask
        volScalarField mask_;

        //- Absorptivity
        scalar absorptivity_;

        //- Time start [s]
        const scalar timeStart_;

        //- Duration [s]
        const scalar duration_;


    // Private member functions

        //- No copy construct
        constantRadiation(const constantRadiation&) = delete;

        //- No copy assignment
        void operator=(const constantRadiation&) = delete;


public:

    //- Runtime type information
    TypeName("constantRadiation");


    // Constructors

        //- Construct from surface film model and dictionary
        constantRadiation
        (
            surfaceFilmRegionModel& film,
            const dictionary& dict
        );


    //- Destructor
    virtual ~constantRadiation();


    // Member Functions

        // Evolution

            //- Correct
            virtual void correct();

            //- Return the radiation sensible enthalpy source
            //  Also updates qrNet
            virtual tmp<volScalarField> Shs();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceFilmModels
} // End namespace regionModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
