/*---------------------------------------------------------------------------* \
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
    Foam::regionModels::surfaceFilmModels::distributionContactAngleForce

Description
    PDF distribution based film contact angle force

See also
    Foam::regionModels::surfaceFilmModels::contactAngleForce
    Foam::distributionModel

SourceFiles
    distributionContactAngleForce.C

\*---------------------------------------------------------------------------*/

#ifndef distributionContactAngleForce_H
#define distributionContactAngleForce_H

#include "submodels/kinematic/force/contactAngleForces/contactAngleForce/contactAngleForce.H"
#include "distributionModel/distributionModel.H"
#include "primitives/random/Random/Random.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{
namespace surfaceFilmModels
{

/*---------------------------------------------------------------------------*\
                Class distributionContactAngleForce Declaration
\*---------------------------------------------------------------------------*/

class distributionContactAngleForce
:
    public contactAngleForce
{
    // Private Data

        //- Random number generator
        Random rndGen_;

        //- Parcel size PDF model
        const autoPtr<distributionModel> distribution_;


    // Private member functions

        //- No copy construct
        distributionContactAngleForce
        (
            const distributionContactAngleForce&
        ) = delete;

        //- No copy assignment
        void operator=(const distributionContactAngleForce&) = delete;


protected:

        //- Return the contact angle field
        virtual tmp<volScalarField> theta() const;


public:

    //- Runtime type information
    TypeName("distributionContactAngle");


    // Constructors

        //- Construct from surface film model
        distributionContactAngleForce
        (
            surfaceFilmRegionModel& film,
            const dictionary& dict
        );


    //- Destructor
    virtual ~distributionContactAngleForce();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceFilmModels
} // End namespace regionModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
