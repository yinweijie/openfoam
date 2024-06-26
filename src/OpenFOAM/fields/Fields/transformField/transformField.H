/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

InNamespace
    Foam

Description
    Spatial transformation functions for primitive fields.

SourceFiles
    transformField.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_transformField_H
#define Foam_transformField_H

#include "primitives/transform/transform.H"
#include "primitives/quaternion/quaternion.H"
#include "primitives/septernion/septernion.H"
#include "fields/Fields/vectorField/vectorField.H"
#include "fields/Fields/tensorField/tensorField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// transform()

template<class Type>
void transform
(
    Field<Type>& result,
    const tensor& rot,
    const Field<Type>& fld
);

template<class Type>
void transform
(
    Field<Type>& result,
    const tensorField& rot,
    const Field<Type>& fld
);


template<class Type>
tmp<Field<Type>> transform
(
    const tensorField& rot,
    const Field<Type>& fld
);

template<class Type>
tmp<Field<Type>> transform
(
    const tensorField& rot,
    const tmp<Field<Type>>& tfld
);

template<class Type>
tmp<Field<Type>> transform
(
    const tmp<tensorField>& trot,
    const Field<Type>& fld
);

template<class Type>
tmp<Field<Type>> transform
(
    const tmp<tensorField>& trot,
    const tmp<Field<Type>>& tfld
);


template<class Type>
tmp<Field<Type>> transform
(
    const tensor& rot,
    const Field<Type>& fld
);

template<class Type>
tmp<Field<Type>> transform
(
    const tensor& rot,
    const tmp<Field<Type>>& tfld
);


// invTransform()

template<class Type>
void invTransform
(
    Field<Type>& result,
    const tensor& rot,
    const Field<Type>& fld
);

template<class Type>
void invTransform
(
    Field<Type>& result,
    const tensorField& rot,
    const Field<Type>& fld
);


template<class Type>
tmp<Field<Type>> invTransform
(
    const tensorField& rot,
    const Field<Type>& fld
);

template<class Type>
tmp<Field<Type>> invTransform
(
    const tensorField& rot,
    const tmp<Field<Type>>& tfld
);

template<class Type>
tmp<Field<Type>> invTransform
(
    const tmp<tensorField>& trot,
    const Field<Type>& fld
);

template<class Type>
tmp<Field<Type>> invTransform
(
    const tmp<tensorField>& trot,
    const tmp<Field<Type>>& tfld
);


template<class Type>
tmp<Field<Type>> invTransform
(
    const tensor& rot,
    const Field<Type>& fld
);

template<class Type>
tmp<Field<Type>> invTransform
(
    const tensor& rot,
    const tmp<Field<Type>>& tfld
);


// transformFieldMask()

template<class Type1, class Type2>
tmp<Field<Type1>> transformFieldMask(const Field<Type2>& fld);

template<class Type1, class Type2>
tmp<Field<Type1>> transformFieldMask(const tmp<Field<Type2>>& tfld);


// Specializations

template<>
tmp<Field<symmTensor>>
transformFieldMask<symmTensor>(const tensorField&);

template<>
tmp<Field<symmTensor>>
transformFieldMask<symmTensor>(const tmp<tensorField>&);

template<>
tmp<Field<sphericalTensor>>
transformFieldMask<sphericalTensor>(const tensorField&);

template<>
tmp<Field<sphericalTensor>>
transformFieldMask<sphericalTensor>(const tmp<tensorField>&);


//- Rotate given vectorField with the given quaternion
void transform(vectorField&, const quaternion&, const vectorField&);

//- Rotate given vectorField with the given quaternion
tmp<vectorField> transform(const quaternion&, const vectorField&);

//- Rotate given tmp<vectorField> with the given quaternion
tmp<vectorField> transform(const quaternion&, const tmp<vectorField>&);


//- Transform given vectorField of coordinates with the given septernion
void transformPoints(vectorField&, const septernion&, const vectorField&);

//- Transform given vectorField of coordinates with the given septernion
tmp<vectorField> transformPoints(const septernion&, const vectorField&);

//- Transform given tmp<vectorField> of coordinates with the given septernion
tmp<vectorField> transformPoints(const septernion&, const tmp<vectorField>&);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "fields/Fields/transformField/transformFieldTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
