/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2017 OpenFOAM Foundation
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
    Foam::ThermoCombustion

Description
    Thermo model wrapper for combustion models

SourceFiles
    ThermoCombustion.C

\*---------------------------------------------------------------------------*/

#ifndef ThermoCombustion_H
#define ThermoCombustion_H

#include "memory/autoPtr/autoPtr.H"
#include "CombustionModelCaseDir/CombustionModel/CombustionModelPascal.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    class ThermoCombustion Declaration
\*---------------------------------------------------------------------------*/

template<class ReactionThermo>
class ThermoCombustion
:
    public CombustionModel<ReactionThermo>
{
    // Private Member Functions

        //- Construct as copy (not implemented)
        ThermoCombustion(const ThermoCombustion&) = delete;

        //- No copy assignment
        void operator=(const ThermoCombustion&) = delete;


protected:

    // Protected data

        //- Thermo
        ReactionThermo& thermo_;


public:

    // Constructors

        //- Construct from components
        ThermoCombustion
        (
            const word& modelType,
            ReactionThermo& thermo,
            const compressibleTurbulenceModel& turb
        );


    //- Destructor
    virtual ~ThermoCombustion();


    // Member Functions

        //- Return access to the thermo package
        virtual ReactionThermo& thermo();

        //- Return const access to the thermo package
        virtual const ReactionThermo& thermo() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "CombustionModelCaseDir/ThermoCombustion/ThermoCombustion.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
