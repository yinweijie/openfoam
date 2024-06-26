/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2013 OpenFOAM Foundation
    Copyright (C) 2019,2023 OpenCFD Ltd.
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
    Foam::GAMGInterfaceField

Description
     Abstract base class for GAMG agglomerated interface fields.

SourceFiles
    GAMGInterfaceField.C
    GAMGInterfaceFieldNew.C

\*---------------------------------------------------------------------------*/

#ifndef GAMGInterfaceField_H
#define GAMGInterfaceField_H

#include "matrices/lduMatrix/lduAddressing/lduInterfaceFields/lduInterfaceField/lduInterfaceField.H"
#include "matrices/lduMatrix/solvers/GAMG/interfaces/GAMGInterface/GAMGInterface.H"
#include "memory/autoPtr/autoPtr.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                      Class GAMGInterfaceField Declaration
\*---------------------------------------------------------------------------*/

class GAMGInterfaceField
:
    public lduInterfaceField
{
    // Private data

        //- Local reference cast into the interface
        const GAMGInterface& interface_;


    // Private Member Functions

        //- No copy construct
        GAMGInterfaceField(const GAMGInterfaceField&) = delete;

        //- No copy assignment
        void operator=(const GAMGInterfaceField&) = delete;


public:

    //- Runtime type information
    TypeName("GAMGInterfaceField");


    // Declare run-time constructor selection tables

        declareRunTimeSelectionTable
        (
            autoPtr,
            GAMGInterfaceField,
            lduInterfaceField,
            (
                const GAMGInterface& GAMGCp,
                const lduInterfaceField& fineInterface
            ),
            (GAMGCp, fineInterface)
        );

        declareRunTimeSelectionTable
        (
            autoPtr,
            GAMGInterfaceField,
            lduInterface,
            (
                const GAMGInterface& GAMGCp,
                const bool doTransform,
                const int rank
            ),
            (GAMGCp, doTransform, rank)
        );

        declareRunTimeSelectionTable
        (
            autoPtr,
            GAMGInterfaceField,
            Istream,
            (
                const GAMGInterface& GAMGCp,
                Istream& is
            ),
            (GAMGCp, is)
        );


    // Selectors

        //- Return a pointer to a new interface created on freestore given
        //  the fine interface
        static autoPtr<GAMGInterfaceField> New
        (
            const GAMGInterface& GAMGCp,
            const lduInterfaceField& fineInterface
        );

        //- Return a pointer to a new interface created on freestore given
        //  the fine interface
        static autoPtr<GAMGInterfaceField> New
        (
            const GAMGInterface& GAMGCp,
            const bool doTransform,
            const int rank
        );

        //- Return a pointer to a new interface created on freestore given
        //  the fine interface and stream
        static autoPtr<GAMGInterfaceField> New
        (
            const word& patchFieldType,
            const GAMGInterface& GAMGCp,
            Istream& is
        );


    // Constructors

        //- Construct from GAMG interface and fine level interface field
        GAMGInterfaceField
        (
            const GAMGInterface& GAMGCp,
            const lduInterfaceField&
        )
        :
            lduInterfaceField(GAMGCp),
            interface_(GAMGCp)
        {}

        //- Construct from GAMG interface and fine level interface field
        GAMGInterfaceField
        (
            const GAMGInterface& GAMGCp,
            const bool doTransform,
            const int rank
        )
        :
            lduInterfaceField(GAMGCp),
            interface_(GAMGCp)
        {}

        //- Construct from GAMG interface and fine level interface field
        GAMGInterfaceField
        (
            const GAMGInterface& GAMGCp,
            Istream& is
        )
        :
            lduInterfaceField(GAMGCp),
            interface_(GAMGCp)
        {}

        //- Construct by assembling and return a clone.
        virtual autoPtr<GAMGInterfaceField> clone
        (
            const GAMGInterface& GAMGCp,
            const UPtrList<lduInterfaceField>& other // other
        ) const = 0;


    //- Destructor
    virtual ~GAMGInterfaceField() = default;


    // Member Functions

        // Access

            //- Return interface
            const GAMGInterface& interface() const
            {
                return interface_;
            }


        // I/O

            //- Write to stream
            virtual void write(Ostream&) const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
