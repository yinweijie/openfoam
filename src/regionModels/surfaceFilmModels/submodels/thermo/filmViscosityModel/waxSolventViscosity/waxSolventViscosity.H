/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2017 OpenFOAM Foundation
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
    Foam::regionModels::surfaceFilmModels::waxSolventViscosity

Description
    Wax solvent mixture viscosity model.

SourceFiles
    waxSolventViscosity.C

\*---------------------------------------------------------------------------*/

#ifndef waxSolventViscosity_H
#define waxSolventViscosity_H

#include "submodels/thermo/filmViscosityModel/filmViscosityModel/filmViscosityModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{
namespace surfaceFilmModels
{

/*---------------------------------------------------------------------------*\
                    Class waxSolventViscosity Declaration
\*---------------------------------------------------------------------------*/

class waxSolventViscosity
:
    public filmViscosityModel
{
    // Private member functions

        //- Correct the mixture viscosity
        void correctMu();

        //- No copy construct
        waxSolventViscosity(const waxSolventViscosity&) = delete;

        //- No copy assignment
        void operator=(const waxSolventViscosity&) = delete;


protected:

    // Protected data

        //- Wax viscosity
        volScalarField muWax_;

        //- Wax viscosity model
        autoPtr<filmViscosityModel> muWaxModel_;

        //- Solvent viscosity
        volScalarField muSolvent_;

        //- Solvent viscosity model
        autoPtr<filmViscosityModel> muSolventModel_;


public:

    //- Runtime type information
    TypeName("waxSolvent");


    // Constructors

        //- Construct from surface film model
        waxSolventViscosity
        (
            surfaceFilmRegionModel& film,
            const dictionary& dict,
            volScalarField& mu
        );


    //- Destructor
    virtual ~waxSolventViscosity() = default;


    // Member Functions

        //- Correct
        virtual void correct
        (
            const volScalarField& p,
            const volScalarField& T
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceFilmModels
} // End namespace regionModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
