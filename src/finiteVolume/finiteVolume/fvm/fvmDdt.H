/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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
    Foam::fvm

Description
    Calculate the matrix for the first temporal derivative.

SourceFiles
    fvmDdt.C

\*---------------------------------------------------------------------------*/

#ifndef fvmDdt_H
#define fvmDdt_H

#include "fields/volFields/volFieldsFwd.H"
#include "fvMatrices/fvMatrix/fvMatrix.H"
#include "primitives/traits/one.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Namespace fvm functions Declaration
\*---------------------------------------------------------------------------*/

namespace fvm
{
    template<class Type>
    tmp<fvMatrix<Type>> ddt
    (
        const GeometricField<Type, fvPatchField, volMesh>&
    );

    template<class Type>
    tmp<fvMatrix<Type>> ddt
    (
        const Foam::one,
        const GeometricField<Type, fvPatchField, volMesh>&
    );

    template<class Type>
    tmp<fvMatrix<Type>> ddt
    (
        const dimensionedScalar&,
        const GeometricField<Type, fvPatchField, volMesh>&
    );

    template<class Type>
    tmp<fvMatrix<Type>> ddt
    (
        const volScalarField&,
        const GeometricField<Type, fvPatchField, volMesh>&
    );

    template<class Type>
    tmp<fvMatrix<Type>> ddt
    (
        const volScalarField&,
        const volScalarField&,
        const GeometricField<Type, fvPatchField, volMesh>&
    );

    template<class Type>
    tmp<fvMatrix<Type>> ddt
    (
        const Foam::one,
        const Foam::one,
        const GeometricField<Type, fvPatchField, volMesh>&
    );

    template<class Type>
    tmp<fvMatrix<Type>> ddt
    (
        const Foam::one,
        const volScalarField&,
        const GeometricField<Type, fvPatchField, volMesh>&
    );

    template<class Type>
    tmp<fvMatrix<Type>> ddt
    (
        const volScalarField&,
        const Foam::one,
        const GeometricField<Type, fvPatchField, volMesh>&
    );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "finiteVolume/fvm/fvmDdt.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
