/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "phaseModel/phaseModel.H"
#include "twoPhaseSystem/twoPhaseSystem.H"
#include "diameterModels/diameterModel/diameterModel.H"
#include "fvMatrices/fvMatrix/fvMatrix.H"
#include "TurbulenceModels/phaseCompressible/PhaseCompressibleTurbulenceModel/PhaseCompressibleTurbulenceModelPascal.H"
#include "interfacialModels/dragModels/dragModel/dragModel.H"
#include "interfacialModels/heatTransferModels/heatTransferModel/heatTransferModel.H"
#include "fields/fvsPatchFields/basic/fixedValue/fixedValueFvsPatchFields.H"
#include "fields/fvPatchFields/basic/fixedValue/fixedValueFvPatchFields.H"
#include "fields/fvPatchFields/derived/slip/slipFvPatchFields.H"
#include "fields/fvPatchFields/derived/partialSlip/partialSlipFvPatchFields.H"
#include "finiteVolume/fvc/fvcFlux.H"
#include "interpolation/surfaceInterpolation/surfaceInterpolation/surfaceInterpolate.H"


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::phaseModel::phaseModel
(
    const twoPhaseSystem& fluid,
    const dictionary& phaseProperties,
    const word& phaseName
)
:
    volScalarField
    (
        IOobject
        (
            IOobject::groupName("alpha", phaseName),
            fluid.mesh().time().timeName(),
            fluid.mesh(),
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        ),
        fluid.mesh(),
        dimensionedScalar(dimless, Zero)
    ),
    fluid_(fluid),
    name_(phaseName),
    phaseDict_
    (
        phaseProperties.subDict(name_)
    ),
    residualAlpha_
    (
        "residualAlpha",
        dimless,
        fluid.subDict(phaseName)
    ),
    alphaMax_(phaseDict_.getOrDefault<scalar>("alphaMax", 1)),
    thermo_(rhoThermo::New(fluid.mesh(), name_)),
    U_
    (
        IOobject
        (
            IOobject::groupName("U", name_),
            fluid.mesh().time().timeName(),
            fluid.mesh(),
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        fluid.mesh()
    ),
    alphaPhi_
    (
        IOobject
        (
            IOobject::groupName("alphaPhi", name_),
            fluid.mesh().time().timeName(),
            fluid.mesh()
        ),
        fluid.mesh(),
        dimensionedScalar(dimensionSet(0, 3, -1, 0, 0), Zero)
    ),
    alphaRhoPhi_
    (
        IOobject
        (
            IOobject::groupName("alphaRhoPhi", name_),
            fluid.mesh().time().timeName(),
            fluid.mesh()
        ),
        fluid.mesh(),
        dimensionedScalar(dimensionSet(1, 0, -1, 0, 0), Zero)
    )
{
    alphaPhi_.setOriented();
    alphaRhoPhi_.setOriented();

    thermo_->validate("phaseModel " + name_, "h", "e");

    const word phiName = IOobject::groupName("phi", name_);

    IOobject phiHeader
    (
        phiName,
        fluid_.mesh().time().timeName(),
        fluid_.mesh(),
        IOobject::NO_READ
    );

    if (phiHeader.typeHeaderOk<surfaceScalarField>(true))
    {
        Info<< "Reading face flux field " << phiName << endl;

        phiPtr_.reset
        (
            new surfaceScalarField
            (
                IOobject
                (
                    phiName,
                    fluid_.mesh().time().timeName(),
                    fluid_.mesh(),
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                ),
                fluid_.mesh()
            )
        );
    }
    else
    {
        Info<< "Calculating face flux field " << phiName << endl;

        wordList phiTypes
        (
            U_.boundaryField().size(),
            fvsPatchFieldBase::calculatedType()
        );

        forAll(U_.boundaryField(), i)
        {
            if
            (
                isA<fixedValueFvPatchVectorField>(U_.boundaryField()[i])
             || isA<slipFvPatchVectorField>(U_.boundaryField()[i])
             || isA<partialSlipFvPatchVectorField>(U_.boundaryField()[i])
            )
            {
                phiTypes[i] = fixedValueFvsPatchScalarField::typeName;
            }
        }

        phiPtr_.reset
        (
            new surfaceScalarField
            (
                IOobject
                (
                    phiName,
                    fluid_.mesh().time().timeName(),
                    fluid_.mesh(),
                    IOobject::NO_READ,
                    IOobject::AUTO_WRITE
                ),
                fvc::flux(U_),
                phiTypes
            )
        );
    }

    dPtr_ = diameterModel::New
    (
        phaseDict_,
        *this
    );

    turbulence_ =
        PhaseCompressibleTurbulenceModel<phaseModel>::New
        (
            *this,
            thermo_->rho(),
            U_,
            alphaRhoPhi_,
            phi(),
            *this
        );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::phaseModel::~phaseModel()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::phaseModel& Foam::phaseModel::otherPhase() const
{
    return fluid_.otherPhase(*this);
}


Foam::tmp<Foam::volScalarField> Foam::phaseModel::d() const
{
    return dPtr_().d();
}


Foam::PhaseCompressibleTurbulenceModel<Foam::phaseModel>&
Foam::phaseModel::turbulence()
{
    return *turbulence_;
}


const Foam::PhaseCompressibleTurbulenceModel<Foam::phaseModel>&
Foam::phaseModel::turbulence() const
{
    return *turbulence_;
}


void Foam::phaseModel::correct()
{
    return dPtr_->correct();
}


bool Foam::phaseModel::read(const dictionary& phaseProperties)
{
    phaseDict_ = phaseProperties.subDict(name_);
    return dPtr_->read(phaseDict_);
}


void Foam::phaseModel::correctInflowOutflow(surfaceScalarField& alphaPhi) const
{
    surfaceScalarField::Boundary& alphaPhiBf = alphaPhi.boundaryFieldRef();
    const volScalarField::Boundary& alphaBf = boundaryField();
    const surfaceScalarField::Boundary& phiBf = phi().boundaryField();

    forAll(alphaPhiBf, patchi)
    {
        fvsPatchScalarField& alphaPhip = alphaPhiBf[patchi];

        if (!alphaPhip.coupled())
        {
            alphaPhip = phiBf[patchi]*alphaBf[patchi];
        }
    }
}


// ************************************************************************* //
