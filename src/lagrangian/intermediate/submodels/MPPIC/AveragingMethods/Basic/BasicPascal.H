/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2017 OpenFOAM Foundation
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
    Foam::AveragingMethods::Basic

Group
    grpLagrangianIntermediateMPPICAveragingMethods

Description
    Basic lagrangian averaging procedure.

    This is a cell-volume based average. Point values are summed over the
    computational cells and the result is divided by the cell volume.

    Interpolation is done assuming a constant value over each cells. Cell
    gradients are calculated by the default fvc::grad scheme, and are also
    assumed constant when interpolated.

SourceFiles
    Basic.C

\*---------------------------------------------------------------------------*/

#ifndef Basic_H
#define Basic_H

#include "submodels/MPPIC/AveragingMethods/AveragingMethod/AveragingMethod.H"
#include "meshes/pointMesh/pointMesh.H"
#include "meshes/polyMesh/polyMeshTetDecomposition/tetIndices.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace AveragingMethods
{

/*---------------------------------------------------------------------------*\
                        Class Basic Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class Basic
:
    public AveragingMethod<Type>
{
public:

    //- Public typedefs

        //- Gradient type
        typedef typename AveragingMethod<Type>::TypeGrad TypeGrad;


private:

    //- Private data

        //- Cell average field
        Field<Type>& data_;

        //- Gradient field
        Field<TypeGrad> dataGrad_;


    //- Private member functions

        //- Re-calculate gradient
        virtual void updateGrad();


public:

    //- Runtime type information
    TypeName("basic");


    //- Constructors

        //- Construct from components
        Basic
        (
            const IOobject& io,
            const dictionary& dict,
            const fvMesh &mesh
        );

        //- Construct a copy
        Basic(const Basic<Type>& am);

        //- Construct and return a clone
        virtual autoPtr<AveragingMethod<Type>> clone() const
        {
            return autoPtr<AveragingMethod<Type>>
            (
                new Basic<Type>(*this)
            );
        }


    //- Destructor
    virtual ~Basic();


    //- Member Functions

        //- Add point value to interpolation
        void add
        (
            const barycentric& coordinates,
            const tetIndices& tetIs,
            const Type& value
        );

        //- Interpolate
        Type interpolate
        (
            const barycentric& coordinates,
            const tetIndices& tetIs
        ) const;

        //- Interpolate gradient
        TypeGrad interpolateGrad
        (
            const barycentric& coordinates,
            const tetIndices& tetIs
        ) const;

        //- Return an internal field of the average
        tmp<Field<Type>> primitiveField() const;

        //- Return an internal field of the gradient
        tmp<Field<TypeGrad>> internalFieldGrad() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace AveragingMethods
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "submodels/MPPIC/AveragingMethods/Basic/BasicPascal.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
