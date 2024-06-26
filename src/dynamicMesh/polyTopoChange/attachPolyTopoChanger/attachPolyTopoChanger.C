/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "polyTopoChange/attachPolyTopoChanger/attachPolyTopoChanger.H"
#include "meshes/polyMesh/polyMesh.H"
#include "polyTopoChange/polyTopoChange.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::attachPolyTopoChanger::attachPolyTopoChanger
(
    const IOobject& io,
    polyMesh& mesh
)
:
    polyTopoChanger(io, mesh)
{}


Foam::attachPolyTopoChanger::attachPolyTopoChanger
(
    polyMesh& mesh
)
:
    polyTopoChanger(mesh)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::attachPolyTopoChanger::attach(const bool removeEmptyPatches)
{
    if (debug)
    {
        Pout<< "void attachPolyTopoChanger::attach(): "
            << "Attaching mesh" << endl;
    }

    // Save current file instance
    const fileName oldInst = mesh_.facesInstance();

    // Execute all polyMeshModifiers
    changeMesh(false);  // no inflation

    const pointField p = mesh_.oldPoints();

    mesh_.movePoints(p);

    if (debug)
    {
        Pout<< "Clearing mesh." << endl;
    }

    if (removeEmptyPatches)
    {
        // Re-do the boundary patches, removing the ones with zero size
        const polyBoundaryMesh& oldPatches = mesh_.boundaryMesh();

        polyPatchList newPatches(oldPatches.size());
        label nNewPatches = 0;

        forAll(oldPatches, patchi)
        {
            const word& patchName = oldPatches[patchi].name();

            if (returnReduceOr(oldPatches[patchi].size()))
            {
                newPatches.set
                (
                    nNewPatches,
                    oldPatches[patchi].clone
                    (
                        mesh_.boundaryMesh(),
                        nNewPatches,
                        oldPatches[patchi].size(),
                        oldPatches[patchi].start()
                    )
                );

                ++nNewPatches;
            }
            else
            {
                if (debug)
                {
                    Pout<< "Removing zero-sized patch " << patchi
                        << " named " << patchName << endl;
                }
            }
        }

        newPatches.resize(nNewPatches);

        mesh_.removeBoundary();
        mesh_.addPatches(newPatches);
    }

    // Reset the file instance to overwrite the original mesh
    mesh_.setInstance(oldInst);

    if (debug)
    {
        Pout<< "void attachPolyTopoChanger::attach(): "
            << "Finished attaching mesh" << endl;
    }

    mesh_.checkMesh();
}


// ************************************************************************* //
