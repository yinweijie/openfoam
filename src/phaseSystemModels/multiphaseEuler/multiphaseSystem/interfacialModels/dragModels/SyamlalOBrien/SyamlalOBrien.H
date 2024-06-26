/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
    Copyright (C) 2022 OpenCFD Ltd.
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
    Foam::multiphaseEuler::dragModels::SyamlalOBrien

Description
    Syamlal, M., Rogers, W. and O'Brien, T. J. (1993) MFIX documentation,
    Theory Guide. Technical Note DOE/METC-94/1004. Morgantown, West Virginia,
    USA.

SourceFiles
    SyamlalOBrien.C

\*---------------------------------------------------------------------------*/

#ifndef Foam_multiphaseEuler_dragModels_SyamlalOBrien_H
#define Foam_multiphaseEuler_dragModels_SyamlalOBrien_H

#include "interfacialModels/dragModels/dragModel/dragModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace multiphaseEuler
{
namespace dragModels
{

/*---------------------------------------------------------------------------*\
                           Class SyamlalOBrien Declaration
\*---------------------------------------------------------------------------*/

class SyamlalOBrien
:
    public dragModel
{

public:

    //- Runtime type information
    TypeName("SyamlalOBrien");


    // Constructors

        //- Construct from components
        SyamlalOBrien
        (
            const dictionary& interfaceDict,
            const phaseModel& phase1,
            const phaseModel& phase2
        );


    //- Destructor
    virtual ~SyamlalOBrien();


    // Member Functions

        tmp<volScalarField> K(const volScalarField& Ur) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace dragModels
} // End namespace multiphaseEuler
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
