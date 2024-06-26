/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2019-2023 OpenCFD Ltd.
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

#include "turbulentTransportModels/RAS/kkLOmega/kkLOmega.H"
#include "cfdTools/general/bound/bound.H"
#include "fvMesh/wallDist/wallDist/wallDist.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace incompressible
{
namespace RASModels
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(kkLOmega, 0);
addToRunTimeSelectionTable(RASModel, kkLOmega, dictionary);

// * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * * //

tmp<volScalarField> kkLOmega::fv(const volScalarField& Ret) const
{
    return(1.0 - exp(-sqrt(Ret)/Av_));
}


tmp<volScalarField> kkLOmega::fINT() const
{
    return
    (
        min
        (
            kt_/(Cint_*(kl_ + kt_ + kMin_)),
            dimensionedScalar("1.0", dimless, 1.0)
        )
    );
}


tmp<volScalarField> kkLOmega::fSS(const volScalarField& Omega) const
{
    return(exp(-sqr(Css_*nu()*Omega/(kt_ + kMin_))));
}


tmp<volScalarField> kkLOmega::Cmu(const volScalarField& S) const
{
    return(1.0/(A0_ + As_*(S/(omega_ + omegaMin_))));
}


tmp<volScalarField> kkLOmega::BetaTS(const volScalarField& ReOmega) const
{
    return(scalar(1) - exp(-sqr(max(ReOmega - CtsCrit_, scalar(0)))/Ats_));
}


tmp<volScalarField> kkLOmega::fTaul
(
    const volScalarField& lambdaEff,
    const volScalarField& ktL,
    const volScalarField& Omega
) const
{
    return
    (
        scalar(1)
      - exp
        (
            -CtauL_*ktL
          /
            (
                sqr
                (
                    lambdaEff*Omega
                  + dimensionedScalar
                    (
                        "ROOTVSMALL",
                        dimLength*inv(dimTime),
                        ROOTVSMALL
                    )
                )
            )
        )
    );
}


tmp<volScalarField> kkLOmega::alphaT
(
    const volScalarField& lambdaEff,
    const volScalarField& fv,
    const volScalarField& ktS
) const
{
    return(fv*CmuStd_*sqrt(ktS)*lambdaEff);
}


tmp<volScalarField> kkLOmega::fOmega
(
    const volScalarField& lambdaEff,
    const volScalarField& lambdaT
) const
{
    return
    (
        scalar(1)
      - exp
        (
           -0.41
           *pow4
            (
                lambdaEff
              / (
                    lambdaT
                  + dimensionedScalar
                    (
                        "ROTVSMALL",
                        lambdaT.dimensions(),
                        ROOTVSMALL
                    )
                )
            )
        )
    );
}


tmp<volScalarField> kkLOmega::phiBP(const volScalarField& Omega) const
{
    return
    (
        min
        (
            max
            (
                kt_/nu()
             / (
                    Omega
                  + dimensionedScalar
                    (
                        "ROTVSMALL",
                        Omega.dimensions(),
                        ROOTVSMALL
                    )
                )
              - CbpCrit_,
                scalar(0)
            ),
            scalar(50)
        )
    );
}


tmp<volScalarField> kkLOmega::phiNAT
(
    const volScalarField& ReOmega,
    const volScalarField& fNatCrit
) const
{
    return
    (
        max
        (
            ReOmega
          - CnatCrit_
          / (
                fNatCrit + dimensionedScalar("ROTVSMALL", dimless, ROOTVSMALL)
            ),
            scalar(0)
        )
    );
}


tmp<volScalarField> kkLOmega::D(const volScalarField& k) const
{
    return nu()*magSqr(fvc::grad(sqrt(k)));
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

void kkLOmega::correctNut()
{
    // Currently this function is not implemented due to the complexity of
    // evaluating nut.  Better calculate nut at the end of correct()
    NotImplemented;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

kkLOmega::kkLOmega
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

    A0_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "A0",
            coeffDict_,
            4.04
        )
    ),
    As_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "As",
            coeffDict_,
            2.12
        )
    ),
    Av_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Av",
            coeffDict_,
            6.75
        )
    ),
    Abp_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Abp",
            coeffDict_,
            0.6
        )
    ),
    Anat_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Anat",
            coeffDict_,
            200
        )
    ),
    Ats_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Ats",
            coeffDict_,
            200
        )
    ),
    CbpCrit_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "CbpCrit",
            coeffDict_,
            1.2
        )
    ),
    Cnc_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cnc",
            coeffDict_,
            0.1
        )
    ),
    CnatCrit_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "CnatCrit",
            coeffDict_,
            1250
        )
    ),
    Cint_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cint",
            coeffDict_,
            0.75
        )
    ),
    CtsCrit_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "CtsCrit",
            coeffDict_,
            1000
        )
    ),
    CrNat_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "CrNat",
            coeffDict_,
            0.02
        )
    ),
    C11_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "C11",
            coeffDict_,
            3.4e-6
        )
    ),
    C12_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "C12",
            coeffDict_,
            1.0e-10
        )
    ),
    CR_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "CR",
            coeffDict_,
            0.12
        )
    ),
    CalphaTheta_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "CalphaTheta",
            coeffDict_,
            0.035
        )
    ),
    Css_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Css",
            coeffDict_,
            1.5
        )
    ),
    CtauL_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "CtauL",
            coeffDict_,
            4360
        )
    ),
    Cw1_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cw1",
            coeffDict_,
            0.44
        )
    ),
    Cw2_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cw2",
            coeffDict_,
            0.92
        )
    ),
    Cw3_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cw3",
            coeffDict_,
            0.3
        )
    ),
    CwR_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "CwR",
            coeffDict_,
            1.5
        )
    ),
    Clambda_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Clambda",
            coeffDict_,
            2.495
        )
    ),
    CmuStd_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "CmuStd",
            coeffDict_,
            0.09
        )
    ),
    Prtheta_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Prtheta",
            coeffDict_,
            0.85
        )
    ),
    Sigmak_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Sigmak",
            coeffDict_,
            1
        )
    ),
    Sigmaw_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Sigmaw",
            coeffDict_,
            1.17
        )
    ),
    kt_
    (
        IOobject
        (
            IOobject::groupName("kt", alphaRhoPhi.group()),
            runTime_.timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    ),
    kl_
    (
        IOobject
        (
            IOobject::groupName("kl", alphaRhoPhi.group()),
            runTime_.timeName(),
            mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh_
    ),
    omega_
    (
        IOobject
        (
            IOobject::groupName("omega", alphaRhoPhi.group()),
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
            "epsilon",
            runTime_.timeName(),
            mesh_
        ),
        kt_*omega_ + D(kl_) + D(kt_)
    ),
    y_(wallDist::New(mesh_).y())
{
    bound(kt_, kMin_);
    bound(kl_, kMin_);
    bound(omega_, omegaMin_);
    bound(epsilon_, epsilonMin_);

    if (type == typeName)
    {
        // Evaluating nut_ is complex so start from the field read from file
        nut_.correctBoundaryConditions();

        printCoeffs(type);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool kkLOmega::read()
{
    if (eddyViscosity<incompressible::RASModel>::read())
    {
        A0_.readIfPresent(coeffDict());
        As_.readIfPresent(coeffDict());
        Av_.readIfPresent(coeffDict());
        Abp_.readIfPresent(coeffDict());
        Anat_.readIfPresent(coeffDict());
        Abp_.readIfPresent(coeffDict());
        Ats_.readIfPresent(coeffDict());
        CbpCrit_.readIfPresent(coeffDict());
        Cnc_.readIfPresent(coeffDict());
        CnatCrit_.readIfPresent(coeffDict());
        Cint_.readIfPresent(coeffDict());
        CtsCrit_.readIfPresent(coeffDict());
        CrNat_.readIfPresent(coeffDict());
        C11_.readIfPresent(coeffDict());
        C12_.readIfPresent(coeffDict());
        CR_.readIfPresent(coeffDict());
        CalphaTheta_.readIfPresent(coeffDict());
        Css_.readIfPresent(coeffDict());
        CtauL_.readIfPresent(coeffDict());
        Cw1_.readIfPresent(coeffDict());
        Cw2_.readIfPresent(coeffDict());
        Cw3_.readIfPresent(coeffDict());
        CwR_.readIfPresent(coeffDict());
        Clambda_.readIfPresent(coeffDict());
        CmuStd_.readIfPresent(coeffDict());
        Prtheta_.readIfPresent(coeffDict());
        Sigmak_.readIfPresent(coeffDict());
        Sigmaw_.readIfPresent(coeffDict());

        return true;
    }

    return false;
}


void kkLOmega::validate()
{}


void kkLOmega::correct()
{
    eddyViscosity<incompressible::RASModel>::correct();

    if (!turbulence_)
    {
        return;
    }

    const volScalarField lambdaT(sqrt(kt_)/(omega_ + omegaMin_));

    const volScalarField lambdaEff(min(Clambda_*y_, lambdaT));

    const volScalarField fw
    (
        pow
        (
            lambdaEff
           /(lambdaT + dimensionedScalar("SMALL", dimLength, ROOTVSMALL)),
            2.0/3.0
        )
    );

    tmp<volTensorField> tgradU(fvc::grad(U_));
    const volTensorField& gradU = tgradU();

    const volScalarField Omega(sqrt(2.0)*mag(skew(gradU)));

    const volScalarField S2(2.0*magSqr(devSymm(gradU)));

    const volScalarField ktS(fSS(Omega)*fw*kt_);

    const volScalarField nuts
    (
        fv(sqr(fw)*kt_/nu()/(omega_ + omegaMin_))
       *fINT()
       *Cmu(sqrt(S2))*sqrt(ktS)*lambdaEff
    );
    volScalarField Pkt(this->GName(), nuts*S2);

    const volScalarField ktL(kt_ - ktS);
    const volScalarField ReOmega(sqr(y_)*Omega/nu());
    const volScalarField nutl
    (
        min
        (
            C11_*fTaul(lambdaEff, ktL, Omega)*Omega*sqr(lambdaEff)
           *sqrt(ktL)*lambdaEff/nu()
          + C12_*BetaTS(ReOmega)*ReOmega*sqr(y_)*Omega
        ,
            0.5*(kl_ + ktL)/(sqrt(S2) + omegaMin_)
        )
    );

    const volScalarField Pkl(nutl*S2);

    const volScalarField alphaTEff
    (
        alphaT(lambdaEff, fv(sqr(fw)*kt_/nu()/(omega_ + omegaMin_)), ktS)
    );

    // By pass source term divided by kl_

    const dimensionedScalar fwMin("SMALL", dimless, ROOTVSMALL);

    const volScalarField Rbp
    (
        CR_*(1.0 - exp(-phiBP(Omega)()/Abp_))*omega_
       /(fw + fwMin)
    );

    const volScalarField fNatCrit(1.0 - exp(-Cnc_*sqrt(kl_)*y_/nu()));

    // Natural source term divided by kl_
    const volScalarField Rnat
    (
        CrNat_*(1.0 - exp(-phiNAT(ReOmega, fNatCrit)/Anat_))*Omega
    );


    omega_.boundaryFieldRef().updateCoeffs();
    // Push any changed cell values to coupled neighbours
    omega_.boundaryFieldRef().evaluateCoupled<coupledFvPatch>();

    // Turbulence specific dissipation rate equation
    tmp<fvScalarMatrix> omegaEqn
    (
        fvm::ddt(omega_)
      + fvm::div(phi_, omega_)
      - fvm::laplacian(DomegaEff(alphaTEff), omega_)
     ==
        Cw1_*Pkt*omega_/(kt_ + kMin_)
      - fvm::SuSp
        (
            (1.0 - CwR_/(fw + fwMin))*kl_*(Rbp + Rnat)/(kt_ + kMin_)
          , omega_
        )
      - fvm::Sp(Cw2_*sqr(fw)*omega_, omega_)
      + (
            Cw3_*fOmega(lambdaEff, lambdaT)*alphaTEff*sqr(fw)*sqrt(kt_)
        )()()/pow3(y_())
    );

    omegaEqn.ref().relax();
    omegaEqn.ref().boundaryManipulate(omega_.boundaryFieldRef());

    solve(omegaEqn);
    bound(omega_, omegaMin_);


    const volScalarField Dl(D(kl_));

    // Laminar kinetic energy equation
    tmp<fvScalarMatrix> klEqn
    (
        fvm::ddt(kl_)
      + fvm::div(phi_, kl_)
      - fvm::laplacian(nu(), kl_)
     ==
        Pkl
      - fvm::Sp(Rbp + Rnat + Dl/(kl_ + kMin_), kl_)
    );

    klEqn.ref().relax();
    klEqn.ref().boundaryManipulate(kl_.boundaryFieldRef());

    solve(klEqn);
    bound(kl_, kMin_);


    const volScalarField Dt(D(kt_));

    // Turbulent kinetic energy equation
    tmp<fvScalarMatrix> ktEqn
    (
        fvm::ddt(kt_)
      + fvm::div(phi_, kt_)
      - fvm::laplacian(DkEff(alphaTEff), kt_)
     ==
        Pkt
      + (Rbp + Rnat)*kl_
      - fvm::Sp(omega_ + Dt/(kt_+ kMin_), kt_)
    );

    ktEqn.ref().relax();
    ktEqn.ref().boundaryManipulate(kt_.boundaryFieldRef());

    solve(ktEqn);
    bound(kt_, kMin_);


    // Update total fluctuation kinetic energy dissipation rate
    epsilon_ = kt_*omega_ + Dl + Dt;
    bound(epsilon_, epsilonMin_);


    // Re-calculate turbulent viscosity
    nut_ = nuts + nutl;
    nut_.correctBoundaryConditions();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace incompressible
} // End namespace Foam

// ************************************************************************* //
