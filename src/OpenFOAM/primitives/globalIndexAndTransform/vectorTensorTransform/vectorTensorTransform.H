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

Class
    Foam::vectorTensorTransform

Description
    Vector-tensor class used to perform translations and rotations in
    3D space.

SourceFiles
    vectorTensorTransformI.H
    vectorTensorTransform.C
    vectorTensorTransformTemplates.C

\*---------------------------------------------------------------------------*/

#ifndef vectorTensorTransform_H
#define vectorTensorTransform_H

#include "primitives/Tensor/floats/tensor.H"
#include "primitives/strings/word/word.H"
#include "primitives/traits/contiguous.H"
#include "meshes/primitiveShapes/point/pointField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class vectorTensorTransform;
Istream& operator>>(Istream& is, vectorTensorTransform&);
Ostream& operator<<(Ostream& os, const vectorTensorTransform& C);


/*---------------------------------------------------------------------------*\
                    Class vectorTensorTransform Declaration
\*---------------------------------------------------------------------------*/

class vectorTensorTransform
{
    // Private Data

        //- Translation vector
        vector t_;

        //- Rotation tensor
        tensor R_;

        //- Recording if the transform has non-identity transform to
        //  allow its calculation to be skipped, which is the majority
        //  of the expected cases
        bool hasR_;


public:

    // Static Data

        static const char* const typeName;

        static const vectorTensorTransform zero;

        static const vectorTensorTransform I;


    // Generated Methods

        //- Copy construct
        vectorTensorTransform(const vectorTensorTransform&) = default;

        //- Copy assignment
        vectorTensorTransform&
            operator=(const vectorTensorTransform&) = default;


    // Constructors

        //- Default construct - no translation, identity rotation.
        inline vectorTensorTransform();

        //- Construct given a translation vector, rotation tensor and
        //- hasR bool
        inline vectorTensorTransform
        (
            const vector& t,
            const tensor& R,
            bool hasR = true
        );

        //- Construct a pure translation vectorTensorTransform given a
        //- translation vector
        inline explicit vectorTensorTransform(const vector& t);

        //- Construct a pure rotation vectorTensorTransform given a
        //- rotation tensor
        inline explicit vectorTensorTransform(const tensor& R);

        //- Construct from Istream
        explicit vectorTensorTransform(Istream&);


    // Member Functions

        // Access

            inline const vector& t() const;

            inline const tensor& R() const;

            inline bool hasR() const;


        // Edit

            inline vector& t();

            inline tensor& R();


        // Transform

            //- Transform the given position
            inline vector transformPosition(const vector& v) const;

            //- Transform the given pointField
            inline pointField transformPosition(const pointField& pts) const;

            //- Inverse transform the given position
            inline vector invTransformPosition(const vector& v) const;

            //- Inverse transform the given pointField
            inline pointField invTransformPosition(const pointField& pts) const;

            //- Transform the given field
            template<class Type>
            tmp<Field<Type>> transform(const Field<Type>&) const;


    // Member Operators

        inline void operator&=(const vectorTensorTransform&);

        //- Assign translation
        inline void operator=(const vector&);
        inline void operator+=(const vector&);
        inline void operator-=(const vector&);

        inline void operator=(const tensor&);
        inline void operator&=(const tensor&);


    // IOstream Operators

        friend Istream& operator>>(Istream& is, vectorTensorTransform&);

        friend Ostream& operator<<(Ostream& os, const vectorTensorTransform&);
};


// * * * * * * * * * * * * * * * * * Traits  * * * * * * * * * * * * * * * * //

//- Contiguous data for vectorTensorTransform
template<> struct is_contiguous<vectorTensorTransform> : std::true_type {};


// * * * * * * * * * * * * * * * Global Functions  * * * * * * * * * * * * * //

//- Return the inverse of the given vectorTensorTransform
inline vectorTensorTransform inv(const vectorTensorTransform& tr);

//- Return a string representation of a vectorTensorTransform
word name(const vectorTensorTransform&);

//- Template specialisations
template<>
tmp<Field<bool>> vectorTensorTransform::transform(const Field<bool>&) const;

template<>
tmp<Field<label>> vectorTensorTransform::transform(const Field<label>&) const;

template<>
tmp<Field<scalar>> vectorTensorTransform::transform(const Field<scalar>&)
const;


// * * * * * * * * * * * * * * * Global Operators  * * * * * * * * * * * * * //

inline bool operator==
(
    const vectorTensorTransform& tr1,
    const vectorTensorTransform& tr2
);


inline bool operator!=
(
    const vectorTensorTransform& tr1,
    const vectorTensorTransform& tr2

);


inline vectorTensorTransform operator+
(
    const vectorTensorTransform& tr,
    const vector& t
);


inline vectorTensorTransform operator+
(
    const vector& t,
    const vectorTensorTransform& tr
);


inline vectorTensorTransform operator-
(
    const vectorTensorTransform& tr,
    const vector& t
);


inline vectorTensorTransform operator&
(
    const vectorTensorTransform& tr1,
    const vectorTensorTransform& tr2
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "primitives/globalIndexAndTransform/vectorTensorTransform/vectorTensorTransformI.H"

#ifdef NoRepository
    #include "primitives/globalIndexAndTransform/vectorTensorTransform/vectorTensorTransformTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
