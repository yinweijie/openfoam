/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 OpenFOAM Foundation
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
    Foam::regionModels::surfaceFilmModels::BrunDrippingInjection

Description
    Film Dripping mass transfer model.

    If the film thickness exceeds the critical value needed to generate one or
    more drops, the equivalent mass is removed from the film.  The critical film
    thickness is calculated from the Rayleigh-Taylor stability analysis of film
    flow on an inclined plane by Brun et.al.

    Reference:
    \verbatim
        Brun, P. T., Damiano, A., Rieu, P., Balestra, G., & Gallaire, F. (2015).
        Rayleigh-Taylor instability under an inclined plane.
        Physics of Fluids (1994-present), 27(8), 084107.
    \endverbatim

    The diameter of the drops formed are obtained from the local capillary
    length multiplied by the \c dCoeff coefficient which defaults to 3.3.

    Reference:
    \verbatim
        Lefebvre, A. (1988).
        Atomization and sprays
        (Vol. 1040, No. 2756). CRC press.
    \endverbatim

SourceFiles
    BrunDrippingInjection.C

\*---------------------------------------------------------------------------*/

#ifndef BrunDrippingInjection_H
#define BrunDrippingInjection_H

#include "submodels/kinematic/injectionModel/injectionModel/injectionModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{
namespace surfaceFilmModels
{

/*---------------------------------------------------------------------------*\
                    Class BrunDrippingInjection Declaration
\*---------------------------------------------------------------------------*/

class BrunDrippingInjection
:
    public injectionModel
{
    // Private member functions

        //- No copy construct
        BrunDrippingInjection(const BrunDrippingInjection&) = delete;

        //- No copy assignment
        void operator=(const BrunDrippingInjection&) = delete;


protected:

    // Protected data

        //- Critical non-dimensional interface velocity
        //  Coefficient in the film angle stability function.
        //  Defaults to 1.62208
        scalar ubarStar_;

        //- Coefficient relating the diameter of the drops formed to
        //  the capillary length.
        //  Defaults to 3.3
        scalar dCoeff_;

        //- Stable film thickness - drips only formed if thickness
        //  exceeds this threshold value
        scalar deltaStable_;

        //- Diameters of particles to inject into the dripping
        scalarList diameter_;


public:

    //- Runtime type information
    TypeName("BrunDrippingInjection");


    // Constructors

        //- Construct from surface film model
        BrunDrippingInjection
        (
            surfaceFilmRegionModel& film,
            const dictionary& dict
        );


    //- Destructor
    virtual ~BrunDrippingInjection();


    // Member Functions

        //- Correct
        virtual void correct
        (
            scalarField& availableMass,
            scalarField& massToInject,
            scalarField& diameterToInject
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceFilmModels
} // End namespace regionModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
