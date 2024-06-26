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
    Foam::AveragingMethods::Dual

Group
    grpLagrangianIntermediateMPPICAveragingMethods

Description
    Dual-mesh lagrangian averaging procedure.

    Point values are summed using the tetrahedral decomposition of the
    computational cells. Summation is done in the cells, and also in the
    terahedrons surrounding each point. The latter forms a type of dual mesh.
    The interpolation is weighted by proximity to the cell centre or point, as
    calculated by the barycentric coordinate within the tethrahedron.

    Values are interpolated linearly across the tethrahedron. Gradients are
    calculated directly from the point values using a first order finite
    element basis. The computed gradient is assumed constant over the
    tethrahedron.

SourceFiles
    Dual.C

\*---------------------------------------------------------------------------*/

#ifndef Dual_H
#define Dual_H

#include "submodels/MPPIC/AveragingMethods/AveragingMethod/AveragingMethod.H"
#include "meshes/pointMesh/pointMesh.H"
#include "meshes/polyMesh/polyMeshTetDecomposition/tetIndices.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace AveragingMethods
{

/*---------------------------------------------------------------------------*\
                            Class Dual Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class Dual
:
    public AveragingMethod<Type>
{
public:

    //- Public typedefs

        //- Gradient type
        typedef typename AveragingMethod<Type>::TypeGrad TypeGrad;


private:

    //- Private data

        //- Volume of the cell-centered regions
        const Field<scalar>& volumeCell_;

        //- Volume of the point-centered regions
        Field<scalar> volumeDual_;

        //- Data on the cells
        Field<Type>& dataCell_;

        //- Data on the points
        Field<Type>& dataDual_;


    //- Private Member Functions

        //- The sizing for the FieldField parts
        static labelList sizing(const fvMesh& mesh);


        //- Sync point data over processor boundaries
        void syncDualData();


public:

    //- Runtime type information
    TypeName("dual");


    //- Constructors

        //- Construct from components
        Dual
        (
            const IOobject& io,
            const dictionary& dict,
            const fvMesh& mesh
        );

        //- Construct a copy
        Dual(const Dual<Type>& am);

        //- Construct and return a clone
        virtual autoPtr<AveragingMethod<Type>> clone() const
        {
            return autoPtr<AveragingMethod<Type>>
            (
                new Dual<Type>(*this)
            );
        }


    //- Destructor
    virtual ~Dual() = default;


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

        //- Calculate the average
        void average();
        void average(const AveragingMethod<scalar>& weight);

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
    #include "submodels/MPPIC/AveragingMethods/Dual/Dual.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
