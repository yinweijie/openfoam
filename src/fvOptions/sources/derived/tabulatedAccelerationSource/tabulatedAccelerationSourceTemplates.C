/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015 OpenFOAM Foundation
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

#include "sources/derived/tabulatedAccelerationSource/tabulatedAccelerationSource.H"
#include "fvMesh/fvMesh.H"
#include "fvMatrices/fvMatrices.H"
#include "fields/UniformDimensionedFields/uniformDimensionedFields.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class RhoFieldType>
void Foam::fv::tabulatedAccelerationSource::addSup
(
    const RhoFieldType& rho,
    fvMatrix<vector>& eqn,
    const label fieldi
)
{
    Vector<vector> acceleration(motion_.acceleration());

    // If gravitational force is present combine with the linear acceleration
    if (mesh_.time().foundObject<uniformDimensionedVectorField>("g"))
    {
        auto& g =
            mesh_.time().lookupObjectRef<uniformDimensionedVectorField>("g");

        const auto& hRef =
            mesh_.lookupObject<uniformDimensionedScalarField>("hRef");

        g = g0_ - dimensionedVector("a", dimAcceleration, acceleration.x());

        dimensionedScalar ghRef
        (
            mag(g.value()) > SMALL
          ? g & (cmptMag(g.value())/mag(g.value()))*hRef
          : dimensionedScalar("ghRef", g.dimensions()*dimLength, 0)
        );

        mesh_.lookupObjectRef<volScalarField>("gh")
            = (g & mesh_.C()) - ghRef;

        mesh_.lookupObjectRef<surfaceScalarField>("ghf")
            = (g & mesh_.Cf()) - ghRef;
    }
    // ... otherwise include explicitly in the momentum equation
    else
    {
        eqn -= rho*dimensionedVector("a", dimAcceleration, acceleration.x());
    }

    dimensionedVector Omega
    (
        "Omega",
        dimensionSet(0, 0, -1, 0, 0),
        acceleration.y()
    );

    dimensionedVector dOmegaDT
    (
        "dOmegaDT",
        dimensionSet(0, 0, -2, 0, 0),
        acceleration.z()
    );

    eqn -=
    (
        rho*(2*Omega ^ eqn.psi())         // Coriolis force
      + rho*(Omega ^ (Omega ^ mesh_.C())) // Centrifugal force
      + rho*(dOmegaDT ^ mesh_.C())        // Angular tabulatedAcceleration force
    );
}


// ************************************************************************* //
