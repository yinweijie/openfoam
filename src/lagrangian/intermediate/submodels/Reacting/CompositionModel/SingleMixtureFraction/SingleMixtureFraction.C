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

\*---------------------------------------------------------------------------*/

#include "submodels/Reacting/CompositionModel/SingleMixtureFraction/SingleMixtureFraction.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class CloudType>
void Foam::SingleMixtureFraction<CloudType>::constructIds()
{
    forAll(this->phaseProps(), phaseI)
    {
        switch (this->phaseProps()[phaseI].phase())
        {
            case phaseProperties::GAS:
            {
                idGas_ = phaseI;
                break;
            }
            case phaseProperties::LIQUID:
            {
                idLiquid_ = phaseI;
                break;
            }
            case phaseProperties::SOLID:
            {
                idSolid_ = phaseI;
                break;
            }
            default:
            {
                FatalErrorInFunction
                    << "Unknown phase enumeration" << nl << abort(FatalError);
            }
        }
    }

    if (idGas_ < 0)
    {
        FatalErrorInFunction
            << "No gas phase found in phase list:" << nl
            << this->phaseTypes() << exit(FatalError);
    }
    if (idLiquid_ < 0)
    {
        FatalErrorInFunction
            << "No liquid phase found in phase list:" << nl
            << this->phaseTypes() << exit(FatalError);
    }
    if (idSolid_ < 0)
    {
        FatalErrorInFunction
            << "No solid phase found in phase list:" << nl
            << this->phaseTypes() << exit(FatalError);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::SingleMixtureFraction<CloudType>::SingleMixtureFraction
(
    const dictionary& dict,
    CloudType& owner
)
:
    CompositionModel<CloudType>(dict, owner, typeName),

    idGas_(-1),
    idLiquid_(-1),
    idSolid_(-1),

    YMixture0_(3)
{
    constructIds();

    if (this->phaseProps().size() != 3)
    {
        FatalErrorInFunction
            << "Incorrect number of phases:" << nl
            << "    Please specify 1 gas, 1 liquid and 1 solid"
            << exit(FatalError);
    }

    this->coeffDict().readEntry("YGasTot0", YMixture0_[idGas_]);
    this->coeffDict().readEntry("YLiquidTot0", YMixture0_[idLiquid_]);
    this->coeffDict().readEntry("YSolidTot0", YMixture0_[idSolid_]);

    if (mag(sum(YMixture0_) - 1.0) > SMALL)
    {
        FatalErrorInFunction
            << "Sum of phases should be 1. Phase fractions:" << nl
            << YMixture0_ << exit(FatalError);
    }
}


template<class CloudType>
Foam::SingleMixtureFraction<CloudType>::SingleMixtureFraction
(
    const SingleMixtureFraction<CloudType>& cm
)
:
    CompositionModel<CloudType>(cm),
    idGas_(cm.idGas_),
    idLiquid_(cm.idLiquid_),
    idSolid_(cm.idSolid_),
    YMixture0_(cm.YMixture0_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::SingleMixtureFraction<CloudType>::~SingleMixtureFraction()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
const Foam::scalarField&
Foam::SingleMixtureFraction<CloudType>::YMixture0() const
{
    return YMixture0_;
}


template<class CloudType>
Foam::label Foam::SingleMixtureFraction<CloudType>::idGas() const
{
    return idGas_;
}


template<class CloudType>
Foam::label Foam::SingleMixtureFraction<CloudType>::idLiquid() const
{
    return idLiquid_;
}


template<class CloudType>
Foam::label Foam::SingleMixtureFraction<CloudType>::idSolid() const
{
    return idSolid_;
}


// ************************************************************************* //
