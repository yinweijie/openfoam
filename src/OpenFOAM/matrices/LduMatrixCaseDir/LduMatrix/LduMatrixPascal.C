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

\*---------------------------------------------------------------------------*/

#include "matrices/lduMatrix/lduMatrix/lduMatrix.H"
#include "db/IOstreams/IOstreams.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type, class DType, class LUType>
Foam::LduMatrix<Type, DType, LUType>::LduMatrix(const lduMesh& mesh)
:
    lduMesh_(mesh),
    diagPtr_(nullptr),
    upperPtr_(nullptr),
    lowerPtr_(nullptr),
    sourcePtr_(nullptr),
    interfaces_(0),
    interfacesUpper_(0),
    interfacesLower_(0)
{}


template<class Type, class DType, class LUType>
Foam::LduMatrix<Type, DType, LUType>::LduMatrix(const LduMatrix& A)
:
    lduMesh_(A.lduMesh_),
    diagPtr_(nullptr),
    upperPtr_(nullptr),
    lowerPtr_(nullptr),
    sourcePtr_(nullptr),
    interfaces_(0),
    interfacesUpper_(0),
    interfacesLower_(0)
{
    if (A.diagPtr_)
    {
        diagPtr_ = new Field<DType>(*(A.diagPtr_));
    }

    if (A.upperPtr_)
    {
        upperPtr_ = new Field<LUType>(*(A.upperPtr_));
    }

    if (A.lowerPtr_)
    {
        lowerPtr_ = new Field<LUType>(*(A.lowerPtr_));
    }

    if (A.sourcePtr_)
    {
        sourcePtr_ = new Field<Type>(*(A.sourcePtr_));
    }
}


template<class Type, class DType, class LUType>
Foam::LduMatrix<Type, DType, LUType>::LduMatrix(LduMatrix& A, bool reuse)
:
    lduMesh_(A.lduMesh_),
    diagPtr_(nullptr),
    upperPtr_(nullptr),
    lowerPtr_(nullptr),
    sourcePtr_(nullptr),
    interfaces_(0),
    interfacesUpper_(0),
    interfacesLower_(0)
{
    if (reuse)
    {
        if (A.diagPtr_)
        {
            diagPtr_ = A.diagPtr_;
            A.diagPtr_ = nullptr;
        }

        if (A.upperPtr_)
        {
            upperPtr_ = A.upperPtr_;
            A.upperPtr_ = nullptr;
        }

        if (A.lowerPtr_)
        {
            lowerPtr_ = A.lowerPtr_;
            A.lowerPtr_ = nullptr;
        }

        if (A.sourcePtr_)
        {
            sourcePtr_ = A.sourcePtr_;
            A.sourcePtr_ = nullptr;
        }
    }
    else
    {
        if (A.diagPtr_)
        {
            diagPtr_ = new Field<DType>(*(A.diagPtr_));
        }

        if (A.upperPtr_)
        {
            upperPtr_ = new Field<LUType>(*(A.upperPtr_));
        }

        if (A.lowerPtr_)
        {
            lowerPtr_ = new Field<LUType>(*(A.lowerPtr_));
        }

        if (A.sourcePtr_)
        {
            sourcePtr_ = new Field<Type>(*(A.sourcePtr_));
        }
    }
}


template<class Type, class DType, class LUType>
Foam::LduMatrix<Type, DType, LUType>::LduMatrix
(
    const lduMesh& mesh,
    Istream& is
)
:
    lduMesh_(mesh),
    diagPtr_(new Field<DType>(is)),
    upperPtr_(new Field<LUType>(is)),
    lowerPtr_(new Field<LUType>(is)),
    sourcePtr_(new Field<Type>(is)),
    interfaces_(0),
    interfacesUpper_(0),
    interfacesLower_(0)
{}


// * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * * //

