/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2012 OpenFOAM Foundation
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
    Foam::XiModels::algebraic

Description
    Simple algebraic model for Xi based on Gulders correlation
    with a linear correction function to give a plausible profile for Xi.
    See report TR/HGW/10 for details on the Weller two equations model.
    See \link XiModel.H \endlink for more details on flame wrinkling modelling.

SourceFiles
    algebraic.C

\*---------------------------------------------------------------------------*/

#ifndef algebraic_H
#define algebraic_H

#include "XiModels/XiModel/XiModel.H"
#include "XiModels/XiEqModels/XiEqModel/XiEqModel.H"
#include "XiModels/XiGModels/XiGModel/XiGModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace XiModels
{

/*---------------------------------------------------------------------------*\
                          Class algebraic Declaration
\*---------------------------------------------------------------------------*/

class algebraic
:
    public XiModel
{
    // Private data

        scalar XiShapeCoef;

        autoPtr<XiEqModel> XiEqModel_;
        autoPtr<XiGModel> XiGModel_;


    // Private Member Functions

        //- No copy construct
        algebraic(const algebraic&) = delete;

        //- No copy assignment
        void operator=(const algebraic&) = delete;


public:

    //- Runtime type information
    TypeName("algebraic");


    // Constructors

        //- Construct from components
        algebraic
        (
            const dictionary& XiProperties,
            const psiuReactionThermo& thermo,
            const compressible::RASModel& turbulence,
            const volScalarField& Su,
            const volScalarField& rho,
            const volScalarField& b,
            const surfaceScalarField& phi
        );


    //- Destructor
    virtual ~algebraic();


    // Member Functions

        //- Return the flame diffusivity
        virtual tmp<volScalarField> Db() const;

        //- Correct the flame-wrinkling Xi
        virtual void correct();

        //- Update properties from given dictionary
        virtual bool read(const dictionary& XiProperties);

        //- Write fields of the XiEq model
        virtual void writeFields()
        {
            XiEqModel_().writeFields();
        }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace XiModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
