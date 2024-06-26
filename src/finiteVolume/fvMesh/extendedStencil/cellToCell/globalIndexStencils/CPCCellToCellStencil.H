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

Class
    Foam::CPCCellToCellStencil

Description

SourceFiles
    CPCCellToCellStencil.C

\*---------------------------------------------------------------------------*/

#ifndef CPCCellToCellStencil_H
#define CPCCellToCellStencil_H

#include "fvMesh/extendedStencil/cellToCell/globalIndexStencils/cellToCellStencil.H"
#include "primitives/bools/lists/boolList.H"
#include "containers/HashTables/HashSet/HashSet.H"
#include "containers/HashTables/Map/Map.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class CPCCellToCellStencil Declaration
\*---------------------------------------------------------------------------*/

class CPCCellToCellStencil
:
    public cellToCellStencil
{
    // Private Member Functions

        //- Calculates per point the neighbour data (= pointCells)
        void calcPointBoundaryData
        (
            const boolList& isValidBFace,
            const labelList& boundaryPoints,
            Map<labelList>& neiGlobal
        ) const;

        //- Calculates per cell the neighbour data
        //  (= cell or boundary in global numbering).
        //  First element is always cell itself!
        void calcCellStencil(labelListList& globalCellCells) const;


public:

    // Constructors

        //- Construct from all cells and boundary faces
        explicit CPCCellToCellStencil(const polyMesh&);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
