/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2017 OpenFOAM Foundation
    Copyright (C) 2019-2022 OpenCFD Ltd.
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

#include "globalIndex.H"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

template<class SubListType>
Foam::labelList
Foam::globalIndex::calcListOffsets
(
    const List<SubListType>& lists,
    const bool checkOverflow
)
{
    labelList values;

    const label len = lists.size();

    if (len)
    {
        values.resize(len+1);

        label start = 0;
        for (label i = 0; i < len; ++i)
        {
            values[i] = start;
            start += lists[i].size();

            if (checkOverflow && start < values[i])
            {
                reportOverflowAndExit(i);
            }
        }
        values[len] = start;
    }

    return values;
}


template<class ProcIDsContainer, class Type>
void Foam::globalIndex::gather
(
    const labelUList& off,  // needed on master only
    const label comm,
    const ProcIDsContainer& procIDs,
    const UList<Type>& fld,
    List<Type>& allFld,
    const int tag,
    const Pstream::commsTypes preferredCommsType
)
{
    // low-level: no parRun guard

    // Automatically change from nonBlocking to scheduled for
    // non-contiguous data.
    const UPstream::commsTypes commsType =
    (
        (
            !is_contiguous<Type>::value
         && UPstream::commsTypes::nonBlocking == preferredCommsType
        )
      ? UPstream::commsTypes::scheduled
      : preferredCommsType
    );

    const label startOfRequests = UPstream::nRequests();

    if (Pstream::myProcNo(comm) == procIDs[0])
    {
        allFld.resize_nocopy(off.last());  // == totalSize()

        // Assign my local data - respect offset information
        // so that we can request 0 entries to be copied.
        // Also handle the case where we have a slice of the full
        // list.

        SubList<Type>(allFld, off[1]-off[0], off[0]) =
           SubList<Type>(fld, off[1]-off[0]);

        for (label i = 1; i < procIDs.size(); ++i)
        {
            SubList<Type> procSlot(allFld, off[i+1]-off[i], off[i]);

            if (procSlot.empty())
            {
                // Nothing to do
            }
            else if (is_contiguous<Type>::value)
            {
                IPstream::read
                (
                    commsType,
                    procIDs[i],
                    procSlot.data_bytes(),
                    procSlot.size_bytes(),
                    tag,
                    comm
                );
            }
            else
            {
                IPstream fromProc(commsType, procIDs[i], 0, tag, comm);
                fromProc >> procSlot;
            }
        }
    }
    else
    {
        if (fld.empty())
        {
            // Nothing to do
        }
        else if (is_contiguous<Type>::value)
        {
            OPstream::write
            (
                commsType,
                procIDs[0],
                fld.cdata_bytes(),
                fld.size_bytes(),
                tag,
                comm
            );
        }
        else
        {
            OPstream toMaster(commsType, procIDs[0], 0, tag, comm);
            toMaster << fld;
        }
    }

    if (commsType == Pstream::commsTypes::nonBlocking)
    {
        // Wait for all to finish
        UPstream::waitRequests(startOfRequests);
    }
}


template<class Type, class Addr>
void Foam::globalIndex::gather
(
    const labelUList& off,  // needed on master only
    const label comm,
    const UList<int>& procIDs,
    const IndirectListBase<Type, Addr>& fld,
    List<Type>& allFld,
    const int tag,
    const Pstream::commsTypes preferredCommsType
)
{
    // low-level: no parRun guard

    // Automatically change from nonBlocking to scheduled for
    // non-contiguous data.
    const UPstream::commsTypes commsType =
    (
        (
            !is_contiguous<Type>::value
         && UPstream::commsTypes::nonBlocking == preferredCommsType
        )
      ? UPstream::commsTypes::scheduled
      : preferredCommsType
    );

    if (Pstream::myProcNo(comm) == procIDs[0])
    {
        allFld.resize_nocopy(off.last());  // == totalSize()

        // Assign my local data - respect offset information
        // so that we can request 0 entries to be copied

        SubList<Type> localSlot(allFld, off[1]-off[0], off[0]);
        if (!localSlot.empty())
        {
            localSlot = fld;
        }

        // Already verified commsType != nonBlocking
        for (label i = 1; i < procIDs.size(); ++i)
        {
            SubList<Type> procSlot(allFld, off[i+1]-off[i], off[i]);

            if (procSlot.empty())
            {
                // Nothing to do
            }
            else
            {
                IPstream fromProc(commsType, procIDs[i], 0, tag, comm);
                fromProc >> procSlot;
            }
        }
    }
    else
    {
        if (fld.empty())
        {
            // Nothing to do
        }
        else
        {
            OPstream toMaster(commsType, procIDs[0], 0, tag, comm);
            toMaster << fld;
        }
    }
}


