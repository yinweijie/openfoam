/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 Wikki Ltd
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

#include "faMesh/faMeshMapper/faAreaMapper.H"
#include "meshes/polyMesh/mapPolyMesh/mapPolyMesh.H"
#include "include/demandDrivenData.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::faAreaMapper::calcAddressing() const
{
    if
    (
        newFaceLabelsPtr_
     || newFaceLabelsMapPtr_
     || directAddrPtr_
     || interpolationAddrPtr_
     || weightsPtr_
     || insertedObjectLabelsPtr_
    )
    {
        FatalErrorInFunction
            << "Addressing already calculated"
            << abort(FatalError);
    }

    // Mapping

    const label oldNInternal = mpm_.nOldInternalFaces();

    hasUnmapped_ = false;

    // Calculate new face labels

    // Copy old face labels
    const labelList& oldFaces = mesh_.faceLabels();

    // Prepare a list of new face labels and (preliminary) addressing
    // Note: dimensioned to number of boundary faces of polyMesh
    newFaceLabelsPtr_ = new labelList(mesh_.mesh().nBoundaryFaces(), -1);
    labelList& newFaceLabels = *newFaceLabelsPtr_;

    newFaceLabelsMapPtr_ = new labelList(mesh_.mesh().nBoundaryFaces(), -1);
    labelList& newFaceLabelsMap = *newFaceLabelsMapPtr_;
    label nNewFaces = 0;

    Info<< "Old face list size: " << oldFaces.size()
        << " estimated new size " << newFaceLabels.size() << endl;

    // Get reverse face map
    const labelList& reverseFaceMap = mpm_.reverseFaceMap();

    // Pick up live old faces
    forAll(oldFaces, faceI)
    {
        if (reverseFaceMap[oldFaces[faceI]] > -1)
        {
            // Face is live, add it and record addressing
            newFaceLabels[nNewFaces] = reverseFaceMap[oldFaces[faceI]];
            newFaceLabelsMap[nNewFaces] = faceI;

            nNewFaces++;
        }
    }

    // Assemble the maps
    if (direct())
    {
        Info<< "Direct"<< endl;
        // Direct mapping: no further faces to add.  Resize list
        newFaceLabels.setSize(nNewFaces);

        directAddrPtr_ = new labelList(newFaceLabels.size());
        labelList& addr = *directAddrPtr_;

        // Adjust for creation of a boundary face from an internal face
        forAll(addr, faceI)
        {
            if (newFaceLabelsMap[faceI] < oldNInternal)
            {
                addr[faceI] = 0;
            }
            else
            {
                addr[faceI] = newFaceLabelsMap[faceI];
            }
        }
    }
    else
    {
        // There are further faces to add.  Prepare interpolation addressing
        // and weights to full size
        interpolationAddrPtr_ = new labelListList(newFaceLabels.size());
        labelListList& addr = *interpolationAddrPtr_;

        weightsPtr_ = new scalarListList(newFaceLabels.size());
        scalarListList& w = *weightsPtr_;

        // Insert single addressing and weights
        for (label addrI = 0; addrI < nNewFaces; ++addrI)
        {
            addr[addrI] = labelList(1, newFaceLabelsMap[addrI]);
            w[addrI] = scalarList(1, scalar(1));
        }

        // Pick up faces from points, edges and faces where the origin
        // Only map from faces which were previously in the faMesh, using
        // fast lookup

        // Set of faces previously in the mesh
        labelHashSet oldFaceLookup(oldFaces);

        // Go through faces-from lists and add the ones where all
        // old face labels belonged to the faMesh

        const List<objectMap>& ffp = mpm_.facesFromPointsMap();

        forAll(ffp, ffpI)
        {
            // Get addressing
            const labelList& mo = ffp[ffpI].masterObjects();

            // Check if master objects are in faMesh
            labelList validMo(mo.size());
            label nValidMo = 0;

            forAll(mo, moI)
            {
                if (oldFaceLookup.found(mo[moI]))
                {
                    validMo[nValidMo] = oldFaceLookup[mo[moI]];
                    nValidMo++;
                }
            }

            if (nValidMo > 0)
            {
                // Some objects found: add face and interpolation to list
                newFaceLabels[nNewFaces] = ffp[ffpI].index();

                // No old face available
                newFaceLabelsMap[nNewFaces] = -1;

                // Map from masters, uniform weights
                addr[nNewFaces] = validMo;
                w[nNewFaces] = scalarList(validMo.size(), 1.0/validMo.size());

                nNewFaces++;
            }
        }

        const List<objectMap>& ffe = mpm_.facesFromEdgesMap();

        forAll(ffe, ffeI)
        {
            // Get addressing
            const labelList& mo = ffe[ffeI].masterObjects();

            // Check if master objects are in faMesh
            labelList validMo(mo.size());
            label nValidMo = 0;

            forAll(mo, moI)
            {
                if (oldFaceLookup.found(mo[moI]))
                {
                    validMo[nValidMo] = oldFaceLookup[mo[moI]];
                    nValidMo++;
                }
            }

            if (nValidMo > 0)
            {
                // Some objects found: add face and interpolation to list
                newFaceLabels[nNewFaces] = ffe[ffeI].index();

                // No old face available
                newFaceLabelsMap[nNewFaces] = -1;

                // Map from masters, uniform weights
                addr[nNewFaces] = validMo;
                w[nNewFaces] = scalarList(validMo.size(), 1.0/validMo.size());

                nNewFaces++;
            }
        }

        const List<objectMap>& fff = mpm_.facesFromFacesMap();

        forAll(fff, fffI)
        {
            // Get addressing
            const labelList& mo = fff[fffI].masterObjects();

            // Check if master objects are in faMesh
            labelList validMo(mo.size());
            label nValidMo = 0;

            forAll(mo, moI)
            {
                if (oldFaceLookup.found(mo[moI]))
                {
                    validMo[nValidMo] = oldFaceLookup[mo[moI]];
                    nValidMo++;
                }
            }

            if (nValidMo > 0)
            {
                // Some objects found: add face and interpolation to list
                newFaceLabels[nNewFaces] = fff[fffI].index();

                // No old face available
                newFaceLabelsMap[nNewFaces] = -1;

                // Map from masters, uniform weights
                addr[nNewFaces] = validMo;
                w[nNewFaces] = scalarList(validMo.size(), 1.0/validMo.size());

                nNewFaces++;
            }
        }

        // All faces collected.  Reset sizes of lists
        newFaceLabels.setSize(nNewFaces);
        newFaceLabelsMap.setSize(nNewFaces);
        addr.setSize(nNewFaces);
        w.setSize(nNewFaces);
        Info<< "addr: " << addr << nl
            << "w: " << w << endl;
    }

    // Inserted objects cannot appear in the new faMesh as they have no master
    // HJ, 10/Aug/2011
    insertedObjectLabelsPtr_ = new labelList(0);
}


