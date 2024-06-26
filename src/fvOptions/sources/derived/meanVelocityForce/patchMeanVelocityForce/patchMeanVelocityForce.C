/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2016 OpenFOAM Foundation
    Copyright (C) 2022 OpenCFD Ltd.
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

#include "sources/derived/meanVelocityForce/patchMeanVelocityForce/patchMeanVelocityForce.H"
#include "meshes/polyMesh/polyPatches/constraint/processorCyclic/processorCyclicPolyPatch.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
    defineTypeNameAndDebug(patchMeanVelocityForce, 0);
    addToRunTimeSelectionTable(option, patchMeanVelocityForce, dictionary);
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::patchMeanVelocityForce::patchMeanVelocityForce
(
    const word& sourceName,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    meanVelocityForce(sourceName, modelType, dict, mesh),
    patch_(coeffs_.get<word>("patch")),
    patchi_(mesh.boundaryMesh().findPatchID(patch_))
{
    if (patchi_ < 0)
    {
        FatalIOErrorInFunction(dict)
            << "Cannot find patch " << patch_
            << exit(FatalIOError);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::scalar Foam::fv::patchMeanVelocityForce::magUbarAve
(
    const volVectorField& U
) const
{
    FixedList<scalar, 2> sumAmagUsumA(Zero);

    sumAmagUsumA[0] +=
        sum
        (
            (flowDir_ & U.boundaryField()[patchi_])
          * mesh_.boundary()[patchi_].magSf()
        );
    sumAmagUsumA[1] += sum(mesh_.boundary()[patchi_].magSf());


    // If the mean velocity force is applied to a cyclic patch
    // for parallel runs include contributions from processorCyclic patches
    // generated from the decomposition of the cyclic patch
    const polyBoundaryMesh& patches = mesh_.boundaryMesh();

    if (Pstream::parRun() && isA<cyclicPolyPatch>(patches[patchi_]))
    {
        for
        (
            const label patchi
          : processorCyclicPolyPatch::patchIDs(patch_, patches)
        )
        {
            sumAmagUsumA[0] +=
                sum
                (
                    (flowDir_ & U.boundaryField()[patchi])
                  * mesh_.boundary()[patchi].magSf()
                );
            sumAmagUsumA[1] += sum(mesh_.boundary()[patchi].magSf());
        }
    }

    mesh_.reduce(sumAmagUsumA, sumOp<scalar>());

    return
    (
        sumAmagUsumA[0]
      / stabilise(sumAmagUsumA[1], VSMALL)
    );
}


// ************************************************************************* //
