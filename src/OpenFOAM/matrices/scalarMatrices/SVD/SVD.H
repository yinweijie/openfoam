/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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
    Foam::SVD

Description
    Singular value decomposition of a rectangular matrix.

SourceFiles
    SVDI.H
    SVD.C

\*---------------------------------------------------------------------------*/

#ifndef SVD_H
#define SVD_H

#include "matrices/scalarMatrices/scalarMatrices.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

/*---------------------------------------------------------------------------*\
                      Class SVD Declaration
\*---------------------------------------------------------------------------*/

class SVD
{
    // Private data

        //- Rectangular matrix with the same dimensions as the input
        scalarRectangularMatrix U_;

        //- Square matrix V
        scalarRectangularMatrix V_;

        //- The singular values
        DiagonalMatrix<scalar> S_;

        //- Convergence flag
        bool converged_;

        //- The number of zero singular values
        label nZeros_;


    // Private Member Functions

        //- No copy construct
        SVD(const SVD&) = delete;

        //- No copy assignment
        void operator=(const SVD&) = delete;

        template<class T>
        inline const T sign(const T& a, const T& b);


public:

    // Constructors

        //- Construct from a rectangular Matrix
        SVD(const scalarRectangularMatrix& A, const scalar minCondition = 0);


    // Access functions

        //- Return U
        inline const scalarRectangularMatrix& U() const;

        //- Return the square matrix V
        inline const scalarRectangularMatrix& V() const;

        //- Return the singular values
        inline const scalarDiagonalMatrix& S() const;

        //- Return the minimum non-zero singular value
        inline bool converged() const;

        //- Return the number of zero singular values
        inline label nZeros() const;

        //- Return the minimum non-zero singular value
        inline scalar minNonZeroS() const;

        //- Return the matrix product V S^(-1) U^T (the pseudo inverse)
        scalarRectangularMatrix VSinvUt() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "matrices/scalarMatrices/SVD/SVDI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
