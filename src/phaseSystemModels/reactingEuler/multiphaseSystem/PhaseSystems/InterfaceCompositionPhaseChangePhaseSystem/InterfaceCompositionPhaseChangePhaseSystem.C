/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2018 OpenFOAM Foundation
    Copyright (C) 2020 OpenCFD Ltd.
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

#include "PhaseSystems/InterfaceCompositionPhaseChangePhaseSystem/InterfaceCompositionPhaseChangePhaseSystem.H"
#include "interfacialCompositionModels/interfaceCompositionModels/interfaceCompositionModel/interfaceCompositionModel.H"
#include "interfacialCompositionModels/massTransferModels/massTransferModel/massTransferModel.H"

// * * * * * * * * * * * * Private Member Functions * * * * * * * * * * * * //

template<class BasePhaseSystem>
Foam::tmp<Foam::volScalarField>
Foam::InterfaceCompositionPhaseChangePhaseSystem<BasePhaseSystem>::iDmdt
(
    const phasePairKey& key
) const
{
    tmp<volScalarField> tIDmdt = phaseSystem::dmdt(key);

    const phasePair unorderedPair
    (
        this->phases()[key.first()],
        this->phases()[key.second()]
    );

    forAllConstIter(phasePair, unorderedPair, iter)
    {
        const phaseModel& phase = iter();
        const phaseModel& otherPhase = iter.otherPhase();
        const phasePair pair(phase, otherPhase, true);

        if (interfaceCompositionModels_.found(pair))
        {
            const scalar iDmdtSign = Pair<word>::compare(pair, key);

            for
            (
                const word& member
              : interfaceCompositionModels_[pair]->species()
            )
            {
                tIDmdt.ref() +=
                    iDmdtSign
                   *(
                        *(*iDmdtSu_[pair])[member]
                      + *(*iDmdtSp_[pair])[member]*phase.Y(member)
                    );
            }
        }
    }

    return tIDmdt;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class BasePhaseSystem>
Foam::InterfaceCompositionPhaseChangePhaseSystem<BasePhaseSystem>::
InterfaceCompositionPhaseChangePhaseSystem
(
    const fvMesh& mesh
)
:
    BasePhaseSystem(mesh),
    nInterfaceCorrectors_
    (
        this->template getOrDefault<label>("nInterfaceCorrectors", 1)
    )
{
    this->generatePairsAndSubModels
    (
        "interfaceComposition",
        interfaceCompositionModels_
    );

    this->generatePairsAndSubModels
    (
        "massTransfer",
        massTransferModels_,
        false
    );

    // Check that models have been specified in the correct combinations
    forAllConstIter
    (
        interfaceCompositionModelTable,
        interfaceCompositionModels_,
        interfaceCompositionModelIter
    )
    {
        const phasePair& pair =
            this->phasePairs_[interfaceCompositionModelIter.key()];
        const phaseModel& phase = pair.phase1();
        const phaseModel& otherPhase = pair.phase2();

        if (!pair.ordered())
        {
            FatalErrorInFunction
                << "An interfacial composition model is specified for the "
                << "unordered " << pair << " pair. Composition models only "
                << "apply to ordered pairs. An entry for a "
                << phasePairKey("A", "B", true) << " pair means a model for "
                << "the A side of the A-B interface; i.e., \"A in the presence "
                << "of B\""
                << exit(FatalError);
        }


        const phasePairKey key(phase.name(), otherPhase.name());

        if (!this->phasePairs_.found(key))
        {
            FatalErrorInFunction
                << "A mass transfer model the " << key << " pair is not "
                << "specified. This is required by the corresponding interface "
                << "composition model."
                << exit(FatalError);
        }

        const phasePair& uoPair = this->phasePairs_[key];

        if (!massTransferModels_[uoPair][uoPair.index(phase)])
        {
            FatalErrorInFunction
                << "A mass transfer model for the " << pair.phase1().name()
                << " side of the " << uoPair << " pair is not "
                << "specified. This is required by the corresponding interface "
                << "composition model."
                << exit(FatalError);
        }
    }
    forAllConstIter
    (
        massTransferModelTable,
        massTransferModels_,
        massTransferModelIter
    )
    {
        const phasePair& pair =
            this->phasePairs_[massTransferModelIter.key()];

        if (!this->heatTransferModels_.found(pair))
        {
             FatalErrorInFunction
                 << "A heat transfer model for " << pair << " pair is not "
                 << "specified. This is required by the corresponding species "
                 << "transfer model"
                 << exit(FatalError);
        }
    }

    // Generate mass transfer fields, initially assumed to be zero
    forAllConstIter
    (
        interfaceCompositionModelTable,
        interfaceCompositionModels_,
        interfaceCompositionModelIter
    )
    {
        const interfaceCompositionModel& compositionModel =
            interfaceCompositionModelIter();

        const phasePair& pair =
            this->phasePairs_[interfaceCompositionModelIter.key()];

        iDmdtSu_.set(pair, new HashPtrTable<volScalarField>());
        iDmdtSp_.set(pair, new HashPtrTable<volScalarField>());

        for (const word& member : compositionModel.species())
        {
            iDmdtSu_[pair]->set
            (
                member,
                new volScalarField
                (
                    IOobject
                    (
                        IOobject::groupName("iDmdtSu", pair.name()),
                        this->mesh().time().timeName(),
                        this->mesh()
                    ),
                    this->mesh(),
                    dimensionedScalar(dimDensity/dimTime)
                )
            );

            iDmdtSp_[pair]->set
            (
                member,
                new volScalarField
                (
                    IOobject
                    (
                        IOobject::groupName("iDmdtSp", pair.name()),
                        this->mesh().time().timeName(),
                        this->mesh()
                    ),
                    this->mesh(),
                    dimensionedScalar(dimDensity/dimTime)
                )
            );
        }
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class BasePhaseSystem>
Foam::InterfaceCompositionPhaseChangePhaseSystem<BasePhaseSystem>::
~InterfaceCompositionPhaseChangePhaseSystem()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

template<class BasePhaseSystem>
Foam::tmp<Foam::volScalarField>
Foam::InterfaceCompositionPhaseChangePhaseSystem<BasePhaseSystem>::dmdt
(
    const phasePairKey& key
) const
{
    return BasePhaseSystem::dmdt(key) + this->iDmdt(key);
}


template<class BasePhaseSystem>
Foam::PtrList<Foam::volScalarField>
Foam::InterfaceCompositionPhaseChangePhaseSystem<BasePhaseSystem>::dmdts() const
{
    PtrList<volScalarField> dmdts(BasePhaseSystem::dmdts());

    forAllConstIter
    (
        interfaceCompositionModelTable,
        interfaceCompositionModels_,
        interfaceCompositionModelIter
    )
    {
        const interfaceCompositionModel& compositionModel =
            interfaceCompositionModelIter();

        const phasePair& pair =
            this->phasePairs_[interfaceCompositionModelIter.key()];
        const phaseModel& phase = pair.phase1();
        const phaseModel& otherPhase = pair.phase2();

        for (const word& member : compositionModel.species())
        {
            const volScalarField iDmdt
            (
                *(*iDmdtSu_[pair])[member]
              + *(*iDmdtSp_[pair])[member]*phase.Y(member)
            );

            this->addField(phase, "dmdt", iDmdt, dmdts);
            this->addField(otherPhase, "dmdt", - iDmdt, dmdts);
        }
    }

    return dmdts;
}


template<class BasePhaseSystem>
Foam::autoPtr<Foam::phaseSystem::massTransferTable>
Foam::InterfaceCompositionPhaseChangePhaseSystem<BasePhaseSystem>::
massTransfer() const
{
    autoPtr<phaseSystem::massTransferTable> eqnsPtr =
        BasePhaseSystem::massTransfer();

    phaseSystem::massTransferTable& eqns = eqnsPtr();

    // Sum up the contribution from each interface composition model
    forAllConstIter
    (
        interfaceCompositionModelTable,
        interfaceCompositionModels_,
        interfaceCompositionModelIter
    )
    {
        const interfaceCompositionModel& compositionModel =
            interfaceCompositionModelIter();

        const phasePair& pair =
            this->phasePairs_[interfaceCompositionModelIter.key()];
        const phaseModel& phase = pair.phase1();
        const phaseModel& otherPhase = pair.phase2();
        const phasePair& unorderedPair =
            this->phasePairs_[phasePair(phase, otherPhase)];

        const volScalarField& Tf(*this->Tf_[unorderedPair]);

        const volScalarField K
        (
            massTransferModels_[unorderedPair][unorderedPair.index(phase)]->K()
        );

        for
        (
            const word& member
          : compositionModel.species()
        )
        {
            const word name(IOobject::groupName(member, phase.name()));
            const word otherName
            (
                IOobject::groupName(member, otherPhase.name())
            );

            const volScalarField KD(K*compositionModel.D(member));

            const volScalarField Yf(compositionModel.Yf(member, Tf));

            *(*iDmdtSu_[pair])[member] = phase.rho()*KD*Yf;
            *(*iDmdtSp_[pair])[member] = - phase.rho()*KD;

            const fvScalarMatrix eqn
            (
                *(*iDmdtSu_[pair])[member]
              + fvm::Sp(*(*iDmdtSp_[pair])[member], phase.Y(member))
            );

            const volScalarField iDmdt
            (
                *(*iDmdtSu_[pair])[member]
              + *(*iDmdtSp_[pair])[member]*phase.Y(member)
            );

            // Implicit transport through this phase
            *eqns[name] += eqn;

            // Explicit transport out of the other phase
            if (eqns.found(otherName))
            {
                *eqns[otherName] -= iDmdt;
            }
        }
    }

    return eqnsPtr;
}


template<class BasePhaseSystem>
void Foam::InterfaceCompositionPhaseChangePhaseSystem<BasePhaseSystem>::
correctInterfaceThermo()
{
    // This loop solves for the interface temperatures, Tf, and updates the
    // interface composition models.
    //
    // The rate of heat transfer to the interface must equal the latent heat
    // consumed at the interface, i.e.:
    //
    // H1*(T1 - Tf) + H2*(T2 - Tf) == mDotL
    //                             == K*rho*(Yfi - Yi)*Li
    //
    // Yfi is likely to be a strong non-linear (typically exponential) function
    // of Tf, so the solution for the temperature is newton-accelerated

    forAllConstIter
    (
        typename BasePhaseSystem::heatTransferModelTable,
        this->heatTransferModels_,
        heatTransferModelIter
    )
    {
        const phasePair& pair =
            this->phasePairs_[heatTransferModelIter.key()];

        const phasePairKey key12(pair.first(), pair.second(), true);
        const phasePairKey key21(pair.second(), pair.first(), true);

        const volScalarField H1(heatTransferModelIter().first()->K());
        const volScalarField H2(heatTransferModelIter().second()->K());
        const dimensionedScalar HSmall("small", heatTransferModel::dimK, SMALL);

        volScalarField& Tf = *this->Tf_[pair];

        for (label i = 0; i < nInterfaceCorrectors_; ++ i)
        {
            volScalarField mDotL
            (
                IOobject
                (
                    "mDotL",
                    this->mesh().time().timeName(),
                    this->mesh()
                ),
                this->mesh(),
                dimensionedScalar(dimEnergy/dimVolume/dimTime)
            );
            volScalarField mDotLPrime
            (
                IOobject
                (
                    "mDotLPrime",
                    this->mesh().time().timeName(),
                    this->mesh()
                ),
                this->mesh(),
                dimensionedScalar(mDotL.dimensions()/dimTemperature)
            );

            // Add latent heats from forward and backward models
            if (this->interfaceCompositionModels_.found(key12))
            {
                this->interfaceCompositionModels_[key12]->addMDotL
                (
                    massTransferModels_[pair].first()->K(),
                    Tf,
                    mDotL,
                    mDotLPrime
                );
            }
            if (this->interfaceCompositionModels_.found(key21))
            {
                this->interfaceCompositionModels_[key21]->addMDotL
                (
                    massTransferModels_[pair].second()->K(),
                    Tf,
                    mDotL,
                    mDotLPrime
                );
            }

            // Update the interface temperature by applying one step of newton's
            // method to the interface relation
            Tf -=
                (
                    H1*(Tf - pair.phase1().thermo().T())
                  + H2*(Tf - pair.phase2().thermo().T())
                  + mDotL
                )
               /(
                    max(H1 + H2 + mDotLPrime, HSmall)
                );

            Tf.correctBoundaryConditions();

            Info<< "Tf." << pair.name()
                << ": min = " << min(Tf.primitiveField())
                << ", mean = " << average(Tf.primitiveField())
                << ", max = " << max(Tf.primitiveField())
                << endl;

            // Update the interface compositions
            if (this->interfaceCompositionModels_.found(key12))
            {
                this->interfaceCompositionModels_[key12]->update(Tf);
            }
            if (this->interfaceCompositionModels_.found(key21))
            {
                this->interfaceCompositionModels_[key21]->update(Tf);
            }
        }
    }
}


template<class BasePhaseSystem>
bool Foam::InterfaceCompositionPhaseChangePhaseSystem<BasePhaseSystem>::read()
{
    if (BasePhaseSystem::read())
    {
        bool readOK = true;

        // Models ...

        return readOK;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