template<class Type, class DType, class LUType>
Foam::LduMatrix<Type, DType, LUType>::~LduMatrix()
{
    if (diagPtr_)
    {
        delete diagPtr_;
    }

    if (upperPtr_)
    {
        delete upperPtr_;
    }

    if (lowerPtr_)
    {
        delete lowerPtr_;
    }

    if (sourcePtr_)
    {
        delete sourcePtr_;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type, class DType, class LUType>
Foam::Field<DType>& Foam::LduMatrix<Type, DType, LUType>::diag()
{
    if (!diagPtr_)
    {
        diagPtr_ = new Field<DType>(lduAddr().size(), Zero);
    }

    return *diagPtr_;
}


template<class Type, class DType, class LUType>
Foam::Field<LUType>& Foam::LduMatrix<Type, DType, LUType>::upper()
{
    if (!upperPtr_)
    {
        if (lowerPtr_)
        {
            upperPtr_ = new Field<LUType>(*lowerPtr_);
        }
        else
        {
            upperPtr_ = new Field<LUType>
            (
                lduAddr().lowerAddr().size(),
                Zero
            );
        }
    }

    return *upperPtr_;
}


template<class Type, class DType, class LUType>
Foam::Field<LUType>& Foam::LduMatrix<Type, DType, LUType>::lower()
{
    if (!lowerPtr_)
    {
        if (upperPtr_)
        {
            lowerPtr_ = new Field<LUType>(*upperPtr_);
        }
        else
        {
            lowerPtr_ = new Field<LUType>
            (
                lduAddr().lowerAddr().size(),
                Zero
            );
        }
    }

    return *lowerPtr_;
}


template<class Type, class DType, class LUType>
Foam::Field<Type>& Foam::LduMatrix<Type, DType, LUType>::source()
{
    if (!sourcePtr_)
    {
        sourcePtr_ = new Field<Type>(lduAddr().size(), Zero);
    }

    return *sourcePtr_;
}


template<class Type, class DType, class LUType>
const Foam::Field<DType>& Foam::LduMatrix<Type, DType, LUType>::diag() const
{
    if (!diagPtr_)
    {
        FatalErrorInFunction
            << "diagPtr_ unallocated"
            << abort(FatalError);
    }

    return *diagPtr_;
}


template<class Type, class DType, class LUType>
const Foam::Field<LUType>& Foam::LduMatrix<Type, DType, LUType>::upper() const
{
    if (!lowerPtr_ && !upperPtr_)
    {
        FatalErrorInFunction
            << "lowerPtr_ or upperPtr_ unallocated"
            << abort(FatalError);
    }

    if (upperPtr_)
    {
        return *upperPtr_;
    }
    else
    {
        return *lowerPtr_;
    }
}


template<class Type, class DType, class LUType>
const Foam::Field<LUType>& Foam::LduMatrix<Type, DType, LUType>::lower() const
{
    if (!lowerPtr_ && !upperPtr_)
    {
        FatalErrorInFunction
            << "lowerPtr_ or upperPtr_ unallocated"
            << abort(FatalError);
    }

    if (lowerPtr_)
    {
        return *lowerPtr_;
    }
    else
    {
        return *upperPtr_;
    }
}


template<class Type, class DType, class LUType>
const Foam::Field<Type>& Foam::LduMatrix<Type, DType, LUType>::source() const
{
    if (!sourcePtr_)
    {
        FatalErrorInFunction
            << "sourcePtr_ unallocated"
            << abort(FatalError);
    }

    return *sourcePtr_;
}


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template<class Type, class DType, class LUType>
Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const LduMatrix<Type, DType, LUType>& ldum
)
{
    if (ldum.diagPtr_)
    {
        os  << "Diagonal = "
            << *ldum.diagPtr_
            << endl << endl;
    }

    if (ldum.upperPtr_)
    {
        os  << "Upper triangle = "
            << *ldum.upperPtr_
            << endl << endl;
    }

    if (ldum.lowerPtr_)
    {
        os  << "Lower triangle = "
            << *ldum.lowerPtr_
            << endl << endl;
    }

    if (ldum.sourcePtr_)
    {
        os  << "Source = "
            << *ldum.sourcePtr_
            << endl << endl;
    }

    os.check(FUNCTION_NAME);

    return os;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "matrices/LduMatrixCaseDir/LduMatrix/LduMatrixOperationsPascal.C"
#include "matrices/LduMatrixCaseDir/LduMatrix/LduMatrixATmulPascal.C"
#include "matrices/LduMatrixCaseDir/LduMatrix/LduMatrixUpdateMatrixInterfacesPascal.C"
#include "matrices/LduMatrixCaseDir/LduMatrix/LduMatrixPreconditionerPascal.C"
#include "matrices/LduMatrixCaseDir/LduMatrix/LduMatrixSmootherPascal.C"
#include "matrices/LduMatrixCaseDir/LduMatrix/LduMatrixSolverPascal.C"

// ************************************************************************* //
