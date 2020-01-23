/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
    Copyright (C) 2017-2020 OpenCFD Ltd.
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
    Foam::geometricSurfacePatch

Description
    The geometricSurfacePatch is like patchIdentifier but for surfaces.
    Holds type, name and index.

SourceFiles
    geometricSurfacePatch.C

\*---------------------------------------------------------------------------*/

#ifndef geometricSurfacePatch_H
#define geometricSurfacePatch_H

#include "surfZoneIdentifier.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                    Class geometricSurfacePatch Declaration
\*---------------------------------------------------------------------------*/

class geometricSurfacePatch
{
    // Private Data

        //- Name of patch
        word name_;

        //- Index of patch in boundary
        label index_;

        //- Type name of patch
        word geometricType_;

public:

    // Public Data

        //- The name for an 'empty' type
        static constexpr const char* const emptyType = "empty";


    // Static Member Functions

        //- Default patch name: "patch" or "patchN"
        static word defaultName(const label n = -1)
        {
            return
            (
                n < 0
              ? word("patch", false)
              : word("patch" + std::to_string(n), false)
            );
        }


    // Public Classes

        //- Helper to convert identifier types as an operation
        struct fromIdentifier
        {
            geometricSurfacePatch
            operator()(const surfZoneIdentifier& ident) const
            {
                return geometricSurfacePatch(ident);
            }
        };


    // Generated Methods

        //- Copy construct
        geometricSurfacePatch(const geometricSurfacePatch&) = default;

        //- Copy assignment
        geometricSurfacePatch&
            operator=(const geometricSurfacePatch&) = default;


    // Constructors

        //- Default construct, use index=0, name="patch"
        geometricSurfacePatch();

        //- Construct null with specified index
        explicit geometricSurfacePatch(const label index);

        //- Construct from mandatory components
        geometricSurfacePatch(const word& name, const label index);

        //- Construct from components
        geometricSurfacePatch
        (
            const word& name,
            const label index,
            const word& geometricType
        );

        //- Construct from dictionary
        geometricSurfacePatch
        (
            const word& name,
            const dictionary& dict,
            const label index
        );

        //- Implicit conversion from surfZoneIdentifier
        geometricSurfacePatch(const surfZoneIdentifier& ident);


    // Member Functions

        //- The patch/zone name
        const word& name() const
        {
            return name_;
        }

        //- Modifiable patch/zone name
        word& name()
        {
            return name_;
        }

        //- The geometric type of the patch/zone
        const word& geometricType() const
        {
            return geometricType_;
        }

        //- Modifiable geometric type of the patch/zone
        word& geometricType()
        {
            return geometricType_;
        }

        //- The index of this patch/zone in the surface mesh
        label index() const
        {
            return index_;
        }

        //- Modifiable index of this patch/zone in the surface mesh
        label& index()
        {
            return index_;
        }

        //- Write (geometricType) dictionary entry
        //- (without surrounding braces)
        //  \warning Prior to 2020-01 was identical to operator<< output
        void write(Ostream& os) const;


    // Housekeeping

        //- Removed(2020-01) Construct from Istream
        //  \deprecated(2020-01) - unused, inconsistent
        geometricSurfacePatch(Istream& is, const label index) = delete;

        //- Deprecated(2020-01) Construct from components
        //  \deprecated(2020-01) - order inconsistent with other identifiers
        geometricSurfacePatch
        (
            const word& geometricType,
            const word& name,
            const label index
        ) FOAM_DEPRECATED(2020-01);

        //- Deprecated(2020-01) Write dictionary
        //  \deprecated(2020-01) - Write dictionary
        void FOAM_DEPRECATED_FOR(2020-01, "write() or operator<<")
        writeDict(Ostream& os) const
        {
            write(os);
        }
};


// Global Operators

//- Compare patches for equality
bool operator==(const geometricSurfacePatch& a, const geometricSurfacePatch& b);

//- Compare patches for inequality
bool operator!=(const geometricSurfacePatch& a, const geometricSurfacePatch& b);


//- Read name, geometricType
Istream& operator>>(Istream& is, geometricSurfacePatch& obj);

//- Write name, geometricType. Entries are quoted to support empty words.
Ostream& operator<<(Ostream& os, const geometricSurfacePatch& obj);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //