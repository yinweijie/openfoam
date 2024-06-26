/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2014 OpenFOAM Foundation
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

#include "submodels/BreakupModel/ETAB/ETAB.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::ETAB<CloudType>::ETAB
(
    const dictionary& dict,
    CloudType& owner
)
:
    BreakupModel<CloudType>(dict, owner, typeName, true),
    k1_(0.2),
    k2_(0.2),
    WeTransition_(100.0),
    AWe_(0.0)
{
    if (!this->defaultCoeffs(true))
    {
        this->coeffDict().readEntry("k1", k1_);
        this->coeffDict().readEntry("k2", k2_);
        this->coeffDict().readEntry("WeTransition", WeTransition_);
    }

    scalar k21 = k2_/k1_;
    AWe_ = (k21*sqrt(WeTransition_) - 1.0)/pow4(WeTransition_);
}


template<class CloudType>
Foam::ETAB<CloudType>::ETAB(const ETAB<CloudType>& bum)
:
    BreakupModel<CloudType>(bum),
    k1_(bum.k1_),
    k2_(bum.k2_),
    WeTransition_(bum.WeTransition_),
    AWe_(bum.AWe_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::ETAB<CloudType>::~ETAB()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
bool Foam::ETAB<CloudType>::update
(
    const scalar dt,
    const vector& g,
    scalar& d,
    scalar& tc,
    scalar& ms,
    scalar& nParticle,
    scalar& KHindex,
    scalar& y,
    scalar& yDot,
    const scalar d0,
    const scalar rho,
    const scalar mu,
    const scalar sigma,
    const vector& U,
    const scalar rhoc,
    const scalar muc,
    const vector& Urel,
    const scalar Urmag,
    const scalar tMom,
    scalar& dChild,
    scalar& massChild
)
{
    scalar r  = 0.5*d;
    scalar r2 = r*r;
    scalar r3 = r*r2;

    scalar semiMass = nParticle*pow3(d);

    // inverse of characteristic viscous damping time
    scalar rtd = 0.5*this->TABCmu_*mu/(rho*r2);

    // oscillation frequency (squared)
    scalar omega2 = this->TABComega_*sigma/(rho*r3) - rtd*rtd;

    if (omega2 > 0)
    {
        scalar omega = sqrt(omega2);
        scalar romega = 1.0/omega;

        scalar We = rhoc*sqr(Urmag)*r/sigma;
        scalar Wetmp = We/this->TABtwoWeCrit_;

        scalar y1 = y - Wetmp;
        scalar y2 = yDot*romega;

        scalar a = sqrt(y1*y1 + y2*y2);

        // scotty we may have break-up
        if (a + Wetmp > 1.0)
        {
            scalar phic = y1/a;

            // constrain phic within -1 to 1
            phic = clamp(phic, -1, 1);

            scalar phit = acos(phic);
            scalar phi = phit;
            scalar quad = -y2/a;
            if (quad < 0)
            {
                phi = constant::mathematical::twoPi - phit;
            }

            scalar tb = 0;

            if (mag(y) < 1.0)
            {
                scalar theta = acos((1.0 - Wetmp)/a);

                if (theta < phi)
                {
                    if (constant::mathematical::twoPi - theta >= phi)
                    {
                        theta = -theta;
                    }
                    theta += constant::mathematical::twoPi;
                }
                tb = (theta - phi)*romega;

                // breakup occurs
                if (dt > tb)
                {
                    y = 1.0;
                    yDot = -a*omega*sin(omega*tb + phi);
                }
            }

            // update droplet size
            if (dt > tb)
            {
                scalar sqrtWe = AWe_*pow4(We) + 1.0;
                scalar Kbr = k1_*omega*sqrtWe;

                if (We > WeTransition_)
                {
                    sqrtWe = sqrt(We);
                    Kbr =k2_*omega*sqrtWe;
                }

                scalar cosdtbu = clamp((1.0 - 1.0/Wetmp), -1, 1);
                scalar dtbu = romega*acos(cosdtbu);
                scalar decay = exp(-Kbr*dtbu);

                scalar rNew = decay*r;
                if (rNew < r)
                {
                    d = 2.0*rNew;
                    y = 0.0;
                    yDot = 0.0;
                }
            }
        }
    }
    else
    {
        // reset droplet distortion parameters
        y = 0;
        yDot = 0;
    }

    // update the nParticle count to conserve mass
    nParticle = semiMass/pow3(d);

    // Do not add child parcel
    return false;
}


// ************************************************************************* //
