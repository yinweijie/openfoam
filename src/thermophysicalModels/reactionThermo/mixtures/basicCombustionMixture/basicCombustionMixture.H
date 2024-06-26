/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2014-2015 OpenFOAM Foundation
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
    Foam::basicCombustionMixture

Group
    grpReactionThermophysicalMixtures

Description
    Specialization of the basicSpecieMixture for combustion.

SourceFiles
    basicCombustionMixture.C

\*---------------------------------------------------------------------------*/

#ifndef basicCombustionMixture_H
#define basicCombustionMixture_H

#include "mixtures/basicSpecieMixture/basicSpecieMixture.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                 Class basicCombustionMixture Declaration
\*---------------------------------------------------------------------------*/

class basicCombustionMixture
:
    public basicSpecieMixture
{

public:

    //- Run time type information
    TypeName("basicCombustionMixture");


    //- The base class of the mixture
    typedef basicCombustionMixture basicMixtureType;


    // Constructors

        //- Construct from dictionary, specie names, mesh and phase name
        basicCombustionMixture
        (
            const dictionary& thermoDict,
            const wordList& specieNames,
            const fvMesh& mesh,
            const word& phaseName
        );


    //- Destructor
    virtual ~basicCombustionMixture() = default;


    // Member functions

        inline scalar fres(const scalar ft, const scalar stoicRatio) const;

        inline tmp<volScalarField> fres
        (
            const volScalarField& ft,
            const dimensionedScalar& stoicRatio
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "mixtures/basicCombustionMixture/basicCombustionMixtureI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
