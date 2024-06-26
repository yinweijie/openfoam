/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2019-2023 OpenCFD Ltd.
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

#include "meshes/polyMesh/polyPatches/constraint/processorCyclic/processorCyclicPolyPatch.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "fields/Fields/Field/SubField.H"
#include "meshes/polyMesh/polyPatches/constraint/cyclic/cyclicPolyPatch.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(processorCyclicPolyPatch, 0);
    addToRunTimeSelectionTable(polyPatch, processorCyclicPolyPatch, dictionary);
}


// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

Foam::word Foam::processorCyclicPolyPatch::newName
(
    const word& cyclicPolyPatchName,
    const label myProcNo,
    const label neighbProcNo
)
{
    return word
    (
        processorPolyPatch::newName(myProcNo, neighbProcNo)
      + "through"
      + cyclicPolyPatchName
    );
}


Foam::labelList Foam::processorCyclicPolyPatch::patchIDs
(
    const word& cyclicPolyPatchName,
    const polyBoundaryMesh& bm
)
{
    return bm.indices
    (
        wordRe
        (
            "procBoundary.*to.*through" + cyclicPolyPatchName,
            wordRe::REGEX
        )
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::processorCyclicPolyPatch::processorCyclicPolyPatch
(
    const label size,
    const label start,
    const label index,
    const polyBoundaryMesh& bm,
    const int myProcNo,
    const int neighbProcNo,
    const word& referPatchName,
    const transformType transform,
    const word& patchType
)
:
    processorPolyPatch
    (
        newName(referPatchName, myProcNo, neighbProcNo),
        size,
        start,
        index,
        bm,
        myProcNo,
        neighbProcNo,
        transform,
        patchType
    ),
    referPatchName_(referPatchName),
    tag_(-1),
    referPatchID_(-1)
{}


Foam::processorCyclicPolyPatch::processorCyclicPolyPatch
(
    const word& name,
    const dictionary& dict,
    const label index,
    const polyBoundaryMesh& bm,
    const word& patchType
)
:
    processorPolyPatch(name, dict, index, bm, patchType),
    referPatchName_(dict.lookup("referPatch")),
    tag_(dict.getOrDefault<int>("tag", -1)),
    referPatchID_(-1)
{}


Foam::processorCyclicPolyPatch::processorCyclicPolyPatch
(
    const processorCyclicPolyPatch& pp,
    const polyBoundaryMesh& bm
)
:
    processorPolyPatch(pp, bm),
    referPatchName_(pp.referPatchName()),
    tag_(pp.tag()),
    referPatchID_(-1)
{}


Foam::processorCyclicPolyPatch::processorCyclicPolyPatch
(
    const processorCyclicPolyPatch& pp,
    const polyBoundaryMesh& bm,
    const label index,
    const label newSize,
    const label newStart
)
:
    processorPolyPatch(pp, bm, index, newSize, newStart),
    referPatchName_(pp.referPatchName_),
    tag_(pp.tag()),
    referPatchID_(-1)
{}


Foam::processorCyclicPolyPatch::processorCyclicPolyPatch
(
    const processorCyclicPolyPatch& pp,
    const polyBoundaryMesh& bm,
    const label index,
    const label newSize,
    const label newStart,
    const word& referPatchName
)
:
    processorPolyPatch(pp, bm, index, newSize, newStart),
    referPatchName_(referPatchName),
    tag_(-1),
    referPatchID_(-1)
{}


Foam::processorCyclicPolyPatch::processorCyclicPolyPatch
(
    const processorCyclicPolyPatch& pp,
    const polyBoundaryMesh& bm,
    const label index,
    const labelUList& mapAddressing,
    const label newStart
)
:
    processorPolyPatch(pp, bm, index, mapAddressing, newStart),
    referPatchName_(pp.referPatchName()),
    tag_(-1),
    referPatchID_(-1)
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::label Foam::processorCyclicPolyPatch::referPatchID() const
{
    if (referPatchID_ == -1)
    {
        referPatchID_ = this->boundaryMesh().findPatchID
        (
            referPatchName_
        );

        if (referPatchID_ == -1)
        {
            FatalErrorInFunction
                << "Illegal referPatch name " << referPatchName_ << nl
                << "Valid patch names: "
                << this->boundaryMesh().names() << nl
                << exit(FatalError);
        }
    }

    return referPatchID_;
}


int Foam::processorCyclicPolyPatch::tag() const
{
    if (tag_ == -1)
    {
        // Get unique tag to use for all comms. Make sure that both sides
        // use the same tag
        const cyclicPolyPatch& cycPatch = refCast<const cyclicPolyPatch>
        (
            referPatch()
        );

        if (owner())
        {
            tag_ = string::hasher()(cycPatch.name()) % 32768u;
        }
        else
        {
            tag_ = string::hasher()(cycPatch.neighbPatch().name()) % 32768u;
        }

        if (tag_ == UPstream::msgType() || tag_ == -1)
        {
            FatalErrorInFunction
                << "Tag calculated from cyclic patch name " << tag_
                << " is the same as the current message type "
                << UPstream::msgType() << " or -1" << nl
                << "Please set a non-conflicting, unique, tag by hand"
                << " using the 'tag' entry"
                << exit(FatalError);
        }
        if (debug)
        {
            Pout<< "processorCyclicPolyPatch " << name() << " uses tag " << tag_
                << endl;
        }
    }
    return tag_;
}


void Foam::processorCyclicPolyPatch::initGeometry(PstreamBuffers& pBufs)
{
    // Send over processorPolyPatch data
    processorPolyPatch::initGeometry(pBufs);
}


void Foam::processorCyclicPolyPatch::calcGeometry(PstreamBuffers& pBufs)
{
    // Receive and initialise processorPolyPatch data
    processorPolyPatch::calcGeometry(pBufs);

    if (UPstream::parRun())
    {
        // Where do we store the calculated transformation?
        // - on the processor patch?
        // - on the underlying cyclic patch?
        // - or do we not auto-calculate the transformation but
        //   have option of reading it.

        // Update underlying cyclic halves. Need to do both since only one
        // half might be present as a processorCyclic.
        coupledPolyPatch& pp = const_cast<coupledPolyPatch&>(referPatch());
        pp.calcGeometry
        (
            *this,
            faceCentres(),
            faceAreas(),
            faceCellCentres(),
            neighbFaceCentres(),
            neighbFaceAreas(),
            neighbFaceCellCentres()
        );

        if (isA<cyclicPolyPatch>(pp))
        {
            const cyclicPolyPatch& cpp = refCast<const cyclicPolyPatch>(pp);
            const_cast<cyclicPolyPatch&>(cpp.neighbPatch()).calcGeometry
            (
                *this,
                neighbFaceCentres(),
                neighbFaceAreas(),
                neighbFaceCellCentres(),
                faceCentres(),
                faceAreas(),
                faceCellCentres()
            );
        }
    }
}


void Foam::processorCyclicPolyPatch::initMovePoints
(
    PstreamBuffers& pBufs,
    const pointField& p
)
{
    // Recalculate geometry
    initGeometry(pBufs);
}


void Foam::processorCyclicPolyPatch::movePoints
(
    PstreamBuffers& pBufs,
    const pointField&
)
{
    calcGeometry(pBufs);
}


void Foam::processorCyclicPolyPatch::initUpdateMesh(PstreamBuffers& pBufs)
{
    processorPolyPatch::initUpdateMesh(pBufs);
}


void Foam::processorCyclicPolyPatch::updateMesh(PstreamBuffers& pBufs)
{
     referPatchID_ = -1;
     processorPolyPatch::updateMesh(pBufs);
}


void Foam::processorCyclicPolyPatch::initOrder
(
    PstreamBuffers& pBufs,
    const primitivePatch& pp
) const
{
    // Send the patch points and faces across. Note that this is exactly the
    // same as the processorPolyPatch::initOrder in COINCIDENTFULLMATCH
    // mode.
    UOPstream toNeighbour(neighbProcNo(), pBufs);
    toNeighbour << pp.localPoints()
                << pp.localFaces();
}


bool Foam::processorCyclicPolyPatch::order
(
    PstreamBuffers& pBufs,
    const primitivePatch& pp,
    labelList& faceMap,
    labelList& rotation
) const
{
    // Receive the remote patch
    vectorField masterPts;
    faceList masterFaces;
    autoPtr<primitivePatch> masterPtr;
    {
        UIPstream fromNeighbour(neighbProcNo(), pBufs);
        fromNeighbour >> masterPts >> masterFaces;
        masterPtr.reset
        (
            new primitivePatch(SubList<face>(masterFaces), masterPts)
        );
    }

    const cyclicPolyPatch& cycPatch =
        refCast<const cyclicPolyPatch>(referPatch());

    // (ab)use the cyclicPolyPatch ordering:
    //  - owner side stores geometry
    //  - neighbour side does ordering according to owner side
    cycPatch.neighbPatch().initOrder(pBufs, masterPtr());

    return cycPatch.order(pBufs, pp, faceMap, rotation);
}


void Foam::processorCyclicPolyPatch::write(Ostream& os) const
{
    processorPolyPatch::write(os);
    os.writeEntry("referPatch", referPatchName_);
    os.writeEntryIfDifferent<label>("tag", -1, tag_);
}


// ************************************************************************* //
