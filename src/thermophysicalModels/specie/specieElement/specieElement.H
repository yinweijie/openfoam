/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016 OpenFOAM Foundation
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
    Foam::specieElement

Description

SourceFiles
    specieElementI.H

\*---------------------------------------------------------------------------*/

#ifndef specieElement_H
#define specieElement_H

#include "primitives/strings/word/word.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward Declarations
class specieElement;
Ostream& operator<<(Ostream&, const specieElement&);


/*---------------------------------------------------------------------------*\
                         Class specieElement Declaration
\*---------------------------------------------------------------------------*/

class specieElement
{
    // Private Data

        //- Name of the element
        word name_;

        //- Number of atoms of this element in the specie
        label nAtoms_;


public:

    // Constructors

        //- Default construct
        inline specieElement();

        //- Construct from components
        inline specieElement(const word& name, const label nAtoms);

        //- Construct from Istream
        inline explicit specieElement(Istream& is);


    // Member Functions

        //- Return the name of the element
        inline const word& name() const;

        //- Return non-const access to the name of the element
        inline word& name();

        //- Return the number of atoms of this element in the specie
        inline label nAtoms() const;

        //- Return non-const access to the number of atoms of this element
        //  in the specie
        inline label& nAtoms();


    // Member Operators

        //- Equality comparison
        inline bool operator==(const specieElement&) const;

        //- Inequality comparison
        inline bool operator!=(const specieElement&) const;


    // IOstream Operators

        inline friend Ostream& operator<<(Ostream&, const specieElement&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "specieElement/specieElementI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
