/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2019-2020,2023 OpenCFD Ltd.
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

\*---------------------------------------------------------------------------*/

#include "turbulentTransportModels/RAS/LienLeschziner/LienLeschziner.H"
#include "fvMesh/wallDist/wallDist/wallDist.H"
#include "cfdTools/general/bound/bound.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace incompressible
{
namespace RASModels
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(LienLeschziner, 0);
addToRunTimeSelectionTable(RASModel, LienLeschziner, dictionary);

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

tmp<volScalarField> LienLeschziner::fMu() const
{
    const volScalarField yStar(sqrt(k_)*y_/nu());

    return
        (scalar(1) - exp(-Anu_*yStar))
       /((scalar(1) + SMALL) - exp(-Aeps_*yStar));
}


tmp<volScalarField> LienLeschziner::f2() const
{
    tmp<volScalarField> Rt = sqr(k_)/(nu()*epsilon_);

    return scalar(1) - 0.3*exp(-sqr(Rt));
}


tmp<volScalarField> LienLeschziner::E(const volScalarField& f2) const
{
    const volScalarField yStar(sqrt(k_)*y_/nu());
    const volScalarField le
    (
        kappa_*y_*((scalar(1) + SMALL) - exp(-Aeps_*yStar))
    );

    return
        (Ceps2_*pow(Cmu_, 0.75))
       *(f2*sqrt(k_)*epsilon_/le)*exp(-AE_*sqr(yStar));
}


void LienLeschziner::correctNut()
{
    nut_ = Cmu_*fMu()*sqr(k_)/epsilon_;
    nut_.correctBoundaryConditions();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

LienLeschziner::LienLeschziner
(
    const geometricOneField& alpha,
    const geometricOneField& rho,
    const volVectorField& U,
    const surfaceScalarField& alphaRhoPhi,
    const surfaceScalarField& phi,
    const transportModel& transport,
    const word& propertiesName,
    const word& type
)
:
    eddyViscosity<incompressible::RASModel>
    (
        type,
        alpha,
        rho,
        U,
        alphaRhoPhi,
        phi,
        transport,
        propertiesName
    ),

    Ceps1_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Ceps1",
            coeffDict_,
            1.44
        )
    ),
    Ceps2_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Ceps2",
            coeffDict_,
            1.92
        )
    ),
    sigmak_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "sigmak",
            coeffDict_,
            1.0
        )
    ),
    sigmaEps_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "sigmaEps",
            coeffDict_,
            1.3
        )
    ),
    Cmu_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cmu",
            coeffDict_,
            0.09
        )
    ),
    kappa_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "kappa",
            coeffDict_,
            0.41
        )
    ),
    Anu_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Anu",
            coeffDict_,
            0.016
        )
    ),
    Aeps_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Aeps",
            coeffDict_,
            0.263
        )
    ),
    AE_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "AE",
            coeffDict_,
            0.00222
        )
    ),

    k_
    (
        IOobject
        (
            IOobject::groupName("k", alphaRhoPhi.group()),
            runTime_.timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    ),

    epsilon_
    (
        IOobject
        (
            IOobject::groupName("epsilon", alphaRhoPhi.group()),
            runTime_.timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    ),

    y_(wallDist::New(mesh_).y())
{
    bound(k_, kMin_);
    bound(epsilon_, epsilonMin_);

    if (type == typeName)
    {
        printCoeffs(type);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool LienLeschziner::read()
{
    if (eddyViscosity<incompressible::RASModel>::read())
    {
        Ceps1_.readIfPresent(coeffDict());
        Ceps2_.readIfPresent(coeffDict());
        sigmak_.readIfPresent(coeffDict());
        sigmaEps_.readIfPresent(coeffDict());
        Cmu_.readIfPresent(coeffDict());
        kappa_.readIfPresent(coeffDict());
        Anu_.readIfPresent(coeffDict());
        Aeps_.readIfPresent(coeffDict());
        AE_.readIfPresent(coeffDict());

        return true;
    }

    return false;
}


void LienLeschziner::correct()
{
    if (!turbulence_)
    {
        return;
    }

    eddyViscosity<incompressible::RASModel>::correct();

    tmp<volTensorField> tgradU = fvc::grad(U_);
    volScalarField G
    (
        GName(),
        nut_*(tgradU() && twoSymm(tgradU()))
    );
    tgradU.clear();

    // Update epsilon and G at the wall
    epsilon_.boundaryFieldRef().updateCoeffs();
    // Push any changed cell values to coupled neighbours
    epsilon_.boundaryFieldRef().evaluateCoupled<coupledFvPatch>();

    const volScalarField f2(this->f2());

    // Dissipation equation
    tmp<fvScalarMatrix> epsEqn
    (
        fvm::ddt(epsilon_)
      + fvm::div(phi_, epsilon_)
      - fvm::laplacian(DepsilonEff(), epsilon_)
     ==
        Ceps1_*G*epsilon_/k_
      - fvm::Sp(Ceps2_*f2*epsilon_/k_, epsilon_)
      + E(f2)
    );

    epsEqn.ref().relax();
    epsEqn.ref().boundaryManipulate(epsilon_.boundaryFieldRef());
    solve(epsEqn);
    bound(epsilon_, epsilonMin_);


    // Turbulent kinetic energy equation
    tmp<fvScalarMatrix> kEqn
    (
        fvm::ddt(k_)
      + fvm::div(phi_, k_)
      - fvm::laplacian(DkEff(), k_)
     ==
        G
      - fvm::Sp(epsilon_/k_, k_)
    );

    kEqn.ref().relax();
    solve(kEqn);
    bound(k_, kMin_);

    correctNut();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace incompressible
} // End namespace Foam

// ************************************************************************* //