template<class ProcIDsContainer, class Type>
void Foam::globalIndex::gather
(
    const labelUList& off,  // needed on master only
    const label comm,
    const ProcIDsContainer& procIDs,
    List<Type>& fld,
    const int tag,
    const Pstream::commsTypes commsType
)
{
    // low-level: no parRun guard

    List<Type> allData;

    gather(off, comm, procIDs, fld, allData, tag, commsType);

    if (Pstream::myProcNo(comm) == procIDs[0])
    {
        fld.transfer(allData);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void Foam::globalIndex::gather
(
    const UList<Type>& sendData,
    List<Type>& allData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
) const
{
    if (UPstream::parRun())
    {
        gather
        (
            offsets_,  // needed on master only
            comm,
            UPstream::procID(comm),
            sendData,
            allData,
            tag,
            commsType
        );
        if (!UPstream::master(comm))
        {
            allData.clear();  // safety: zero-size on non-master
        }
    }
    else
    {
        // Serial: direct copy
        allData = sendData;
    }
}


template<class Type, class Addr>
void Foam::globalIndex::gather
(
    const IndirectListBase<Type, Addr>& sendData,
    List<Type>& allData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
) const
{
    if (UPstream::parRun())
    {
        gather
        (
            offsets_,  // needed on master only
            comm,
            UPstream::procID(comm),
            sendData,
            allData,
            tag,
            commsType
        );
        if (!UPstream::master(comm))
        {
            allData.clear();  // safety: zero-size on non-master
        }
    }
    else
    {
        // Serial: direct copy
        allData = List<Type>(sendData);
    }
}


template<class Type, class OutputContainer>
OutputContainer Foam::globalIndex::gather
(
    const UList<Type>& sendData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
) const
{
    OutputContainer allData;
    gather(sendData, allData, tag, commsType, comm);
    return allData;
}


template<class Type, class Addr, class OutputContainer>
OutputContainer Foam::globalIndex::gather
(
    const IndirectListBase<Type, Addr>& sendData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
) const
{
    OutputContainer allData;
    gather(sendData, allData, tag, commsType, comm);
    return allData;
}


template<class Type>
void Foam::globalIndex::gatherInplace
(
    List<Type>& fld,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
) const
{
    if (UPstream::parRun())
    {
        List<Type> allData;

        gather
        (
            comm,
            UPstream::procID(comm),
            fld,
            allData,
            tag,
            commsType
        );

        if (UPstream::master(comm))
        {
            fld.transfer(allData);
        }
        else
        {
            fld.clear();  // zero-size on non-master
        }
    }
    // Serial: (no-op)
}


template<class Type, class OutputContainer>
void Foam::globalIndex::mpiGather
(
    const UList<Type>& sendData,
    OutputContainer& allData,
    const label comm
) const
{
    if (!UPstream::parRun())
    {
        // Serial: direct copy
        allData = sendData;
        return;
    }
    if (!is_contiguous<Type>::value)
    {
        FatalErrorInFunction
            << "Cannot be called for non-contiguous data" << nl
            << abort(FatalError);
    }

    auto nSendBytes = sendData.size_bytes();

    List<int> recvSizes;
    List<int> recvOffsets;

    if (UPstream::master(comm))
    {
        const globalIndex& globalAddr = *this;

        // Must be the same as Pstream::nProcs(comm), at least on master!!
        const label nproc = globalAddr.nProcs();

        // Allow request of 0 entries to be sent on master
        if (!globalAddr.localSize(0))
        {
            nSendBytes = 0;
        }

        allData.resize_nocopy(globalAddr.totalSize());

        recvSizes.resize(nproc);
        recvOffsets.resize(nproc+1);

        for (label proci = 0; proci < nproc; ++proci)
        {
            recvSizes[proci] = globalAddr.localSize(proci) * sizeof(Type);
            recvOffsets[proci] = globalAddr.localStart(proci) * sizeof(Type);
        }
        recvOffsets[nproc] = globalAddr.totalSize() * sizeof(Type);
    }
    else
    {
        allData.clear();  // safety: zero-size on non-master
    }

    UPstream::gather
    (
        sendData.cdata_bytes(),
        nSendBytes,
        allData.data_bytes(),
        recvSizes,
        recvOffsets,
        comm
    );
}


template<class Type, class OutputContainer>
OutputContainer Foam::globalIndex::mpiGather
(
    const UList<Type>& sendData,
    const label comm
) const
{
    OutputContainer allData;
    mpiGather(sendData, allData, comm);
    return allData;
}


template<class Type>
void Foam::globalIndex::mpiGatherInplace
(
    List<Type>& fld,
    const label comm
) const
{
    if (UPstream::parRun())
    {
        List<Type> allData;
        mpiGather(fld, allData, comm);

        if (UPstream::master(comm))
        {
            fld.transfer(allData);
        }
        else
        {
            fld.clear();  // zero-size on non-master
        }
    }
    // Serial: (no-op)
}


template<class Type, class OutputContainer>
void Foam::globalIndex::mpiGatherOp
(
    const UList<Type>& sendData,
    OutputContainer& allData,
    const label comm
)
{
    if (UPstream::parRun())
    {
        // Gather sizes - only needed on master
        globalIndex(sendData.size(), globalIndex::gatherOnly{}, comm)
            .mpiGather(sendData, allData, comm);
    }
    else
    {
        // Serial: direct copy
        allData = sendData;
    }
}


template<class Type, class OutputContainer>
OutputContainer Foam::globalIndex::mpiGatherOp
(
    const UList<Type>& sendData,
    const label comm
)
{
    OutputContainer allData;
    mpiGatherOp(sendData, allData, comm);
    return allData;
}


template<class Type>
void Foam::globalIndex::mpiGatherInplaceOp
(
    List<Type>& fld,
    const label comm
)
{
    if (UPstream::parRun())
    {
        List<Type> allData;
        mpiGatherOp(fld, allData, comm);

        if (UPstream::master(comm))
        {
            fld.transfer(allData);
        }
        else
        {
            fld.clear();  // zero-size on non-master
        }
    }
    // Serial: (no-op)
}


template<class Type>
void Foam::globalIndex::gatherOp
(
    const UList<Type>& sendData,
    List<Type>& allData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
)
{
    if (UPstream::parRun())
    {
        // Gather sizes - only needed on master
        globalIndex(sendData.size(), globalIndex::gatherOnly{}, comm)
            .gather(sendData, allData, tag, commsType, comm);
    }
    else
    {
        // Serial: direct copy
        allData = sendData;
    }
}


template<class Type, class Addr>
void Foam::globalIndex::gatherOp
(
    const IndirectListBase<Type, Addr>& sendData,
    List<Type>& allData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
)
{
    if (UPstream::parRun())
    {
        // Gather sizes - only needed on master
        globalIndex(sendData.size(), globalIndex::gatherOnly{}, comm)
            .gather(sendData, allData, tag, commsType, comm);
    }
    else
    {
        // Serial: direct copy
        allData = List<Type>(sendData);
    }
}


template<class Type, class OutputContainer>
OutputContainer Foam::globalIndex::gatherOp
(
    const UList<Type>& sendData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
)
{
    OutputContainer allData;
    gatherOp(sendData, allData, tag, commsType, comm);
    return allData;
}


template<class Type, class Addr, class OutputContainer>
OutputContainer Foam::globalIndex::gatherOp
(
    const IndirectListBase<Type, Addr>& sendData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
)
{
    OutputContainer allData;
    gatherOp(sendData, allData, tag, commsType, comm);
    return allData;
}


template<class Type>
void Foam::globalIndex::gatherInplaceOp
(
    List<Type>& fld,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
)
{
    if (UPstream::parRun())
    {
        // Gather sizes - only needed on master
        globalIndex(fld.size(), globalIndex::gatherOnly{}, comm)
            .gather(fld, tag, commsType, comm);
    }
    // Serial: (no-op)
}


template<class ProcIDsContainer, class Type>
void Foam::globalIndex::scatter
(
    const labelUList& off,  // needed on master only
    const label comm,
    const ProcIDsContainer& procIDs,
    const UList<Type>& allFld,
    UList<Type>& fld,
    const int tag,
    const Pstream::commsTypes preferredCommsType
)
{
    // low-level: no parRun guard

    // Automatically change from nonBlocking to scheduled for
    // non-contiguous data.
    const UPstream::commsTypes commsType =
    (
        (
            !is_contiguous<Type>::value
         && UPstream::commsTypes::nonBlocking == preferredCommsType
        )
      ? UPstream::commsTypes::scheduled
      : preferredCommsType
    );

    // FUTURE:
    // could decide which procs will receive data and use mpiScatter
    // to distribute. Could then skip send/receive for those procs...

    const label startOfRequests = UPstream::nRequests();

    if (Pstream::myProcNo(comm) == procIDs[0])
    {
        for (label i = 1; i < procIDs.size(); ++i)
        {
            const SubList<Type> procSlot(allFld, off[i+1]-off[i], off[i]);

            if (procSlot.empty())
            {
                // Nothing to do
            }
            else if (is_contiguous<Type>::value)
            {
                OPstream::write
                (
                    commsType,
                    procIDs[i],
                    procSlot.cdata_bytes(),
                    procSlot.size_bytes(),
                    tag,
                    comm
                );
            }
            else
            {
                OPstream toProc(commsType, procIDs[i], 0, tag, comm);
                toProc << procSlot;
            }
        }

        // Assign my local data - respect offset information
        // so that we can request 0 entries to be copied.
        // Also handle the case where we have a slice of the full
        // list.

        SubList<Type>(fld, off[1]-off[0]) =
            SubList<Type>(allFld, off[1]-off[0], off[0]);
    }
    else
    {
        // Note: we are receiving into UList, so sizes MUST match or we
        // have a problem. Can therefore reasonably assume that a zero-sized
        // send matches a zero-sized receive, and we can skip that.

        if (fld.empty())
        {
            // Nothing to do
        }
        else if (is_contiguous<Type>::value)
        {
            IPstream::read
            (
                commsType,
                procIDs[0],
                fld.data_bytes(),
                fld.size_bytes(),
                tag,
                comm
            );
        }
        else
        {
            IPstream fromMaster(commsType, procIDs[0], 0, tag, comm);
            fromMaster >> fld;
        }
    }

    if (commsType == Pstream::commsTypes::nonBlocking)
    {
        // Wait for all to finish
        UPstream::waitRequests(startOfRequests);
    }
}


template<class Type>
void Foam::globalIndex::scatter
(
    const UList<Type>& allData,
    UList<Type>& localData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
) const
{
    if (UPstream::parRun())
    {
        scatter
        (
            offsets_,  // needed on master only
            comm,
            UPstream::procID(comm),
            allData,
            localData,
            tag,
            commsType
        );
    }
    else
    {
        // Serial: direct copy
        // - fails miserably if incorrectly dimensioned!
        localData.deepCopy(allData);
    }
}


template<class Type, class OutputContainer>
OutputContainer Foam::globalIndex::scatter
(
    const UList<Type>& allData,
    const int tag,
    const Pstream::commsTypes commsType,
    const label comm
) const
{
    if (UPstream::parRun())
    {
        // The globalIndex might be correct on master only,
        // so scatter local sizes to ensure consistency

        const label localLen
        (
            UPstream::listScatterValues<label>(this->localSizes(), comm)
        );

        OutputContainer localData(localLen);
        this->scatter(allData, localData, tag, commsType, comm);

        return localData;
    }
    else
    {
        // Serial: direct copy
        return OutputContainer(allData);
    }
}


template<class Type, class CombineOp>
void Foam::globalIndex::get
(
    List<Type>& allFld,
    const labelUList& globalIds,
    const CombineOp& cop,
    const label comm,
    const int tag
) const
{
    allFld.resize_nocopy(globalIds.size());

    if (globalIds.size())
    {
        // Sort according to processor
        labelList order;
        DynamicList<label> validBins(Pstream::nProcs());

        CompactListList<label> bins
        (
            bin(offsets(), globalIds, order, validBins)
        );

        // Send local indices to individual processors as local index
        PstreamBuffers sendBufs(Pstream::commsTypes::nonBlocking, tag, comm);

        for (const auto proci : validBins)
        {
            labelList localIDs(bins[proci]);

            for (label& val : localIDs)
            {
                val = toLocal(proci, val);
            }

            UOPstream os(proci, sendBufs);
            os << localIDs;
        }
        sendBufs.finishedSends();


        PstreamBuffers returnBufs(Pstream::commsTypes::nonBlocking, tag, comm);

        for (const int proci : sendBufs.allProcs())
        {
            if (sendBufs.hasRecvData(proci))
            {
                UIPstream is(proci, sendBufs);
                labelList localIDs(is);

                // Collect entries
                List<Type> fld(localIDs.size());
                cop(fld, localIDs);

                UOPstream os(proci, returnBufs);
                os << fld;
            }
        }
        returnBufs.finishedSends();

        // Slot back
        for (const auto proci : validBins)
        {
            label start = bins.offsets()[proci];
            const SubList<label> es
            (
                order,
                bins.offsets()[proci+1]-start,  // start
                start
            );
            UIPstream is(proci, returnBufs);
            List<Type> fld(is);

            UIndirectList<Type>(allFld, es) = fld;
        }
    }
}


// ************************************************************************* //