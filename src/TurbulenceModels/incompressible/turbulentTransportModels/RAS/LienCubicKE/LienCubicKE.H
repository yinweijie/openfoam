/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2019-2021 OpenCFD Ltd.
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
    Foam::incompressible::RASModels::LienCubicKE

Group
    grpIcoRASTurbulence

Description
    Lien cubic non-linear low-Reynolds k-epsilon turbulence models for
    incompressible flows.

    This turbulence model is described in:
    \verbatim
        Lien, F.S., Chen, W.L. & Leschziner, M.A. (1996).
        Low-Reynolds-number eddy-viscosity modeling based on non-linear
        stress-strain/vorticity relations.
        Engineering Turbulence Modelling and Experiments 3, 91-100.
    \endverbatim

    Implemented according to the specification in:
    <a href=
    "http://personalpages.manchester.ac.uk/staff/david.d.apsley/specturb.pdf"
    >Apsley: Turbulence Models 2002</a>

    In addition to the low-Reynolds number damping functions support for
    wall-functions is also included to allow for low- and high-Reynolds number
    operation.

See also
    Foam::incompressible::RASModels::ShihQuadraticKE

SourceFiles
    LienCubicKE.C

\*---------------------------------------------------------------------------*/

#ifndef LienCubicKE_H
#define LienCubicKE_H

#include "turbulentTransportModels/turbulentTransportModel.H"
#include "nonlinearEddyViscosity/nonlinearEddyViscosity.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace incompressible
{
namespace RASModels
{

/*---------------------------------------------------------------------------*\
                           Class LienCubicKE Declaration
\*---------------------------------------------------------------------------*/

class LienCubicKE
:
    public nonlinearEddyViscosity<incompressible::RASModel>
{

protected:

    // Protected data

        // Model coefficients

            dimensionedScalar Ceps1_;
            dimensionedScalar Ceps2_;
            dimensionedScalar sigmak_;
            dimensionedScalar sigmaEps_;
            dimensionedScalar Cmu1_;
            dimensionedScalar Cmu2_;
            dimensionedScalar Cbeta_;
            dimensionedScalar Cbeta1_;
            dimensionedScalar Cbeta2_;
            dimensionedScalar Cbeta3_;
            dimensionedScalar Cgamma1_;
            dimensionedScalar Cgamma2_;
            dimensionedScalar Cgamma4_;

            dimensionedScalar Cmu_;
            dimensionedScalar kappa_;

            dimensionedScalar Anu_;
            dimensionedScalar AE_;


        // Fields

            volScalarField k_;
            volScalarField epsilon_;

            //- Wall distance
            //  Note: different to wall distance in parent RASModel
            //  which is for near-wall cells only
            const volScalarField& y_;


    // Protected Member Functions

        tmp<volScalarField> fMu() const;
        tmp<volScalarField> f2() const;
        tmp<volScalarField> E(const volScalarField& f2) const;

        virtual void correctNut();
        virtual void correctNonlinearStress(const volTensorField& gradU);


public:

    //- Runtime type information
    TypeName("LienCubicKE");

    // Constructors

        //- Construct from components
        LienCubicKE
        (
            const geometricOneField& alpha,
            const geometricOneField& rho,
            const volVectorField& U,
            const surfaceScalarField& alphaRhoPhi,
            const surfaceScalarField& phi,
            const transportModel& transport,
            const word& propertiesName = turbulenceModel::propertiesName,
            const word& type = typeName
        );


    //- Destructor
    virtual ~LienCubicKE() = default;


    // Member Functions

        //- Re-read model coefficients if they have changed
        virtual bool read();

        //- Return the effective diffusivity for k
        tmp<volScalarField> DkEff() const
        {
            return tmp<volScalarField>
            (
                new volScalarField("DkEff", nut_/sigmak_ + nu())
            );
        }

        //- Return the effective diffusivity for epsilon
        tmp<volScalarField> DepsilonEff() const
        {
            return tmp<volScalarField>
            (
                new volScalarField("DepsilonEff", nut_/sigmaEps_ + nu())
            );
        }

        //- Return the turbulence kinetic energy
        virtual tmp<volScalarField> k() const
        {
            return k_;
        }

        //- Return the turbulence kinetic energy dissipation rate
        virtual tmp<volScalarField> epsilon() const
        {
            return epsilon_;
        }

        //- Solve the turbulence equations and correct the turbulence viscosity
        virtual void correct();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // Edn namespace incompressible
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
