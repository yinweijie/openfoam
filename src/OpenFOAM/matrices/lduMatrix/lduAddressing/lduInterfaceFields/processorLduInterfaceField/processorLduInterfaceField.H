/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2014 OpenFOAM Foundation
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
    Foam::processorLduInterfaceField

Description
    Abstract base class for processor coupled interfaces.

SourceFiles
    processorLduInterfaceField.C

\*---------------------------------------------------------------------------*/

#ifndef processorLduInterfaceField_H
#define processorLduInterfaceField_H

#include "fields/Fields/primitiveFieldsFwd.H"
#include "db/typeInfo/typeInfo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                  Class processorLduInterfaceField Declaration
\*---------------------------------------------------------------------------*/

class processorLduInterfaceField
{
public:

    //- Runtime type information
    TypeName("processorLduInterfaceField");


    // Constructors

        //- Default construct
        processorLduInterfaceField() = default;


    //- Destructor
    virtual ~processorLduInterfaceField() = default;


    // Member Functions

        // Access

            //- Return communicator used for comms
            virtual label comm() const = 0;

            //- Return processor number
            virtual int myProcNo() const = 0;

            //- Return neighbour processor number
            virtual int neighbProcNo() const = 0;

            //- Is the transform required
            virtual bool doTransform() const = 0;

            //- Return face transformation tensor
            virtual const tensorField& forwardT() const = 0;

            //- Return rank of component for transform
            virtual int rank() const = 0;


        //- Transform given patch field
        template<class Type>
        void transformCoupleField(Field<Type>& f) const;

        //- Transform given patch component field
        void transformCoupleField
        (
            solveScalarField& f,
            const direction cmpt
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "fields/Fields/tensorField/tensorField.H"

template<class Type>
void Foam::processorLduInterfaceField::transformCoupleField
(
    Field<Type>& f
) const
{
    if (doTransform())
    {
        if (forwardT().size() == 1)
        {
            transform(f, forwardT()[0], f);
        }
        else
        {
            transform(f, forwardT(), f);
        }
    }
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
