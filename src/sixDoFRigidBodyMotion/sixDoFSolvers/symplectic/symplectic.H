/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2016 OpenFOAM Foundation
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
    Foam::sixDoFSolvers::symplectic

Group
    grpSixDoFRigidBodySolvers

Description
    Symplectic 2nd-order explicit time-integrator for 6DoF solid-body motion.

    Reference:
    \verbatim
        Dullweber, A., Leimkuhler, B., & McLachlan, R. (1997).
        Symplectic splitting methods for rigid body molecular dynamics.
        The Journal of chemical physics, 107(15), 5840-5851.
    \endverbatim

    Can only be used for explicit integration of the motion of the body,
    i.e. may only be called once per time-step, no outer-correctors may be
    applied.  For implicit integration with outer-correctors choose either
    CrankNicolson or Newmark schemes.

    Example specification in dynamicMeshDict:
    \verbatim
    solver
    {
        type    symplectic;
    }
    \endverbatim

See also
    Foam::sixDoFSolvers::CrankNicolson
    Foam::sixDoFSolvers::Newmark

SourceFiles
    symplectic.C

\*---------------------------------------------------------------------------*/

#ifndef symplectic_H
#define symplectic_H

#include "sixDoFSolvers/sixDoFSolver/sixDoFSolver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace sixDoFSolvers
{

/*---------------------------------------------------------------------------*\
                           Class symplectic Declaration
\*---------------------------------------------------------------------------*/

class symplectic
:
    public sixDoFSolver
{

public:

    //- Runtime type information
    TypeName("symplectic");


    // Constructors

        //- Construct from a dictionary and the body
        symplectic
        (
            const dictionary& dict,
            sixDoFRigidBodyMotion& body
        );

        //- Construct and return a clone
        virtual autoPtr<sixDoFSolver> clone() const
        {
            return autoPtr<sixDoFSolver>::NewFrom<symplectic>(dict_, body_);
        }


    //- Destructor
    virtual ~symplectic();


    // Member Functions

        //- Drag coefficient
        virtual void solve
        (
            bool firstIter,
            const vector& fGlobal,
            const vector& tauGlobal,
            scalar deltaT,
            scalar deltaT0
        );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace sixDoFSolvers
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
