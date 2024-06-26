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
    Foam::TurbulenceModel

Description
    Templated abstract base class for turbulence models

SourceFiles
    TurbulenceModel.C

\*---------------------------------------------------------------------------*/

#ifndef TurbulenceModel_H
#define TurbulenceModel_H

#include "turbulenceModel.H"
#include "memory/autoPtr/autoPtr.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class TurbulenceModel Declaration
\*---------------------------------------------------------------------------*/

template
<
    class Alpha,
    class Rho,
    class BasicTurbulenceModel,
    class TransportModel
>
class TurbulenceModel
:
    public BasicTurbulenceModel
{

public:

    typedef Alpha alphaField;
    typedef Rho rhoField;
    typedef TransportModel transportModel;


protected:

    // Protected data

        const alphaField& alpha_;
        const transportModel& transport_;


    // Protected Member Functions

        //- No copy construct
        TurbulenceModel(const TurbulenceModel&) = delete;

        //- No copy assignment
        void operator=(const TurbulenceModel&) = delete;


public:

    // Declare run-time constructor selection table

        declareRunTimeNewSelectionTable
        (
            autoPtr,
            TurbulenceModel,
            dictionary,
            (
                const alphaField& alpha,
                const rhoField& rho,
                const volVectorField& U,
                const surfaceScalarField& alphaRhoPhi,
                const surfaceScalarField& phi,
                const transportModel& transport,
                const word& propertiesName
            ),
            (alpha, rho, U, alphaRhoPhi, phi, transport, propertiesName)
        );


    // Constructors

        //- Construct
        TurbulenceModel
        (
            const alphaField& alpha,
            const rhoField& rho,
            const volVectorField& U,
            const surfaceScalarField& alphaRhoPhi,
            const surfaceScalarField& phi,
            const transportModel& transport,
            const word& propertiesName
        );


    // Selectors

        //- Return a reference to the selected turbulence model
        static autoPtr<TurbulenceModel> New
        (
            const alphaField& alpha,
            const rhoField& rho,
            const volVectorField& U,
            const surfaceScalarField& alphaRhoPhi,
            const surfaceScalarField& phi,
            const transportModel& transport,
            const word& propertiesName = turbulenceModel::propertiesName
        );


    //- Destructor
    virtual ~TurbulenceModel() = default;


    // Member Functions

        //- Access function to phase fraction
        const alphaField& alpha() const
        {
            return alpha_;
        }

        //- Access function to incompressible transport model
        const transportModel& transport() const
        {
            return transport_;
        }

        //- Return the laminar viscosity
        virtual tmp<volScalarField> nu() const
        {
            return transport_.nu();
        }

        //- Return the laminar viscosity on patchi
        virtual tmp<scalarField> nu(const label patchi) const
        {
            return transport_.nu(patchi);
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "TurbulenceModel/TurbulenceModelPascal.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
