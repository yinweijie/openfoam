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

Class
    Foam::laminarModels::generalizedNewtonianViscosityModels::HerschelBulkley

Description
    Herschel-Bulkley generalized Newtonian viscosity model

SourceFiles
    HerschelBulkley.C

\*---------------------------------------------------------------------------*/

#ifndef HerschelBulkley_H
#define HerschelBulkley_H

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
                           Class HerschelBulkley Declaration
\*---------------------------------------------------------------------------*/

class HerschelBulkley
:
    public generalizedNewtonianViscosityModel
{
    // Private Data

        dictionary HerschelBulkleyCoeffs_;

        dimensionedScalar n_;
        dimensionedScalar tau0_;


public:

    //- Runtime type information
    TypeName("HerschelBulkley");


    // Constructors

        //- Construct from dictionary (components)
        explicit HerschelBulkley(const dictionary& viscosityProperties);


    //- Destructor
    virtual ~HerschelBulkley() = default;


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