void Foam::faAreaMapper::clearOut()
{
    deleteDemandDrivenData(newFaceLabelsPtr_);
    deleteDemandDrivenData(newFaceLabelsMapPtr_);

    deleteDemandDrivenData(directAddrPtr_);
    deleteDemandDrivenData(interpolationAddrPtr_);
    deleteDemandDrivenData(weightsPtr_);

    deleteDemandDrivenData(insertedObjectLabelsPtr_);
    hasUnmapped_ = false;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::faAreaMapper::faAreaMapper
(
    const faMesh& mesh,
    const mapPolyMesh& mpm
)
:
    mesh_(mesh),
    mpm_(mpm),
    direct_(false),
    hasUnmapped_(false),
    sizeBeforeMapping_(mesh.nFaces()),
    newFaceLabelsPtr_(nullptr),
    newFaceLabelsMapPtr_(nullptr),
    directAddrPtr_(nullptr),
    interpolationAddrPtr_(nullptr),
    weightsPtr_(nullptr),
    insertedObjectLabelsPtr_(nullptr)
{
    // Check for possibility of direct mapping
    if
    (
        mpm_.facesFromPointsMap().empty()
     && mpm_.facesFromEdgesMap().empty()
     && mpm_.facesFromFacesMap().empty()
    )
    {
        direct_ = true;
    }
    else
    {
        direct_ = false;
    }

    // Inserted objects not supported: no master
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::faAreaMapper::~faAreaMapper()
{
    clearOut();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::labelList& Foam::faAreaMapper::newFaceLabels() const
{
    if (!newFaceLabelsPtr_)
    {
        calcAddressing();
    }

    return *newFaceLabelsPtr_;
}


const Foam::labelList& Foam::faAreaMapper::newFaceLabelsMap() const
{
    if (!newFaceLabelsMapPtr_)
    {
        calcAddressing();
    }

    return *newFaceLabelsMapPtr_;
}


const Foam::labelUList& Foam::faAreaMapper::directAddressing() const
{
    if (!direct())
    {
        FatalErrorInFunction
            << "Requested direct addressing for an interpolative mapper."
            << abort(FatalError);
    }

    if (!directAddrPtr_)
    {
        calcAddressing();
    }

    return *directAddrPtr_;
}


const Foam::labelListList& Foam::faAreaMapper::addressing() const
{
    if (direct())
    {
        FatalErrorInFunction
            << "Requested interpolative addressing for a direct mapper."
            << abort(FatalError);
    }

    if (!interpolationAddrPtr_)
    {
        calcAddressing();
    }

    return *interpolationAddrPtr_;
}


const Foam::scalarListList& Foam::faAreaMapper::weights() const
{
    if (direct())
    {
        FatalErrorInFunction
            << "Requested interpolative weights for a direct mapper."
            << abort(FatalError);
    }

    if (!weightsPtr_)
    {
        calcAddressing();
    }

    return *weightsPtr_;
}


const Foam::labelList& Foam::faAreaMapper::insertedObjectLabels() const
{
    if (!insertedObjectLabelsPtr_)
    {
        calcAddressing();
    }

    return *insertedObjectLabelsPtr_;
}


// ************************************************************************* //
