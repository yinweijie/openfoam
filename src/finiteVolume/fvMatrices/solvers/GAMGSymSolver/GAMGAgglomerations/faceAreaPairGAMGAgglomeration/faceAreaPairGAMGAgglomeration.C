/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2023 OpenCFD Ltd.
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

#include "fvMatrices/solvers/GAMGSymSolver/GAMGAgglomerations/faceAreaPairGAMGAgglomeration/faceAreaPairGAMGAgglomeration.H"
#include "fvMesh/fvMesh.H"
#include "fields/surfaceFields/surfaceFields.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(faceAreaPairGAMGAgglomeration, 0);

    addToRunTimeSelectionTable
    (
        GAMGAgglomeration,
        faceAreaPairGAMGAgglomeration,
        lduMesh
    );

    addToRunTimeSelectionTable
    (
        GAMGAgglomeration,
        faceAreaPairGAMGAgglomeration,
        geometry
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::faceAreaPairGAMGAgglomeration::faceAreaPairGAMGAgglomeration
(
    const lduMesh& mesh,
    const dictionary& controlDict
)
:
    pairGAMGAgglomeration(mesh, controlDict)
{
    const fvMesh& fvmesh = refCast<const fvMesh>(mesh);

    //agglomerate(mesh, sqrt(fvmesh.magSf().primitiveField()));
    agglomerate
    (
        nCellsInCoarsestLevel_,
        0,          //mesh,
        mag
        (
            cmptMultiply
            (
                fvmesh.Sf().primitiveField()
               /sqrt(fvmesh.magSf().primitiveField()),
                vector(1, 1.01, 1.02)
                //vector::one
            )
        ),
        true
    );
}


Foam::faceAreaPairGAMGAgglomeration::faceAreaPairGAMGAgglomeration
(
    const lduMesh& mesh,
    const scalarField& cellVolumes,
    const vectorField& faceAreas,
    const dictionary& controlDict
)
:
    pairGAMGAgglomeration(mesh, controlDict)
{
    //agglomerate(mesh, sqrt(mag(faceAreas)));
    agglomerate
    (
        nCellsInCoarsestLevel_,
        0,          //mesh,
        mag
        (
            cmptMultiply
            (
                faceAreas
               /sqrt(mag(faceAreas)),
                vector(1, 1.01, 1.02)
                //vector::one
            )
        ),
        true
    );
}


// ************************************************************************* //
