/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2016 OpenFOAM Foundation
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
    Foam::pisoControl

Description
    Specialization of the pimpleControl class for PISO control.

\*---------------------------------------------------------------------------*/

#ifndef pisoControl_H
#define pisoControl_H

#include "cfdTools/general/solutionControl/pimpleControl/pimpleControl.H"

//- Declare that pisoControl will be used
#define PISO_CONTROL
#undef PIMPLE_CONTROL

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class pisoControl Declaration
\*---------------------------------------------------------------------------*/

class pisoControl
:
    public pimpleControl
{
    // Private member functions

        //- No copy construct
        pisoControl(const pisoControl&) = delete;

        //- No copy assignment
        void operator=(const pisoControl&) = delete;


public:

    // Static Data Members

        //- Run-time type information
        TypeName("pisoControl");


    // Constructors

        //- Construct from mesh and the name of control sub-dictionary
        pisoControl(fvMesh& mesh, const word& dictName="PISO");


    //- Destructor
    virtual ~pisoControl();


    // Member Functions

        // Solution control

            //- Read controls from fvSolution dictionary
            virtual bool read();

            //- Return true if in the final inner (PISO) iteration
            inline bool finalInnerIter() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "cfdTools/general/solutionControl/pisoControl/pisoControlI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
