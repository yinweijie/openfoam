/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
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
    Foam::phaseChangeTwoPhaseMixtures::SchnerrSauer

Description
    SchnerrSauer cavitation model.

    Reference:
    \verbatim
        Schnerr, G. H., And Sauer, J.,
        "Physical and Numerical Modeling of Unsteady Cavitation Dynamics",
        Proc. 4th International Conference on Multiphase Flow,
        New Orleans, U.S.A., 2001.
    \endverbatim

SourceFiles
    SchnerrSauer.C

\*--------------------------------------------------------------------*/

#ifndef SchnerrSauer_H
#define SchnerrSauer_H

#include "phaseChangeTwoPhaseMixture/phaseChangeTwoPhaseMixture.H"

// * * * * * * * * * * * * * * * * *  * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace phaseChangeTwoPhaseMixtures
{

/*--------------------------------------------------------------------*\
                              Class SchnerrSauer
\*--------------------------------------------------------------------*/

class SchnerrSauer
:
    public phaseChangeTwoPhaseMixture
{
    // Private data

        //- Bubble number density
        dimensionedScalar n_;

        //- Nucleation site diameter
        dimensionedScalar dNuc_;

        //- Condensation rate coefficient
        dimensionedScalar Cc_;

        //- Vapourisation rate coefficient
        dimensionedScalar Cv_;

        dimensionedScalar p0_;

        //- Nucleation site volume-fraction
        dimensionedScalar alphaNuc() const;

        //- Reciprocal bubble radius
        tmp<volScalarField>rRb(const volScalarField& limitedAlpha1) const;

        //- Part of the condensation and vapourisation rates
        tmp<volScalarField> pCoeff(const volScalarField& p) const;


public:

    //- Runtime type information
    TypeName("SchnerrSauer");


    // Constructors

        //- Construct from components
        SchnerrSauer
        (
            const volVectorField& U,
            const surfaceScalarField& phi
        );


    //- Destructor
    virtual ~SchnerrSauer() = default;


    // Member Functions

        //- Return the mass condensation and vaporisation rates as a
        //  coefficient to multiply (1 - alphal) for the condensation rate
        //  and a coefficient to multiply  alphal for the vaporisation rate
        virtual Pair<tmp<volScalarField>> mDotAlphal() const;

        //- Return the mass condensation and vaporisation rates as coefficients
        //  to multiply (p - pSat)
        virtual Pair<tmp<volScalarField>> mDotP() const;

        //- Correct the SchnerrSauer phaseChange model
        virtual void correct();

        //- Read the transportProperties dictionary and update
        virtual bool read();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace phaseChangeTwoPhaseMixtures
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
