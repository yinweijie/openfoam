/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2015-2021 OpenCFD Ltd.
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

#include "nearWallFields/nearWallFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
void Foam::functionObjects::nearWallFields::createFields
(
    PtrList<GeometricField<Type, fvPatchField, volMesh>>& sflds
) const
{
    typedef GeometricField<Type, fvPatchField, volMesh> VolFieldType;

    for (const VolFieldType& fld : obr_.csorted<VolFieldType>())
    {
        const auto fieldMapIter = fieldMap_.cfind(fld.name());

        if (fieldMapIter.good())
        {
            const word& sampleFldName = fieldMapIter.val();

            if (obr_.found(sampleFldName))
            {
                WarningInFunction
                    << "    a field named " << sampleFldName
                    << " already exists on the mesh"
                    << endl;
            }
            else
            {
                IOobject io(fld);
                io.readOpt(IOobjectOption::NO_READ);
                io.writeOpt(IOobjectOption::NO_WRITE);
                io.rename(sampleFldName);

                // Override bc to be calculated
                const label newFieldi = sflds.size();
                sflds.resize(newFieldi+1);

                sflds.set
                (
                    newFieldi,
                    new VolFieldType
                    (
                        io,
                        fld,
                        patchIDs_,
                        fvPatchFieldBase::calculatedType()
                    )
                );

                Log << "    created " << io.name()
                    << " to sample " << fld.name() << endl;
            }
        }
    }
}


template<class Type>
void Foam::functionObjects::nearWallFields::sampleBoundaryField
(
    const interpolationCellPoint<Type>& interpolator,
    GeometricField<Type, fvPatchField, volMesh>& fld
) const
{
    // Construct flat fields for all patch faces to be sampled
    Field<Type> sampledValues(getPatchDataMapPtr_().constructSize());

    forAll(cellToWalls_, celli)
    {
        const labelList& cData = cellToWalls_[celli];

        forAll(cData, i)
        {
            const point& samplePt = cellToSamples_[celli][i];
            sampledValues[cData[i]] = interpolator.interpolate(samplePt, celli);
        }
    }

    // Send back sampled values to patch faces
    getPatchDataMapPtr_().reverseDistribute
    (
        getPatchDataMapPtr_().constructSize(),
        sampledValues
    );

    typename GeometricField<Type, fvPatchField, volMesh>::
        Boundary& fldBf = fld.boundaryFieldRef();

    // Pick up data
    label nPatchFaces = 0;
    for (const label patchi : patchIDs_)
    {
        fvPatchField<Type>& pfld = fldBf[patchi];

        Field<Type> newFld(pfld.size());
        forAll(pfld, i)
        {
            newFld[i] = sampledValues[nPatchFaces++];
        }

        pfld == newFld;
    }
}


template<class Type>
void Foam::functionObjects::nearWallFields::sampleFields
(
    PtrList<GeometricField<Type, fvPatchField, volMesh>>& sflds
) const
{
    typedef GeometricField<Type, fvPatchField, volMesh> VolFieldType;

    forAll(sflds, i)
    {
        const word& fldName = reverseFieldMap_[sflds[i].name()];
        const VolFieldType& fld = obr_.lookupObject<VolFieldType>(fldName);

        // Take over internal and boundary values
        sflds[i] == fld;

        // Construct interpolation method
        interpolationCellPoint<Type> interpolator(fld);

        // Override sampled values
        sampleBoundaryField(interpolator, sflds[i]);
    }
}


// ************************************************************************* //
