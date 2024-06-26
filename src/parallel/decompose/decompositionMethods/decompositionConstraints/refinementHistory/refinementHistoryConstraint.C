/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2016 OpenFOAM Foundation
    Copyright (C) 2018 OpenCFD Ltd.
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

#include "decompositionConstraints/refinementHistory/refinementHistoryConstraint.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "meshes/polyMesh/syncTools/syncTools.H"
#include "polyTopoChange/polyTopoChange/hexRef8/refinementHistory.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace decompositionConstraints
{
    defineTypeName(refinementHistory);

    addToRunTimeSelectionTable
    (
        decompositionConstraint,
        refinementHistory,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::decompositionConstraints::refinementHistory::refinementHistory
(
    const dictionary& dict
)
:
    decompositionConstraint(dict, typeName)
{
    if (decompositionConstraint::debug)
    {
        Info<< type()
            << " : setting constraints to refinement history" << endl;
    }
}


Foam::decompositionConstraints::refinementHistory::refinementHistory()
:
    decompositionConstraint(dictionary(), typeName)
{
    if (decompositionConstraint::debug)
    {
        Info<< type()
            << " : setting constraints to refinement history" << endl;
    }
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::decompositionConstraints::refinementHistory::add
(
    const polyMesh& mesh,
    boolList& blockedFace,
    PtrList<labelList>& specifiedProcessorFaces,
    labelList& specifiedProcessor,
    List<labelPair>& explicitConnections
) const
{
    // The refinement history type
    typedef ::Foam::refinementHistory HistoryType;

    // Local storage if read from file
    autoPtr<const HistoryType> readFromFile;

    const HistoryType* historyPtr =
        mesh.findObject<HistoryType>("refinementHistory");

    if (historyPtr)
    {
        if (decompositionConstraint::debug)
        {
            Info<< type() << " : found refinementHistory" << endl;
        }
    }
    else
    {
        if (decompositionConstraint::debug)
        {
            Info<< type() << " : reading refinementHistory from time "
                << mesh.facesInstance() << endl;
        }

        readFromFile.reset
        (
            new HistoryType
            (
                IOobject
                (
                    "refinementHistory",
                    mesh.facesInstance(),
                    polyMesh::meshSubDir,
                    mesh,
                    IOobject::READ_IF_PRESENT,
                    IOobject::NO_WRITE
                ),
                mesh.nCells()
            )
        );

        historyPtr = readFromFile.get();  // get(), not release()
    }

    const auto& history = *historyPtr;

    if (history.active())
    {
        // refinementHistory itself implements decompositionConstraint
        history.add
        (
            blockedFace,
            specifiedProcessorFaces,
            specifiedProcessor,
            explicitConnections
        );
    }
}


void Foam::decompositionConstraints::refinementHistory::apply
(
    const polyMesh& mesh,
    const boolList& blockedFace,
    const PtrList<labelList>& specifiedProcessorFaces,
    const labelList& specifiedProcessor,
    const List<labelPair>& explicitConnections,
    labelList& decomposition
) const
{
    // The refinement history type
    typedef ::Foam::refinementHistory HistoryType;

    // Local storage if read from file
    autoPtr<const HistoryType> readFromFile;

    const HistoryType* historyPtr =
        mesh.findObject<HistoryType>("refinementHistory");

    if (!historyPtr)
    {
        readFromFile.reset
        (
            new HistoryType
            (
                IOobject
                (
                    "refinementHistory",
                    mesh.facesInstance(),
                    polyMesh::meshSubDir,
                    mesh,
                    IOobject::READ_IF_PRESENT,
                    IOobject::NO_WRITE
                ),
                mesh.nCells()
            )
        );

        historyPtr = readFromFile.get();  // get(), not release()
    }

    const auto& history = *historyPtr;

    if (history.active())
    {
        // refinementHistory itself implements decompositionConstraint
        history.apply
        (
            blockedFace,
            specifiedProcessorFaces,
            specifiedProcessor,
            explicitConnections,
            decomposition
        );
    }
}


// ************************************************************************* //
