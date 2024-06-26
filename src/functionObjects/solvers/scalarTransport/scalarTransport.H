/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2017 OpenFOAM Foundation
    Copyright (C) 2016-2020 OpenCFD Ltd.
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
    Foam::functionObjects::scalarTransport

Group
    grpSolversFunctionObjects

Description
    Evolves a passive scalar transport equation.

    - To specify the field name set the \c field entry
    - To employ the same numerical schemes as another field set
      the \c schemesField entry,
    - The diffusivity can be set manually using the 'D' entry, retrieved
      from the turbulence model or specified nut
    - Alternatively if a turbulence model is available a turbulent diffusivity
      may be constructed from the laminar and turbulent viscosities using the
      optional diffusivity coefficients \c alphaD and \c alphaDt (which default
      to 1):
      \verbatim
          D = alphaD*nu + alphaDt*nut
      \endverbatim
    - To specify a transport quantity within a phase enter phase.
    - bounded01 bounds the transported scalar within 0 and 1.

Usage
    Example of function object specification to solve a scalar transport
    equation:
    \verbatim
    functions
    {
        scalar1
        {
            type            scalarTransport;
            libs            (solverFunctionObjects);

            resetOnStartUp  no;
            region          cabin;
            field           H2O;


            fvOptions
            {
                ...
            }
        }
    }
    \endverbatim

    Example of function object specification to solve a residence time
    in a two phase flow:
    equation:
    \verbatim
    functions
    {
        sTransport
        {
            type            scalarTransport;
            libs            (solverFunctionObjects);

            enabled         true;
            writeControl    writeTime;
            writeInterval   1;

            field           s;
            bounded01       false;
            phase           alpha.water;

            write           true;

            fvOptions
            {
                unitySource
                {
                    type            scalarSemiImplicitSource;
                    enabled         true;

                    selectionMode   all;
                    volumeMode      specific;

                    sources
                    {
                        s           (1 0);
                    }
                }
            }

            resetOnStartUp  false;
        }
    }
    \endverbatim

    Where the entries comprise:
    \table
        Property     | Description             | Required    | Default value
        type         | Type name: scalarTransport | yes      |
        field        | Name of the scalar field | no         | s
        phi          | Name of flux field      | no          | phi
        rho          | Name of density field   | no          | rho
        phase        | Name of the phase       | no          | none
        nut          | Name of the turbulence viscosity | no | none
        D            | Diffusion coefficient   | no          | auto generated
        nCorr        | Number of correctors    | no          | 0
        resetOnStartUp | Reset scalar to zero on start-up | no | no
        schemesField | Name of field to specify schemes | no | field name
        fvOptions    | List of scalar sources  | no          |
        bounded01    | Bounds scalar between 0-1 for multiphase | no | true
        phasePhiCompressed | Compressed flux for VOF | no | alphaPhiUn
    \endtable

See also
    Foam::functionObjects::fvMeshFunctionObject

SourceFiles
    scalarTransport.C

\*---------------------------------------------------------------------------*/

#ifndef functionObjects_scalarTransport_H
#define functionObjects_scalarTransport_H

#include "functionObjects/fvMeshFunctionObject/fvMeshFunctionObject.H"
#include "fields/volFields/volFields.H"
#include "cfdTools/general/fvOptions/fvOptionList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace functionObjects
{

/*---------------------------------------------------------------------------*\
                       Class scalarTransport Declaration
\*---------------------------------------------------------------------------*/

class scalarTransport
:
    public fvMeshFunctionObject
{
    // Private data

        //- Name of the transport field.
        word fieldName_;

        //- Name of flux field (optional)
        word phiName_;

        //- Name of density field (optional)
        word rhoName_;

        //- Name of turbulent viscosity field (optional)
        word nutName_;

        //- Name of phase field (optional)
        word phaseName_;

        //- Name of phase field compressed flux (optional)
        word phasePhiCompressedName_;

        //- Diffusion coefficient (optional)
        scalar D_;

        //- Flag to indicate whether a constant, uniform D_ is specified
        bool constantD_;

        //- Laminar diffusion coefficient (optional)
        scalar alphaD_;

        //- Turbulent diffusion coefficient (optional)
        scalar alphaDt_;

        //- Number of corrector iterations (optional)
        label nCorr_;

        //- Flag to reset the scalar to zero on start-up
        bool resetOnStartUp_;

        //- Name of field whose schemes are used (optional)
        word schemesField_;

        //- Run-time selectable finite volume options, e.g. sources, constraints
        fv::optionList fvOptions_;

        //- Bound scalar between 0-1 using MULES for multiphase case
        bool bounded01_;


    // Private Member Functions

        //- Return reference to registered transported field
        volScalarField& transportedField();

        //- Return the diffusivity field
        tmp<volScalarField> D
        (
            const volScalarField& s,
            const surfaceScalarField& phi
        ) const;

        //- No copy construct
        scalarTransport(const scalarTransport&) = delete;

        //- No copy assignment
        void operator=(const scalarTransport&) = delete;


public:

    //- Runtime type information
    TypeName("scalarTransport");


    // Constructors

        //- Construct from Time and dictionary
        scalarTransport
        (
            const word& name,
            const Time& runTime,
            const dictionary& dict
        );


    //- Destructor
    virtual ~scalarTransport();


    // Member Functions

        //- Read the scalarTransport data
        virtual bool read(const dictionary&);

        //- Calculate the scalarTransport
        virtual bool execute();

        //- Do nothing.
        //  The volScalarField is registered and written automatically
        virtual bool write();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace functionObjects
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
