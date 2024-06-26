/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2019,2023 OpenCFD Ltd.
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

#include "matrices/lduMatrix/solvers/GAMG/interfaces/cyclicGAMGInterface/cyclicGAMGInterface.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "primitives/tuples/labelPairHashes.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(cyclicGAMGInterface, 0);
    addToRunTimeSelectionTable
    (
        GAMGInterface,
        cyclicGAMGInterface,
        lduInterface
    );
    addToRunTimeSelectionTable
    (
        GAMGInterface,
        cyclicGAMGInterface,
        Istream
    );


    // Add under name cyclicSlip
    addNamedToRunTimeSelectionTable
    (
        GAMGInterface,
        cyclicGAMGInterface,
        lduInterface,
        cyclicSlip
    );
    addNamedToRunTimeSelectionTable
    (
        GAMGInterface,
        cyclicGAMGInterface,
        Istream,
        cyclicSlip
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::cyclicGAMGInterface::cyclicGAMGInterface
(
    const label index,
    const lduInterfacePtrsList& coarseInterfaces,
    const lduInterface& fineInterface,
    const labelField& localRestrictAddressing,
    const labelField& neighbourRestrictAddressing,
    const label fineLevelIndex,
    const label coarseComm
)
:
    GAMGInterface(index, coarseInterfaces),
    neighbPatchID_
    (
        refCast<const cyclicLduInterface>(fineInterface).neighbPatchID()
    ),
    owner_(refCast<const cyclicLduInterface>(fineInterface).owner()),
    forwardT_(refCast<const cyclicLduInterface>(fineInterface).forwardT()),
    reverseT_(refCast<const cyclicLduInterface>(fineInterface).reverseT())
{
    // From coarse face to coarse cell
    DynamicList<label> dynFaceCells(localRestrictAddressing.size());
    // From fine face to coarse face
    DynamicList<label> dynFaceRestrictAddressing
    (
        localRestrictAddressing.size()
    );

    // From coarse cell pair to coarse face
    labelPairLookup cellsToCoarseFace(2*localRestrictAddressing.size());

    forAll(localRestrictAddressing, ffi)
    {
        labelPair cellPair;

        // Do switching on master/slave indexes based on the owner/neighbour of
        // the processor index such that both sides get the same answer.
        if (owner())
        {
            // Master side
            cellPair = labelPair
            (
                localRestrictAddressing[ffi],
                neighbourRestrictAddressing[ffi]
            );
        }
        else
        {
            // Slave side
            cellPair = labelPair
            (
                neighbourRestrictAddressing[ffi],
                localRestrictAddressing[ffi]
            );
        }

        const auto fnd = cellsToCoarseFace.cfind(cellPair);

        if (fnd.good())
        {
            // Already have coarse face
            dynFaceRestrictAddressing.append(fnd.val());
        }
        else
        {
            // New coarse face
            label coarseI = dynFaceCells.size();
            dynFaceRestrictAddressing.append(coarseI);
            dynFaceCells.append(localRestrictAddressing[ffi]);
            cellsToCoarseFace.insert(cellPair, coarseI);
        }
    }

    faceCells_.transfer(dynFaceCells);
    faceRestrictAddressing_.transfer(dynFaceRestrictAddressing);
}


Foam::cyclicGAMGInterface::cyclicGAMGInterface
(
    const label index,
    const lduInterfacePtrsList& coarseInterfaces,
    Istream& is
)
:
    GAMGInterface(index, coarseInterfaces, is),
    neighbPatchID_(readLabel(is)),
    owner_(readBool(is)),
    forwardT_(is),
    reverseT_(is)
{}


Foam::cyclicGAMGInterface::cyclicGAMGInterface
(
    const label index,
    const lduInterfacePtrsList& coarseInterfaces,
    const lduInterface& fineInterface,
    const labelList& interfaceMap,
    const labelUList& faceCells,
    const labelUList& faceRestrictAddresssing,
    const labelUList& faceOffsets,
    const lduInterfacePtrsList& allInterfaces
)
:
    GAMGInterface
    (
        index,
        coarseInterfaces,
        faceCells,
        faceRestrictAddresssing
    ),
    neighbPatchID_
    (
        interfaceMap.find
        (
            refCast<const cyclicLduInterface>(fineInterface).neighbPatchID()
        )
    ),
    owner_(refCast<const cyclicLduInterface>(fineInterface).owner()),
    forwardT_(refCast<const cyclicLduInterface>(fineInterface).forwardT()),
    reverseT_(refCast<const cyclicLduInterface>(fineInterface).reverseT())
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::labelField> Foam::cyclicGAMGInterface::internalFieldTransfer
(
    const Pstream::commsTypes commsType,
    const labelUList& iF
) const
{
    const cyclicGAMGInterface& nbr = neighbPatch();
    const labelUList& nbrFaceCells = nbr.faceCells();

    auto tpnf = tmp<labelField>::New(size());
    auto& pnf = tpnf.ref();

    forAll(pnf, facei)
    {
        pnf[facei] = iF[nbrFaceCells[facei]];
    }

    return tpnf;
}


void Foam::cyclicGAMGInterface::write(Ostream& os) const
{
    GAMGInterface::write(os);
    os  << token::SPACE << neighbPatchID_
        << token::SPACE << owner_
        << token::SPACE << forwardT_
        << token::SPACE << reverseT_;
}


// ************************************************************************* //
