/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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
    Foam::absorptionCoeffs

Description
    Absorption coefficients class used in greyMeanAbsorptionEmission and
    wideBandAbsorptionEmission

SourceFiles
    absorptionCoeffs.C

\*---------------------------------------------------------------------------*/

#ifndef absorptionCoeffs_H
#define absorptionCoeffs_H

#include "containers/Lists/List/List.H"
#include "db/IOstreams/IOstreams.H"
#include "db/IOobjects/IOdictionary/IOdictionary.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
namespace Foam
{
namespace radiation
{

/*---------------------------------------------------------------------------*\
                      Class absorptionCoeffs Declaration
\*---------------------------------------------------------------------------*/

class absorptionCoeffs
{
public:

    // Public data members

        static const int nCoeffs_ = 6;
        typedef FixedList<scalar, nCoeffs_> coeffArray;


private:

    // Private data

        // Temperature limits of applicability of functions
        scalar Tlow_, Thigh_, Tcommon_;

        // Polynomial using inverse temperatures
        bool invTemp_;

        coeffArray highACoeffs_;
        coeffArray lowACoeffs_;


    // Private Member Functions

        //- Check given temperature is within the range of the fitted coeffs
        void checkT(const scalar T) const;


public:

    // Constructors

        //- Default construct
        absorptionCoeffs() = default;


    //- Destructor
    ~absorptionCoeffs() = default;


    // Member functions

        //- Return the coefficients corresponding to the given temperature
        const coeffArray& coeffs(const scalar T) const;

        // Initialise from a dictionary
        void initialise(const dictionary&);


    // Access Functions

        inline bool invTemp() const;

        inline scalar Tcommon() const;

        inline scalar Tlow() const;

        inline scalar Thigh() const;

        inline const coeffArray& highACoeffs() const;

        inline const coeffArray& lowACoeffs() const;
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam
} // End namespace radiation

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "radiationModels/fvDOM/absorptionCoeffs/absorptionCoeffsI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
