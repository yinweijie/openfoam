/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2015 OpenFOAM Foundation
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

#include "diameterModels/IATE/IATE.H"
#include "diameterModels/IATE/IATEsources/IATEsource/IATEsource.H"
#include "twoPhaseSystem/twoPhaseSystem.H"
#include "finiteVolume/fvm/fvmDdt.H"
#include "finiteVolume/fvm/fvmDiv.H"
#include "finiteVolume/fvm/fvmSup.H"
#include "finiteVolume/fvc/fvcDdt.H"
#include "finiteVolume/fvc/fvcDiv.H"
#include "finiteVolume/fvc/fvcAverage.H"
#include "cfdTools/general/fvOptions/fvOptions.H"
#include "global/constants/mathematical/mathematicalConstants.H"
#include "global/constants/fundamental/fundamentalConstants.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace diameterModels
{
    defineTypeNameAndDebug(IATE, 0);

    addToRunTimeSelectionTable
    (
        diameterModel,
        IATE,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::diameterModels::IATE::IATE
(
    const dictionary& diameterProperties,
    const phaseModel& phase
)
:
    diameterModel(diameterProperties, phase),
    kappai_
    (
        IOobject
        (
            IOobject::groupName("kappai", phase.name()),
            phase_.U().time().timeName(),
            phase_.U().mesh(),
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        phase_.U().mesh()
    ),
    dMax_("dMax", dimLength, diameterProperties_),
    dMin_("dMin", dimLength, diameterProperties_),
    residualAlpha_("residualAlpha", dimless, diameterProperties_),
    d_
    (
        IOobject
        (
            IOobject::groupName("d", phase.name()),
            phase_.U().time().timeName(),
            phase_.U().mesh(),
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        dsm()
    ),
    sources_
    (
        diameterProperties_.lookup("sources"),
        IATEsource::iNew(*this)
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::diameterModels::IATE::~IATE()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField> Foam::diameterModels::IATE::dsm() const
{
    return max(6/max(kappai_, 6/dMax_), dMin_);
}

// Placeholder for the nucleation/condensation model
// Foam::tmp<Foam::volScalarField> Foam::diameterModels::IATE::Rph() const
// {
//     const volScalarField& T = phase_.thermo().T();
//     const volScalarField& p = phase_.thermo().p();
//
//     scalar A, B, C, sigma, vm, Rph;
//
//     volScalarField ps(1e5*pow(10, A - B/(T + C)));
//     volScalarField Dbc
//     (
//         4*sigma*vm/(constant::physicoChemical::k*T*log(p/ps))
//     );
//
//     return constant::mathematical::pi*sqr(Dbc)*Rph;
// }

void Foam::diameterModels::IATE::correct()
{
    // Initialise the accumulated source term to the dilatation effect
    volScalarField R
    (
        (
            (1.0/3.0)
           /max
            (
                fvc::average(phase_ + phase_.oldTime()),
                residualAlpha_
            )
        )
       *(fvc::ddt(phase_) + fvc::div(phase_.alphaPhi()))
    );

    // Accumulate the run-time selectable sources
    forAll(sources_, j)
    {
        R -= sources_[j].R();
    }

    fv::options& fvOptions(fv::options::New(phase_.mesh()));

    // Construct the interfacial curvature equation
    fvScalarMatrix kappaiEqn
    (
        fvm::ddt(kappai_) + fvm::div(phase_.phi(), kappai_)
      - fvm::Sp(fvc::div(phase_.phi()), kappai_)
     ==
      - fvm::SuSp(R, kappai_)
    //+ Rph() // Omit the nucleation/condensation term
      + fvOptions(kappai_)
    );

    kappaiEqn.relax();

    fvOptions.constrain(kappaiEqn);

    kappaiEqn.solve();

    // Update the Sauter-mean diameter
    d_ = dsm();
}


bool Foam::diameterModels::IATE::read(const dictionary& phaseProperties)
{
    diameterModel::read(phaseProperties);

    diameterProperties_.readEntry("dMax", dMax_);
    diameterProperties_.readEntry("dMin", dMin_);

    // Re-create all the sources updating number, type and coefficients
    PtrList<IATEsource>
    (
        diameterProperties_.lookup("sources"),
        IATEsource::iNew(*this)
    ).transfer(sources_);

    return true;
}


// ************************************************************************* //
