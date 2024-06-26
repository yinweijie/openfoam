/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2018-2019 OpenCFD Ltd.
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

#include "fields/surfaceFields/surfaceFields.H"

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class T>
void Foam::dynamicRefineFvMesh::mapNewInternalFaces
(
    const labelList& faceMap,
    GeometricField<T, fvsPatchField, surfaceMesh>& sFld
)
{
    typedef GeometricField<T, fvsPatchField, surfaceMesh> GeoField;

    //- Make flat field for ease of looping
    Field<T> tsFld(this->nFaces(), Zero);
    SubField<T>(tsFld, this->nInternalFaces()) = sFld.internalField();

    const typename GeoField::Boundary& bFld = sFld.boundaryField();
    forAll(bFld, patchi)
    {
        label facei = this->boundaryMesh()[patchi].start();
        for (const T& val : bFld[patchi])
        {
            tsFld[facei++] = val;
        }
    }

    const labelUList& owner = this->faceOwner();
    const labelUList& neighbour = this->faceNeighbour();
    const cellList& cells = this->cells();

    for (label facei = 0; facei < nInternalFaces(); facei++)
    {
        label oldFacei = faceMap[facei];

        // Map surface field on newly generated faces by obtaining the
        // hull of the outside faces
        if (oldFacei == -1)
        {
            // Loop over all owner/neighbour cell faces
            // and find already mapped ones (master-faces):
            T tmpValue(pTraits<T>::zero);
            label counter = 0;

            const cell& ownFaces = cells[owner[facei]];
            for (auto ownFacei : ownFaces)
            {
                if (faceMap[ownFacei] != -1)
                {
                    tmpValue += tsFld[ownFacei];
                    counter++;
                }
            }

            const cell& neiFaces = cells[neighbour[facei]];
            for (auto neiFacei : neiFaces)
            {
                if (faceMap[neiFacei] != -1)
                {
                    tmpValue += tsFld[neiFacei];
                    counter++;
                }
            }

            if (counter > 0)
            {
                sFld[facei] = tmpValue/counter;
            }
        }
    }
}


template<class T>
void Foam::dynamicRefineFvMesh::mapNewInternalFaces
(
    const labelList& faceMap
)
{
    typedef GeometricField<T, fvsPatchField, surfaceMesh> GeoField;

    for (GeoField& sFld : this->objectRegistry::sorted<GeoField>())
    {
        //if (mapSurfaceFields_.found(sFld.name()))
        {
            DebugInfo
                << "dynamicRefineFvMesh::mapNewInternalFaces():"
                << " Mapping new internal faces by interpolation on "
                << sFld.name()<< endl;

            if (sFld.is_oriented())
            {
                WarningInFunction << "Ignoring mapping oriented field "
                    << sFld.name() << " since of type " << sFld.type() << endl;
            }
            else
            {
                mapNewInternalFaces(faceMap, sFld);
            }
        }
    }
}

template<class T>
void Foam::dynamicRefineFvMesh::mapNewInternalFaces
(
    const surfaceVectorField& Sf,
    const surfaceScalarField& magSf,
    const labelList& faceMap
)
{
    typedef GeometricField<T, fvsPatchField, surfaceMesh> GeoField;

    typedef GeometricField
    <
        typename outerProduct<vector, T>::type,
        fvsPatchField,
        surfaceMesh
    > NormalGeoField;


    for (GeoField& sFld : this->objectRegistry::sorted<GeoField>())
    {
        //if (mapSurfaceFields_.found(sFld.name()))
        {
            DebugInfo
                << "dynamicRefineFvMesh::mapNewInternalFaces():"
                << " Mapping new internal faces by interpolation on "
                << sFld.name() << endl;

            if (sFld.is_oriented())
            {
                DebugInfo
                    << "dynamicRefineFvMesh::mapNewInternalFaces(): "
                    << "Converting oriented field " << sFld.name()
                    << " to intensive field and mapping" << endl;

                // Assume any oriented field is face area weighted (i.e. a flux)
                // Convert to intensive (& oriented) before mapping. Untested.

                // Convert to intensive and non oriented
                NormalGeoField fFld(sFld*Sf/Foam::sqr(magSf));

                // Interpolate
                mapNewInternalFaces(faceMap, fFld);

                // Convert back to extensive and oriented
                sFld = (fFld & Sf);
            }
            else
            {
                mapNewInternalFaces(faceMap, sFld);
            }
        }
    }
}


// ************************************************************************* //
