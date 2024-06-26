/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2021 OpenCFD Ltd.
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

#include "matrices/LduMatrixCaseDir/LduMatrix/SolverPerformanceDeCased.H"
#include "db/IOstreams/IOstreams.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class Type>
bool Foam::SolverPerformance<Type>::checkSingularity
(
    const Type& wApA
)
{
    for(direction cmpt=0; cmpt<pTraits<Type>::nComponents; cmpt++)
    {
        singular_[cmpt] =
            component(wApA, cmpt) < vsmall_;
    }

    return singular();
}


template<class Type>
bool Foam::SolverPerformance<Type>::singular() const
{
    for(direction cmpt=0; cmpt<pTraits<Type>::nComponents; cmpt++)
    {
        if (!singular_[cmpt]) return false;
    }

    return true;
}


template<class Type>
bool Foam::SolverPerformance<Type>::checkConvergence
(
    const Type& Tolerance,
    const Type& RelTolerance,
    const int logLevel
)
{
    if ((logLevel >= 2) || (debug >= 2))
    {
        Info<< solverName_
            << ":  Iteration " << nIterations_
            << " residual = " << finalResidual_
            << endl;
    }

    converged_ =
    (
        finalResidual_ < Tolerance
     || (
            RelTolerance > small_*pTraits<Type>::one
         && finalResidual_ < cmptMultiply(RelTolerance, initialResidual_)
        )
    );

    return converged_;
}


template<class Type>
void Foam::SolverPerformance<Type>::print
(
    Ostream& os
) const
{
    for(direction cmpt=0; cmpt<pTraits<Type>::nComponents; cmpt++)
    {
        os  << solverName_ << ":  Solving for ";
        if (pTraits<Type>::nComponents == 1)
        {
            os  << fieldName_;
        }
        else
        {
            os  << word(fieldName_ + pTraits<Type>::componentNames[cmpt]);
        }

        if (singular_[cmpt])
        {
            os  << ":  solution singularity" << endl;
        }
        else
        {
            os  << ", Initial residual = " << component(initialResidual_, cmpt)
                << ", Final residual = " << component(finalResidual_, cmpt)
                << ", No Iterations " << nIterations_
                << endl;
        }
    }
}


template<class Type>
void Foam::SolverPerformance<Type>::replace
(
    const Foam::label cmpt,
    const Foam::SolverPerformance<typename pTraits<Type>::cmptType>& sp
)
{
    initialResidual_.replace(cmpt, sp.initialResidual());
    finalResidual_.replace(cmpt, sp.finalResidual());
    nIterations_.replace(cmpt, sp.nIterations());
    singular_[cmpt] = sp.singular();
}


template<class Type>
Foam::SolverPerformance<typename Foam::pTraits<Type>::cmptType>
Foam::SolverPerformance<Type>::max()
{
    return SolverPerformance<typename pTraits<Type>::cmptType>
    (
        solverName_,
        fieldName_,
        cmptMax(initialResidual_),
        cmptMax(finalResidual_),
        cmptMax(nIterations_),
        converged_,
        singular()
    );
}


template<class Type>
bool Foam::SolverPerformance<Type>::operator!=
(
    const SolverPerformance<Type>& sp
) const
{
    return
    (
        solverName()      != sp.solverName()
     || fieldName()       != sp.fieldName()
     || initialResidual() != sp.initialResidual()
     || finalResidual()   != sp.finalResidual()
     || nIterations()     != sp.nIterations()
     || converged()       != sp.converged()
     || singular()        != sp.singular()
    );
}


template<class Type>
typename Foam::SolverPerformance<Type> Foam::max
(
    const typename Foam::SolverPerformance<Type>& sp1,
    const typename Foam::SolverPerformance<Type>& sp2
)
{
    return SolverPerformance<Type>
    (
        sp1.solverName(),
        sp1.fieldName_,
        max(sp1.initialResidual(), sp2.initialResidual()),
        max(sp1.finalResidual(), sp2.finalResidual()),
        max(sp1.nIterations(), sp2.nIterations()),
        sp1.converged() && sp2.converged(),
        sp1.singular() || sp2.singular()
    );
}


template<class Type>
Foam::Istream& Foam::operator>>
(
    Istream& is,
    typename Foam::SolverPerformance<Type>& sp
)
{
    is.readBegin("SolverPerformance");
    is  >> sp.solverName_
        >> sp.fieldName_
        >> sp.initialResidual_
        >> sp.finalResidual_
        >> sp.nIterations_
        >> sp.converged_
        >> sp.singular_;
    is.readEnd("SolverPerformance");

    return is;
}


template<class Type>
Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const typename Foam::SolverPerformance<Type>& sp
)
{
    os  << token::BEGIN_LIST
        << sp.solverName_ << token::SPACE
        << sp.fieldName_ << token::SPACE
        << sp.initialResidual_ << token::SPACE
        << sp.finalResidual_ << token::SPACE
        << sp.nIterations_ << token::SPACE
        << sp.converged_ << token::SPACE
        << sp.singular_ << token::SPACE
        << token::END_LIST;

    return os;
}


// ************************************************************************* //
