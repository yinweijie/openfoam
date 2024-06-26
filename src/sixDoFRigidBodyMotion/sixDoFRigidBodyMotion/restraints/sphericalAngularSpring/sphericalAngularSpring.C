/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2018-2020 OpenCFD Ltd.
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

#include "sixDoFRigidBodyMotion/restraints/sphericalAngularSpring/sphericalAngularSpring.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "sixDoFRigidBodyMotion/sixDoFRigidBodyMotion.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace sixDoFRigidBodyMotionRestraints
{
    defineTypeNameAndDebug(sphericalAngularSpring, 0);

    addToRunTimeSelectionTable
    (
        sixDoFRigidBodyMotionRestraint,
        sphericalAngularSpring,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::sixDoFRigidBodyMotionRestraints::sphericalAngularSpring::
sphericalAngularSpring
(
    const word& name,
    const dictionary& sDoFRBMRDict
)
:
    sixDoFRigidBodyMotionRestraint(name, sDoFRBMRDict),
    refQ_(),
    stiffness_(),
    damping_()
{
    read(sDoFRBMRDict);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::sixDoFRigidBodyMotionRestraints::sphericalAngularSpring::
~sphericalAngularSpring()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::sixDoFRigidBodyMotionRestraints::sphericalAngularSpring::restrain
(
    const sixDoFRigidBodyMotion& motion,
    vector& restraintPosition,
    vector& restraintForce,
    vector& restraintMoment
) const
{
    restraintMoment = Zero;

    for (direction cmpt=0; cmpt<vector::nComponents; cmpt++)
    {
        vector axis = Zero;
        axis[cmpt] = 1;

        vector refDir = Zero;
        refDir[(cmpt + 1) % 3] = 1;

        vector newDir = motion.orientation() & refDir;

        axis = (refQ_ & axis);
        refDir = (refQ_ & refDir);
        newDir -= (axis & newDir)*axis;

        restraintMoment += -stiffness_*(refDir ^ newDir);
    }

    restraintMoment += -damping_*motion.omega();

    restraintForce = Zero;

    // Not needed to be altered as restraintForce is zero, but set to
    // centreOfRotation to be sure of no spurious moment
    restraintPosition = motion.centreOfRotation();

    if (motion.report())
    {
        Info<< " moment " << restraintMoment
            << endl;
    }
}


bool Foam::sixDoFRigidBodyMotionRestraints::sphericalAngularSpring::read
(
    const dictionary& sDoFRBMRDict
)
{
    sixDoFRigidBodyMotionRestraint::read(sDoFRBMRDict);

    refQ_ = sDoFRBMRCoeffs_.getOrDefault<tensor>("referenceOrientation", I);

    if (mag(mag(refQ_) - sqrt(3.0)) > ROOTSMALL)
    {
        FatalErrorInFunction
            << "referenceOrientation " << refQ_ << " is not a rotation tensor. "
            << "mag(referenceOrientation) - sqrt(3) = "
            << mag(refQ_) - sqrt(3.0) << nl
            << exit(FatalError);
    }

    sDoFRBMRCoeffs_.readEntry("stiffness", stiffness_);
    sDoFRBMRCoeffs_.readEntry("damping", damping_);

    return true;
}


void Foam::sixDoFRigidBodyMotionRestraints::sphericalAngularSpring::write
(
    Ostream& os
) const
{
    os.writeEntry("referenceOrientation", refQ_);
    os.writeEntry("stiffness", stiffness_);
    os.writeEntry("damping", damping_);
}


// ************************************************************************* //
