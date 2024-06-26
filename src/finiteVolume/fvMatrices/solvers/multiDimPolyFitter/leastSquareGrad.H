/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2020 DLR
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
    Foam::leastSquareGrad

Description
    Estimates the gradient with a least square scheme in a cell

    Original code supplied by Henning Scheufler, DLR (2019)

SourceFiles
    leastSquareGrad.C

\*---------------------------------------------------------------------------*/

#ifndef leastSquareGrad_H
#define leastSquareGrad_H

#include "fvMesh/fvMesh.H"
#include "fields/volFields/volFields.H"
#include "fields/surfaceFields/surfaceFields.H"
#include "db/IOstreams/Fstreams/OFstream.H"
#include "fvMatrices/solvers/multiDimPolyFitter/multiDimPolyFitter.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class leastSquareGrad Declaration
\*---------------------------------------------------------------------------*/
template<class T>
class leastSquareGrad
{
    // Private Data

        multiDimPolyFitter<T> polyFitter_;

        labelVector geomDir_;

        label nDims_;


public:

    // Constructors

        //- Construct from components
        leastSquareGrad(const word& functionName, const labelVector& geomDir);


    // Member Functions

        //- Return the gradient of the cell
        typename outerProduct<vector, T>::type grad
        (
            const List<vector>& positions,
            const List<T>& listValue
        );

        Map<typename outerProduct<vector, T>::type> grad
        (
            const Map<List<vector>>& positions,
            const Map<List<T>>& listValue
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
