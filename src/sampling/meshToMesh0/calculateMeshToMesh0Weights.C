/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "meshToMesh0/meshToMesh0.H"
#include "tetOverlapVolume/tetOverlapVolume.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::meshToMesh0::calculateInverseDistanceWeights() const
{
    DebugInFunction
        << "Calculating inverse distance weighting factors" << nl;

    if (inverseDistanceWeightsPtr_)
    {
        FatalErrorInFunction
            << "weighting factors already calculated"
            << exit(FatalError);
    }

    //- Initialise overlap volume to zero
    V_ = 0;

    inverseDistanceWeightsPtr_.reset(new scalarListList(toMesh_.nCells()));
    auto& invDistCoeffs = *inverseDistanceWeightsPtr_;

    // get reference to source mesh data
    const labelListList& cc = fromMesh_.cellCells();
    const vectorField& centreFrom = fromMesh_.C();
    const vectorField& centreTo = toMesh_.C();

    forAll(cellAddressing_, celli)
    {
        if (cellAddressing_[celli] != -1)
        {
            const vector& target = centreTo[celli];
            scalar m = mag(target - centreFrom[cellAddressing_[celli]]);

            const labelList& neighbours = cc[cellAddressing_[celli]];

            // if the nearest cell is a boundary cell or there is a direct hit,
            // pick up the value
            label directCelli = -1;
            if (m < directHitTol || neighbours.empty())
            {
                directCelli = celli;
            }
            else
            {
                forAll(neighbours, ni)
                {
                    scalar nm = mag(target - centreFrom[neighbours[ni]]);
                    if (nm < directHitTol)
                    {
                        directCelli = neighbours[ni];
                        break;
                    }
                }
            }


            if (directCelli != -1)
            {
                // Direct hit
                invDistCoeffs[directCelli].setSize(1);
                invDistCoeffs[directCelli][0] = 1.0;
                V_ += fromMesh_.V()[cellAddressing_[directCelli]];
            }
            else
            {
                invDistCoeffs[celli].setSize(neighbours.size() + 1);

                // The first coefficient corresponds to the centre cell.
                // The rest is ordered in the same way as the cellCells list.
                scalar invDist = 1.0/m;
                invDistCoeffs[celli][0] = invDist;
                scalar sumInvDist = invDist;

                // now add the neighbours
                forAll(neighbours, ni)
                {
                    invDist = 1.0/mag(target - centreFrom[neighbours[ni]]);
                    invDistCoeffs[celli][ni + 1] = invDist;
                    sumInvDist += invDist;
                }

                // divide by the total inverse-distance
                forAll(invDistCoeffs[celli], i)
                {
                    invDistCoeffs[celli][i] /= sumInvDist;
                }


                V_ +=
                    invDistCoeffs[celli][0]
                   *fromMesh_.V()[cellAddressing_[celli]];
                for (label i = 1; i < invDistCoeffs[celli].size(); i++)
                {
                    V_ +=
                        invDistCoeffs[celli][i]*fromMesh_.V()[neighbours[i-1]];
                }
            }
        }
    }
}


void Foam::meshToMesh0::calculateInverseVolumeWeights() const
{
    DebugInFunction
        << "Calculating inverse volume weighting factors" << endl;

    if (inverseVolumeWeightsPtr_)
    {
        FatalErrorInFunction
            << "weighting factors already calculated"
            << exit(FatalError);
    }

    //- Initialise overlap volume to zero
    V_ = 0;

    inverseVolumeWeightsPtr_.reset(new scalarListList(toMesh_.nCells()));
    auto& invVolCoeffs = *inverseVolumeWeightsPtr_;

    const labelListList& cellToCell = cellToCellAddressing();

    tetOverlapVolume overlapEngine;

    forAll(cellToCell, celli)
    {
        const labelList& overlapCells = cellToCell[celli];

        if (overlapCells.size() > 0)
        {
            invVolCoeffs[celli].setSize(overlapCells.size());

            forAll(overlapCells, j)
            {
                label cellFrom = overlapCells[j];
                treeBoundBox bbFromMesh
                (
                    pointField
                    (
                        fromMesh_.points(),
                        fromMesh_.cellPoints()[cellFrom]
                    )
                );

                scalar v = overlapEngine.cellCellOverlapVolumeMinDecomp
                (
                    toMesh_,
                    celli,

                    fromMesh_,
                    cellFrom,
                    bbFromMesh
                );
                invVolCoeffs[celli][j] = v/toMesh_.V()[celli];

                V_ += v;
            }
        }
    }
}


void Foam::meshToMesh0::calculateCellToCellAddressing() const
{
    DebugInFunction
        << "Calculating cell to cell addressing" << endl;

    if (cellToCellAddressingPtr_)
    {
        FatalErrorInFunction
            << "addressing already calculated"
            << exit(FatalError);
    }

    //- Initialise overlap volume to zero
    V_ = 0;

    tetOverlapVolume overlapEngine;

    cellToCellAddressingPtr_.reset(new labelListList(toMesh_.nCells()));
    auto& cellToCell = *cellToCellAddressingPtr_;


    forAll(cellToCell, iTo)
    {
        const labelList overLapCells =
            overlapEngine.overlappingCells(fromMesh_, toMesh_, iTo);
        if (overLapCells.size() > 0)
        {
            cellToCell[iTo].setSize(overLapCells.size());
            forAll(overLapCells, j)
            {
                cellToCell[iTo][j] = overLapCells[j];
                V_ += fromMesh_.V()[overLapCells[j]];
            }
        }
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

const Foam::scalarListList& Foam::meshToMesh0::inverseDistanceWeights() const
{
    if (!inverseDistanceWeightsPtr_)
    {
        calculateInverseDistanceWeights();
    }

    return *inverseDistanceWeightsPtr_;
}


const Foam::scalarListList& Foam::meshToMesh0::inverseVolumeWeights() const
{
    if (!inverseVolumeWeightsPtr_)
    {
        calculateInverseVolumeWeights();
    }

    return *inverseVolumeWeightsPtr_;
}


const Foam::labelListList& Foam::meshToMesh0::cellToCellAddressing() const
{
    if (!cellToCellAddressingPtr_)
    {
        calculateCellToCellAddressing();
    }

    return *cellToCellAddressingPtr_;
}


// ************************************************************************* //
