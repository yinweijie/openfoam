/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2018 OpenFOAM Foundation
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

Class
    Foam::heatTransferModel

Description

SourceFiles
    heatTransferModel.C

\*---------------------------------------------------------------------------*/

#ifndef heatTransferModel_H
#define heatTransferModel_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "fields/volFields/volFields.H"
#include "db/dictionary/dictionary.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"

namespace Foam
{

// Forward Declarations
class phasePair;

/*---------------------------------------------------------------------------*\
                        Class heatTransferModel Declaration
\*---------------------------------------------------------------------------*/

class heatTransferModel
{
protected:

    // Protected Data

        //- Phase pair
        const phasePair& pair_;

        //- Residual phase fraction
        const dimensionedScalar residualAlpha_;


public:

    //- Runtime type information
    TypeName("heatTransferModel");


    // Declare runtime construction

        declareRunTimeSelectionTable
        (
            autoPtr,
            heatTransferModel,
            dictionary,
            (
                const dictionary& dict,
                const phasePair& pair
            ),
            (dict, pair)
        );


    // Static Data Members

        //- Coefficient dimensions
        static const dimensionSet dimK;


    // Constructors

        //- Construct froma dictionary and a phase pair
        heatTransferModel
        (
            const dictionary& dict,
            const phasePair& pair
        );


    //- Destructor
    virtual ~heatTransferModel() = default;


    // Selectors

        static autoPtr<heatTransferModel> New
        (
            const dictionary& dict,
            const phasePair& pair
        );


    // Member Functions

        //- The heat transfer function K used in the enthalpy equation
        //    ddt(alpha1*rho1*ha) + ... = ... K*(Ta - Tb)
        //    ddt(alpha2*rho2*hb) + ... = ... K*(Tb - Ta)
        tmp<volScalarField> K() const;

        //- The heat transfer function K used in the enthalpy equation
        //    ddt(alpha1*rho1*ha) + ... = ... K*(Ta - Tb)
        //    ddt(alpha2*rho2*hb) + ... = ... K*(Tb - Ta)
        //  with a specified residual volume fraction
        virtual tmp<volScalarField> K(const scalar residualAlpha) const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
