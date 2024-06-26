/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018-2020 OpenFOAM Foundation
    Copyright (C) 2020 OpenCFD Ltd
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
    Foam::laminarModels::generalizedNewtonianViscosityModels::CrossPowerLaw

Description
    Cross-Power law generalized Newtonian viscosity model

    The strain rate coefficient can be specified either as the constant \c m or
    the critical stress level at the transition to shear thinning \c
    tauStar if \c tauStar is provided:

    Kinematic viscosity [m^2/s]

    \f[
        \nu = \nu_\infty + \frac{(\nu_0 - \nu_\infty)}{1 + (m\gamma)^n}
    \f]

    or

    \f[
        \nu = \nu_\infty
            + \frac{(\nu_0 - \nu_\infty)}
                   {1 + \left(\frac{\nu_0\gamma}{\tau^*}\right)^n}
    \f]

    Example specification:
    \verbatim
         viscosityModel CrossPowerLaw;

         nuInf      10;
         m          0.4;
         n          3;
    \endverbatim

SourceFiles
    CrossPowerLaw.C

\*---------------------------------------------------------------------------*/

#ifndef CrossPowerLaw_H
#define CrossPowerLaw_H

#include "laminar/generalizedNewtonian/generalizedNewtonianViscosityModels/generalizedNewtonianViscosityModel/generalizedNewtonianViscosityModel.H"
#include "dimensionedTypes/dimensionedScalar/dimensionedScalar.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace laminarModels
{
namespace generalizedNewtonianViscosityModels
{

/*---------------------------------------------------------------------------*\
                           Class CrossPowerLaw Declaration
\*---------------------------------------------------------------------------*/

class CrossPowerLaw
:
    public generalizedNewtonianViscosityModel
{
    // Private Data

        dictionary CrossPowerLawCoeffs_;

        dimensionedScalar nuInf_;
        dimensionedScalar m_;
        dimensionedScalar n_;
        dimensionedScalar tauStar_;


public:

    //- Runtime type information
    TypeName("CrossPowerLaw");


    // Constructors

        //- Construct from dictionary (components)
        explicit CrossPowerLaw(const dictionary& viscosityProperties);


    //- Destructor
    virtual ~CrossPowerLaw() = default;


    // Member Functions

        //- Read transportProperties dictionary
        virtual bool read(const dictionary& viscosityProperties);

        //- Return the laminar viscosity
        virtual tmp<volScalarField> nu
        (
            const volScalarField& nu0,
            const volScalarField& strainRate
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace generalizedNewtonianViscosityModels
} // End namespace laminarModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
