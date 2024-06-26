/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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

#include "submodels/kinematic/filmTurbulenceModel/laminar/laminar.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "fvMesh/fvMesh.H"
#include "fvMatrices/fvMatrices.H"
#include "db/Time/TimeOpenFOAM.H"
#include "fields/volFields/volFields.H"
#include "finiteVolume/fvm/fvmSup.H"
#include "kinematicSingleLayer/kinematicSingleLayer.H"
#include "fields/fvPatchFields/basic/extrapolatedCalculated/extrapolatedCalculatedFvPatchFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{
namespace surfaceFilmModels
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(laminar, 0);
addToRunTimeSelectionTable(filmTurbulenceModel, laminar, dictionary);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

laminar::laminar
(
    surfaceFilmRegionModel& film,
    const dictionary& dict
)
:
    filmTurbulenceModel(type(), film, dict),
    Cf_(coeffDict_.get<scalar>("Cf"))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

laminar::~laminar()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

tmp<volVectorField> laminar::Us() const
{
    tmp<volVectorField> tUs
    (
        new volVectorField
        (
            IOobject
            (
                typeName + ":Us",
                filmModel_.regionMesh().time().timeName(),
                filmModel_.regionMesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            filmModel_.regionMesh(),
            dimensionedVector(dimVelocity, Zero),
            fvPatchFieldBase::extrapolatedCalculatedType()
        )
    );

    // apply quadratic profile
    tUs.ref() = Foam::sqrt(2.0)*filmModel_.U();
    tUs.ref().correctBoundaryConditions();

    return tUs;
}


tmp<volScalarField> laminar::mut() const
{
    return tmp<volScalarField>::New
    (
        IOobject
        (
            typeName + ":mut",
            filmModel_.regionMesh().time().timeName(),
            filmModel_.regionMesh(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        filmModel_.regionMesh(),
        dimensionedScalar(dimMass/dimLength/dimTime, Zero)
    );
}


void laminar::correct()
{}


tmp<fvVectorMatrix> laminar::Su(volVectorField& U) const
{
    // local reference to film model
    const kinematicSingleLayer& film =
        static_cast<const kinematicSingleLayer&>(filmModel_);

    // local references to film fields
    const volScalarField& mu = film.mu();
    const volVectorField& Uw = film.Uw();
    const volScalarField& delta = film.delta();
    const volVectorField& Up = film.UPrimary();
    const volScalarField& rhop = film.rhoPrimary();

    // employ simple coeff-based model
    volScalarField Cs("Cs", Cf_*rhop*mag(Up - U));
    volScalarField Cw("Cw", mu/((1.0/3.0)*(delta + film.deltaSmall())));
    Cw.clamp_max(5000.0);

    return
    (
       - fvm::Sp(Cs, U) + Cs*Up // surface contribution
       - fvm::Sp(Cw, U) + Cw*Uw // wall contribution
    );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceFilmModels
} // End namespace regionModels
} // End namespace Foam

// ************************************************************************* //
