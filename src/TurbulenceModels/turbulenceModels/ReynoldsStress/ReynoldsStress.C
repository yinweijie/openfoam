/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2017 OpenFOAM Foundation
    Copyright (C) 2020-2023 OpenCFD Ltd.
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

#include "ReynoldsStress/ReynoldsStress.H"
#include "finiteVolume/fvc/fvc.H"
#include "finiteVolume/fvm/fvm.H"
#include "fvMesh/fvPatches/derived/wall/wallFvPatch.H"

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class BasicTurbulenceModel>
void Foam::ReynoldsStress<BasicTurbulenceModel>::boundNormalStress
(
    volSymmTensorField& R
) const
{
    const scalar kMin = this->kMin_.value();

    R.clamp_min
    (
        symmTensor
        (
            kMin, -GREAT, -GREAT,
            kMin, -GREAT,
            kMin
        )
    );
}


template<class BasicTurbulenceModel>
void Foam::ReynoldsStress<BasicTurbulenceModel>::correctWallShearStress
(
    volSymmTensorField& R
) const
{
    const fvPatchList& patches = this->mesh_.boundary();

    volSymmTensorField::Boundary& RBf = R.boundaryFieldRef();

    forAll(patches, patchi)
    {
        const fvPatch& curPatch = patches[patchi];

        if (isA<wallFvPatch>(curPatch))
        {
            symmTensorField& Rw = RBf[patchi];

            const scalarField& nutw = this->nut_.boundaryField()[patchi];

            const vectorField snGradU
            (
                this->U_.boundaryField()[patchi].snGrad()
            );

            const vectorField& faceAreas
                = this->mesh_.Sf().boundaryField()[patchi];

            const scalarField& magFaceAreas
                = this->mesh_.magSf().boundaryField()[patchi];

            forAll(curPatch, facei)
            {
                // Calculate near-wall velocity gradient
                const tensor gradUw
                    = (faceAreas[facei]/magFaceAreas[facei])*snGradU[facei];

                // Set the wall Reynolds-stress to the near-wall shear-stress
                // Note: the spherical part of the normal stress is included in
                // the pressure
                Rw[facei] = -nutw[facei]*2*devSymm(gradUw);
            }
        }
    }
}


