/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
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
    Foam::linearInterpolationWeights

Description

SourceFiles
    linearInterpolationWeights.C

\*---------------------------------------------------------------------------*/

#ifndef linearInterpolationWeights_H
#define linearInterpolationWeights_H

#include "interpolations/interpolationWeights/interpolationWeights/interpolationWeights.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                  Class linearInterpolationWeights Declaration
\*---------------------------------------------------------------------------*/

class linearInterpolationWeights
:
    public interpolationWeights
{
    // Private Data

        //- Cached index in samples from previous invocation
        mutable label index_;


    // Private Member Functions

        //- Get weights of i and i+1 to calculate integration from t to
        //- samples_[i+1]
        Pair<scalar> integrationWeights(const label i, const scalar t) const;


public:

    //- Runtime type information
    TypeName("linear");


    // Constructors

        //- Construct from components
        explicit linearInterpolationWeights(const scalarField& samples);


    //- Destructor
    virtual ~linearInterpolationWeights() = default;


    // Member Functions

        //- Calculate weights and indices to calculate t from samples.
        //  \return true if indices changed.
        virtual bool valueWeights
        (
            const scalar t,
            labelList& indices,
            scalarField& weights
        ) const;

        //- Calculate weights and indices to calculate integrand of t1..t2
        //- from samples.
        //  \return true if indices changed.
        virtual bool integrationWeights
        (
            const scalar t1,
            const scalar t2,
            labelList& indices,
            scalarField& weights
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
