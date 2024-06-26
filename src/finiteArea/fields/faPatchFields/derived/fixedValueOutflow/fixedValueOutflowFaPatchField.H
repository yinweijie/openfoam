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

Class
    Foam::fixedValueOutflowFaPatchField

Description

Author
    Zeljko Tukovic, FMENA
    Hrvoje Jasak, Wikki Ltd.

SourceFiles
    fixedValueOutflowFaPatchField.C

\*---------------------------------------------------------------------------*/

#ifndef fixedValueOutflowFaPatchField_H
#define fixedValueOutflowFaPatchField_H

#include "fields/faPatchFields/faPatchField/faPatchField.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                  Class fixedValueOutflowFaPatch Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class fixedValueOutflowFaPatchField
:
    public faPatchField<Type>
{
public:

    //- Runtime type information
    TypeName("fixedValueOutflow");


    // Constructors

        //- Construct from patch and internal field
        fixedValueOutflowFaPatchField
        (
            const faPatch&,
            const DimensionedField<Type, areaMesh>&
        );

        //- Construct from patch, internal field and dictionary
        //  The "value" entry: MUST_READ
        fixedValueOutflowFaPatchField
        (
            const faPatch&,
            const DimensionedField<Type, areaMesh>&,
            const dictionary&
        );

        //- Construct by mapping the given fixedValueOutflow patch field
        //- onto a new patch
        fixedValueOutflowFaPatchField
        (
            const fixedValueOutflowFaPatchField<Type>&,
            const faPatch&,
            const DimensionedField<Type, areaMesh>&,
            const faPatchFieldMapper&
        );

        //- Construct as copy
        fixedValueOutflowFaPatchField
        (
            const fixedValueOutflowFaPatchField<Type>&
        );

        //- Construct and return a clone
        virtual tmp<faPatchField<Type>> clone() const
        {
            return tmp<faPatchField<Type>>
            (
                new fixedValueOutflowFaPatchField<Type>(*this)
            );
        }

        //- Construct as copy setting internal field reference
        fixedValueOutflowFaPatchField
        (
            const fixedValueOutflowFaPatchField<Type>&,
            const DimensionedField<Type, areaMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<faPatchField<Type>> clone
        (
            const DimensionedField<Type, areaMesh>& iF
        ) const
        {
            return tmp<faPatchField<Type>>
            (
                new fixedValueOutflowFaPatchField<Type>(*this, iF)
            );
        }


    // Member Functions

        //- True: the patch field fixes a value.
        virtual bool fixesValue() const { return true; }


        // Evaluation functions

            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<Type>> valueInternalCoeffs
            (
                const tmp<scalarField>&
            ) const;

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the value of this patchField with given weights
            virtual tmp<Field<Type>> valueBoundaryCoeffs
            (
                const tmp<scalarField>&
            ) const;

            //- Return the matrix diagonal coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            virtual tmp<Field<Type>> gradientInternalCoeffs() const;

            //- Return the matrix source coefficients corresponding to the
            //  evaluation of the gradient of this patchField
            virtual tmp<Field<Type>> gradientBoundaryCoeffs() const;


        //- Write
        virtual void write(Ostream&) const;


    // Member operators

        virtual void operator=(const UList<Type>&) {}

        virtual void operator=(const faPatchField<Type>&) {}
        virtual void operator+=(const faPatchField<Type>&) {}
        virtual void operator-=(const faPatchField<Type>&) {}
        virtual void operator*=(const faPatchField<scalar>&) {}
        virtual void operator/=(const faPatchField<scalar>&) {}

        virtual void operator+=(const Field<Type>&) {}
        virtual void operator-=(const Field<Type>&) {}

        virtual void operator*=(const Field<scalar>&) {}
        virtual void operator/=(const Field<scalar>&) {}

        virtual void operator=(const Type&) {}
        virtual void operator+=(const Type&) {}
        virtual void operator-=(const Type&) {}
        virtual void operator*=(const scalar) {}
        virtual void operator/=(const scalar) {}
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "fields/faPatchFields/derived/fixedValueOutflow/fixedValueOutflowFaPatchField.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