template<class BasicTurbulenceModel>
void Foam::ReynoldsStress<BasicTurbulenceModel>::checkRealizabilityConditions
(
    const volSymmTensorField& R
) const
{
    const label maxDiffs = 5;
    label nDiffs = 0;

    // (S:Eq. 4a-4c)
    forAll(R, celli)
    {
        bool diff = false;

        if (maxDiffs < nDiffs)
        {
            Info<< "More than " << maxDiffs << " times"
                << " Reynolds-stress realizability checks failed."
                << " Skipping further comparisons." << endl;
            return;
        }

        const symmTensor& r = R[celli];

        if (r.xx() < 0)
        {
            WarningInFunction
                << "Reynolds stress " << r << " at cell " << celli
                << " does not obey the constraint: Rxx >= 0"
                << endl;
            diff = true;
        }

        if (r.xx()*r.yy() - sqr(r.xy()) < 0)
        {
            WarningInFunction
                << "Reynolds stress " << r << " at cell " << celli
                << " does not obey the constraint: Rxx*Ryy - sqr(Rxy) >= 0"
                << endl;
            diff = true;
        }

        if (det(r) < 0)
        {
            WarningInFunction
                << "Reynolds stress " << r << " at cell " << celli
                << " does not obey the constraint: det(R) >= 0"
                << endl;
            diff = true;
        }

        if (diff)
        {
            ++nDiffs;
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class BasicTurbulenceModel>
Foam::ReynoldsStress<BasicTurbulenceModel>::ReynoldsStress
(
    const word& modelName,
    const alphaField& alpha,
    const rhoField& rho,
    const volVectorField& U,
    const surfaceScalarField& alphaRhoPhi,
    const surfaceScalarField& phi,
    const transportModel& transport,
    const word& propertiesName
)
:
    BasicTurbulenceModel
    (
        modelName,
        alpha,
        rho,
        U,
        alphaRhoPhi,
        phi,
        transport,
        propertiesName
    ),

    couplingFactor_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "couplingFactor",
            this->coeffDict_,
            0.0
        )
    ),

    R_
    (
        IOobject
        (
            IOobject::groupName("R", alphaRhoPhi.group()),
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_
    ),

    nut_
    (
        IOobject
        (
            IOobject::groupName("nut", alphaRhoPhi.group()),
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_
    )
{
    if (couplingFactor_.value() < 0.0 || couplingFactor_.value() > 1.0)
    {
        FatalErrorInFunction
            << "couplingFactor = " << couplingFactor_
            << " is not in range 0 - 1" << nl
            << exit(FatalError);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class BasicTurbulenceModel>
bool Foam::ReynoldsStress<BasicTurbulenceModel>::read()
{
    return BasicTurbulenceModel::read();
}


template<class BasicTurbulenceModel>
Foam::tmp<Foam::volSymmTensorField>
Foam::ReynoldsStress<BasicTurbulenceModel>::R() const
{
    return R_;
}


template<class BasicTurbulenceModel>
Foam::tmp<Foam::volScalarField>
Foam::ReynoldsStress<BasicTurbulenceModel>::k() const
{
    tmp<Foam::volScalarField> tk(0.5*tr(R_));
    tk.ref().rename("k");
    return tk;
}


template<class BasicTurbulenceModel>
Foam::tmp<Foam::volSymmTensorField>
Foam::ReynoldsStress<BasicTurbulenceModel>::devRhoReff() const
{
    return devRhoReff(this->U_);
}


template<class BasicTurbulenceModel>
Foam::tmp<Foam::volSymmTensorField>
Foam::ReynoldsStress<BasicTurbulenceModel>::devRhoReff
(
    const volVectorField& U
) const
{
    return tmp<volSymmTensorField>
    (
        new volSymmTensorField
        (
            IOobject
            (
                IOobject::groupName("devRhoReff", this->alphaRhoPhi_.group()),
                this->runTime_.timeName(),
                this->mesh_,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            this->alpha_*this->rho_*R_
          - (this->alpha_*this->rho_*this->nu())
           *devTwoSymm(fvc::grad(U))
        )
    );
}


template<class BasicTurbulenceModel>
template<class RhoFieldType>
Foam::tmp<Foam::fvVectorMatrix>
Foam::ReynoldsStress<BasicTurbulenceModel>::DivDevRhoReff
(
    const RhoFieldType& rho,
    volVectorField& U
) const
{
    if (couplingFactor_.value() > 0.0)
    {
        return
        (
            fvc::laplacian
            (
                (1.0 - couplingFactor_)*this->alpha_*rho*this->nut(),
                U,
                "laplacian(nuEff,U)"
            )
          + fvc::div
            (
                this->alpha_*rho*R_
              + couplingFactor_
               *this->alpha_*rho*this->nut()*fvc::grad(U),
                "div(devRhoReff)"
            )
          - fvc::div(this->alpha_*rho*this->nu()*dev2(T(fvc::grad(U))))
          - fvm::laplacian(this->alpha_*rho*this->nuEff(), U)
        );
    }
    else
    {
        return
        (
            fvc::laplacian
            (
                this->alpha_*rho*this->nut(),
                U,
                "laplacian(nuEff,U)"
            )
          + fvc::div(this->alpha_*rho*R_)
          - fvc::div(this->alpha_*rho*this->nu()*dev2(T(fvc::grad(U))))
          - fvm::laplacian(this->alpha_*rho*this->nuEff(), U)
        );
    }
}


template<class BasicTurbulenceModel>
Foam::tmp<Foam::fvVectorMatrix>
Foam::ReynoldsStress<BasicTurbulenceModel>::divDevRhoReff
(
    volVectorField& U
) const
{
    return DivDevRhoReff(this->rho_, U);
}


template<class BasicTurbulenceModel>
Foam::tmp<Foam::fvVectorMatrix>
Foam::ReynoldsStress<BasicTurbulenceModel>::divDevRhoReff
(
    const volScalarField& rho,
    volVectorField& U
) const
{
    return DivDevRhoReff(rho, U);
}


template<class BasicTurbulenceModel>
void Foam::ReynoldsStress<BasicTurbulenceModel>::validate()
{
    correctNut();
}


template<class BasicTurbulenceModel>
void Foam::ReynoldsStress<BasicTurbulenceModel>::correct()
{
    BasicTurbulenceModel::correct();
}


// ************************************************************************* //
