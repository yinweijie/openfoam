/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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
    Foam::coupledFvPatch

Description
    An abstract base class for patches that couple regions of the
    computational domain e.g. cyclic and processor-processor links.

SourceFiles
    coupledFvPatch.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_coupledFvPatch_H
#define Foam_coupledFvPatch_H

#include "fvMesh/fvPatches/fvPatch/fvPatch.H"
#include "matrices/lduMatrix/lduAddressing/lduInterface/lduInterface.H"
#include "meshes/polyMesh/polyPatches/basic/coupled/coupledPolyPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class coupledFvPatch Declaration
\*---------------------------------------------------------------------------*/

class coupledFvPatch
:
    public lduInterface,
    public fvPatch
{
    // Private Data

        const coupledPolyPatch& coupledPolyPatch_;


protected:

    // Protected Member Functions

        //- Make patch weighting factors
        virtual void makeWeights(scalarField&) const = 0;


public:

    friend class surfaceInterpolation;


    //- Runtime type information
    TypeNameNoDebug(coupledPolyPatch::typeName_());


    // Constructors

        //- Construct from polyPatch
        coupledFvPatch(const polyPatch& patch, const fvBoundaryMesh& bm)
        :
            fvPatch(patch, bm),
            coupledPolyPatch_(refCast<const coupledPolyPatch>(patch))
        {}


    //- Destructor
    virtual ~coupledFvPatch() = default;


    // Member Functions

        // Access

            //- Return true because this patch is coupled
            virtual bool coupled() const
            {
                return coupledPolyPatch_.coupled();
            }

            //- Are the cyclic planes parallel.
            virtual bool parallel() const = 0;

            //- Return face transformation tensor.
            virtual const tensorField& forwardT() const = 0;

            //- Return neighbour-cell transformation tensor.
            virtual const tensorField& reverseT() const = 0;

            //- Return faceCell addressing
            virtual const labelUList& faceCells() const
            {
                return fvPatch::faceCells();
            }

            //- Return delta (P to N) vectors across coupled patch
            virtual tmp<vectorField> delta() const = 0;


        // Interface transfer functions

            //- Return the values of the given internal data adjacent to
            //- the interface as a field
            virtual tmp<labelField> interfaceInternalField
            (
                const labelUList& internalData
            ) const = 0;


            //- Return the values of the given internal data adjacent to
            //- the interface as a field using a mapping faceCell
            virtual tmp<labelField> interfaceInternalField
            (
                const labelUList& internalData,
                const labelUList& faceCell
            ) const = 0;

            //- Inherit initInternalFieldTransfer from lduInterface
            using lduInterface::initInternalFieldTransfer;

            //- Initialise neighbour field transfer
            virtual void initInternalFieldTransfer
            (
                const Pstream::commsTypes commsType,
                labelUList& iF
            ) const
            {}

            //- Initialise neighbour field transfer using faceCells map
            virtual void initInternalFieldTransfer
            (
                const Pstream::commsTypes commsType,
                labelUList& iF,
                const labelUList& faceCell
            ) const
            {}

            //- Return neighbour field
            virtual tmp<labelField> internalFieldTransfer
            (
                const Pstream::commsTypes commsType,
                const labelUList& iF
            ) const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
