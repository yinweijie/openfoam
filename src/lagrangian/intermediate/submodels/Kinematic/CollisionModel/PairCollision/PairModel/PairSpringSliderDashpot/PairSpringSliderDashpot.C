/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2018-2019 OpenCFD Ltd.
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

#include "submodels/Kinematic/CollisionModel/PairCollision/PairModel/PairSpringSliderDashpot/PairSpringSliderDashpot.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class CloudType>
void Foam::PairSpringSliderDashpot<CloudType>::findMinMaxProperties
(
    scalar& RMin,
    scalar& rhoMax,
    scalar& UMagMax
) const
{
    RMin = VGREAT;
    rhoMax = -VGREAT;
    UMagMax = -VGREAT;

    for (const typename CloudType::parcelType& p : this->owner())
    {
        // Finding minimum diameter to avoid excessive arithmetic

        scalar dEff = p.d();

        if (useEquivalentSize_)
        {
            dEff *= cbrt(p.nParticle()*volumeFactor_);
        }

        RMin = min(dEff, RMin);

        rhoMax = max(p.rho(), rhoMax);

        UMagMax = max
        (
            mag(p.U()) + mag(p.omega())*dEff/2,
            UMagMax
        );
    }

    // Transform the minimum diameter into minimum radius
    //     rMin = dMin/2
    // then rMin into minimum R,
    //     1/RMin = 1/rMin + 1/rMin,
    //     RMin = rMin/2 = dMin/4

    RMin /= 4.0;

    // Multiply by two to create the worst-case relative velocity

    UMagMax = 2*UMagMax;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::PairSpringSliderDashpot<CloudType>::PairSpringSliderDashpot
(
    const dictionary& dict,
    CloudType& cloud
)
:
    PairModel<CloudType>(dict, cloud, typeName),
    Estar_(),
    Gstar_(),
    alpha_(this->coeffDict().getScalar("alpha")),
    b_(this->coeffDict().getScalar("b")),
    mu_(this->coeffDict().getScalar("mu")),
    cohesionEnergyDensity_
    (
        this->coeffDict().getScalar("cohesionEnergyDensity")
    ),
    cohesion_(false),
    collisionResolutionSteps_
    (
        this->coeffDict().getScalar("collisionResolutionSteps")
    ),
    volumeFactor_(1.0),
    useEquivalentSize_(Switch(this->coeffDict().lookup("useEquivalentSize")))
{
    if (useEquivalentSize_)
    {
        this->coeffDict().readEntry("volumeFactor", volumeFactor_);
    }

    scalar nu = this->owner().constProps().poissonsRatio();

    scalar E = this->owner().constProps().youngsModulus();

    Estar_ = E/(2.0*(1.0 - sqr(nu)));

    scalar G = E/(2.0*(1.0 + nu));

    Gstar_ = G/(2.0*(2.0 - nu));

    cohesion_ = (mag(cohesionEnergyDensity_) > VSMALL);
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
bool Foam::PairSpringSliderDashpot<CloudType>::controlsTimestep() const
{
    return true;
}


template<class CloudType>
Foam::label Foam::PairSpringSliderDashpot<CloudType>::nSubCycles() const
{
    if (!(this->owner().size()))
    {
        return 1;
    }

    scalar RMin;
    scalar rhoMax;
    scalar UMagMax;

    findMinMaxProperties(RMin, rhoMax, UMagMax);

    // Note:  pi^(7/5)*(5/4)^(2/5) = 5.429675
    scalar minCollisionDeltaT =
        5.429675
       *RMin
       *pow(rhoMax/(Estar_*sqrt(UMagMax) + VSMALL), 0.4)
       /collisionResolutionSteps_;

    return ceil(this->owner().time().deltaTValue()/minCollisionDeltaT);
}


template<class CloudType>
void Foam::PairSpringSliderDashpot<CloudType>::evaluatePair
(
    typename CloudType::parcelType& pA,
    typename CloudType::parcelType& pB
) const
{
    vector r_AB = (pA.position() - pB.position());

    scalar dAEff = pA.d();

    if (useEquivalentSize_)
    {
        dAEff *= cbrt(pA.nParticle()*volumeFactor_);
    }

    scalar dBEff = pB.d();

    if (useEquivalentSize_)
    {
        dBEff *= cbrt(pB.nParticle()*volumeFactor_);
    }

    scalar r_AB_mag = mag(r_AB);

    scalar normalOverlapMag = 0.5*(dAEff + dBEff) - r_AB_mag;

    if (normalOverlapMag > 0)
    {
        // Particles in collision

        // Force coefficient
        scalar forceCoeff = this->forceCoeff(pA, pB);

        vector rHat_AB = r_AB/(r_AB_mag + VSMALL);

        vector U_AB = pA.U() - pB.U();

        // Effective radius
        scalar R = 0.5*dAEff*dBEff/(dAEff + dBEff);

        // Effective mass
        scalar M = pA.mass()*pB.mass()/(pA.mass() + pB.mass());

        scalar kN = (4.0/3.0)*sqrt(R)*Estar_;

        scalar etaN = alpha_*sqrt(M*kN)*pow025(normalOverlapMag);

        // Normal force
        vector fN_AB =
            rHat_AB
           *(kN*pow(normalOverlapMag, b_) - etaN*(U_AB & rHat_AB));

        // Cohesion force, energy density multiplied by the area of
        // particle-particle overlap
        if (cohesion_)
        {
            fN_AB +=
                -cohesionEnergyDensity_
                *overlapArea(dAEff/2.0, dBEff/2.0, r_AB_mag)
                *rHat_AB;
        }

        fN_AB *= forceCoeff;

        pA.f() += fN_AB;
        pB.f() += -fN_AB;

        vector USlip_AB =
            U_AB - (U_AB & rHat_AB)*rHat_AB
          - ((dAEff/2*pA.omega() + dBEff/2*pB.omega()) ^ rHat_AB);

        scalar deltaT = this->owner().mesh().time().deltaTValue();

        vector& tangentialOverlap_AB =
            pA.collisionRecords().matchPairRecord
            (
                pB.origProc(),
                pB.origId()
            ).collisionData();

        vector& tangentialOverlap_BA =
            pB.collisionRecords().matchPairRecord
            (
                pA.origProc(),
                pA.origId()
            ).collisionData();

        vector deltaTangentialOverlap_AB = USlip_AB*deltaT;

        tangentialOverlap_AB += deltaTangentialOverlap_AB;
        tangentialOverlap_BA += -deltaTangentialOverlap_AB;

        scalar tangentialOverlapMag = mag(tangentialOverlap_AB);

        if (tangentialOverlapMag > VSMALL)
        {
            scalar kT = 8.0*sqrt(R*normalOverlapMag)*Gstar_;

            scalar etaT = etaN;

            // Tangential force
            vector fT_AB;

            if (kT*tangentialOverlapMag > mu_*mag(fN_AB))
            {
                // Tangential force greater than sliding friction,
                // particle slips

                fT_AB = -mu_*mag(fN_AB)*USlip_AB/mag(USlip_AB);

                tangentialOverlap_AB = Zero;
                tangentialOverlap_BA = Zero;
            }
            else
            {
                fT_AB = - kT*tangentialOverlap_AB - etaT*USlip_AB;
            }

            fT_AB *= forceCoeff;

            pA.f() += fT_AB;
            pB.f() += -fT_AB;

            pA.torque() += (dAEff/2*-rHat_AB) ^ fT_AB;
            pB.torque() += (dBEff/2*rHat_AB) ^ -fT_AB;
        }
    }
}


// ************************************************************************* //
