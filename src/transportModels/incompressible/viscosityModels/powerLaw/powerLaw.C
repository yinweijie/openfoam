/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2017 OpenCFD Ltd
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

#include "viscosityModels/powerLaw/powerLaw.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "fields/surfaceFields/surfaceFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace viscosityModels
{
    defineTypeNameAndDebug(powerLaw, 0);

    addToRunTimeSelectionTable
    (
        viscosityModel,
        powerLaw,
        dictionary
    );
}
}


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * * //

Foam::tmp<Foam::volScalarField>
Foam::viscosityModels::powerLaw::calcNu() const
{
    return max
    (
        nuMin_,
        min
        (
            nuMax_,
            k_*pow
            (
                max
                (
                    dimensionedScalar("one", dimTime, 1.0)*strainRate(),
                    dimensionedScalar("SMALL", dimless, SMALL)
                ),
                n_.value() - scalar(1)
            )
        )
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::viscosityModels::powerLaw::powerLaw
(
    const word& name,
    const dictionary& viscosityProperties,
    const volVectorField& U,
    const surfaceScalarField& phi
)
:
    viscosityModel(name, viscosityProperties, U, phi),
    powerLawCoeffs_(viscosityProperties.optionalSubDict(typeName + "Coeffs")),
    k_("k", dimViscosity, powerLawCoeffs_),
    n_("n", dimless, powerLawCoeffs_),
    nuMin_("nuMin", dimViscosity, powerLawCoeffs_),
    nuMax_("nuMax", dimViscosity, powerLawCoeffs_),
    nu_
    (
        IOobject
        (
            name,
            U_.time().timeName(),
            U_.db(),
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        calcNu()
    )
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

bool Foam::viscosityModels::powerLaw::read
(
    const dictionary& viscosityProperties
)
{
    viscosityModel::read(viscosityProperties);

    powerLawCoeffs_ = viscosityProperties.optionalSubDict(typeName + "Coeffs");

    powerLawCoeffs_.readEntry("k", k_);
    powerLawCoeffs_.readEntry("n", n_);
    powerLawCoeffs_.readEntry("nuMin", nuMin_);
    powerLawCoeffs_.readEntry("nuMax", nuMax_);

    return true;
}


// ************************************************************************* //
