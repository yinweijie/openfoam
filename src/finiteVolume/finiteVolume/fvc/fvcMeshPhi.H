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

InNamespace
    Foam::fvc

Description
    Calculate the mesh motion flux and convert fluxes from absolute to relative
    and back.

SourceFiles
    fvcMeshPhi.C

\*---------------------------------------------------------------------------*/

#ifndef fvcMeshPhi_H
#define fvcMeshPhi_H

#include "fields/volFields/volFieldsFwd.H"
#include "fields/surfaceFields/surfaceFieldsFwd.H"
#include "dimensionedTypes/dimensionedTypes.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Namespace fvc functions Declaration
\*---------------------------------------------------------------------------*/

namespace fvc
{
    tmp<surfaceScalarField> meshPhi
    (
        const volVectorField& U
    );

    tmp<surfaceScalarField> meshPhi
    (
        const dimensionedScalar& rho,
        const volVectorField& U
    );

    tmp<surfaceScalarField> meshPhi
    (
        const volScalarField& rho,
        const volVectorField& U
    );


    //- Make the given flux relative
    void makeRelative
    (
        surfaceScalarField& phi,
        const volVectorField& U
    );

    //- Make the given flux relative
    void makeRelative
    (
        surfaceScalarField& phi,
        const dimensionedScalar& rho,
        const volVectorField& U
    );

    //- Make the given flux relative
    void makeRelative
    (
        surfaceScalarField& phi,
        const volScalarField& rho,
        const volVectorField& U
    );


    //- Make the given flux absolute
    void makeAbsolute
    (
        surfaceScalarField& phi,
        const volVectorField& U
    );

    //- Make the given flux absolute
    void makeAbsolute
    (
        surfaceScalarField& phi,
        const dimensionedScalar& rho,
        const volVectorField& U
    );

    //- Make the given flux absolute
    void makeAbsolute
    (
        surfaceScalarField& phi,
        const volScalarField& rho,
        const volVectorField& U
    );


    //- Return the given absolute flux in relative form
    tmp<surfaceScalarField> relative
    (
        const tmp<surfaceScalarField>& tphi,
        const volVectorField& U
    );

    //- Return the given absolute flux in relative form
    tmp<surfaceScalarField> relative
    (
        const tmp<surfaceScalarField>& tphi,
        const volScalarField& rho,
        const volVectorField& U
    );


    //- Return the given relative flux in absolute form
    tmp<surfaceScalarField> absolute
    (
        const tmp<surfaceScalarField>& tphi,
        const volVectorField& U
    );

    //- Return the given relative flux in absolute form
    tmp<surfaceScalarField> absolute
    (
        const tmp<surfaceScalarField>& tphi,
        const volScalarField& rho,
        const volVectorField& U
    );

    void correctUf
    (
        autoPtr<surfaceVectorField>& Uf,
        const volVectorField& U,
        const surfaceScalarField& phi
    );

    void correctRhoUf
    (
        autoPtr<surfaceVectorField>& rhoUf,
        const volScalarField& rho,
        const volVectorField& U,
        const surfaceScalarField& phi
    );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
