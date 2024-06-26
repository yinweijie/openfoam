/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2018 OpenFOAM Foundation
    Copyright (C) 2021 OpenCFD Ltd.
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
    Foam::MRFZone

Description
    MRF zone definition based on cell zone and parameters
    obtained from a control dictionary constructed from the given stream.

    The rotation of the MRF region is defined by an origin and axis of
    rotation and an angular speed.

SourceFiles
    MRFZone.C

\*---------------------------------------------------------------------------*/

#ifndef MRFZone_H
#define MRFZone_H

#include "db/dictionary/dictionary.H"
#include "primitives/strings/lists/wordList.H"
#include "primitives/ints/lists/labelList.H"
#include "dimensionedTypes/dimensionedScalar/dimensionedScalar.H"
#include "dimensionedTypes/dimensionedVector/dimensionedVector.H"
#include "fields/volFields/volFieldsFwd.H"
#include "fields/surfaceFields/surfaceFields.H"
#include "fvMatrices/fvMatricesFwd.H"
#include "meshes/polyMesh/mapPolyMesh/mapPolyMesh.H"
#include "primitives/functions/Function1/Function1/Function1Pascal.H"
#include "memory/autoPtr/autoPtr.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes
class fvMesh;

/*---------------------------------------------------------------------------*\
                           Class MRFZone Declaration
\*---------------------------------------------------------------------------*/

class MRFZone
{
    // Private data

        //- Reference to the mesh database
        const fvMesh& mesh_;

        //- Name of the MRF region
        const word name_;

        //- Coefficients dictionary
        dictionary coeffs_;

        //- MRF region active flag
        bool active_;

        //- Name of cell zone
        word cellZoneName_;

        //- Cell zone ID
        label cellZoneID_;

        //- Non-rotating patches
        wordRes excludedPatchNames_;

        //- Indices of non-rotating patches
        labelList excludedPatchLabels_;

        //- Internal faces that are part of MRF
        labelList internalFaces_;

        //- Outside faces (per patch) that move with the MRF
        labelListList includedFaces_;

        //- Excluded faces (per patch) that do not move with the MRF
        labelListList excludedFaces_;

        //- Origin of the axis
        vector origin_;

        //- Axis vector
        vector axis_;

        //- Angular velocity (rad/sec)
        autoPtr<Function1<scalar>> omega_;


    // Private Member Functions

        //- Divide faces in frame according to patch
        void setMRFFaces();

        //- Make the given absolute mass/vol flux relative within the MRF region
        template<class RhoFieldType>
        void makeRelativeRhoFlux
        (
            const RhoFieldType& rho,
            surfaceScalarField& phi
        ) const;

        //- Make the given absolute mass/vol flux relative within the MRF region
        template<class RhoFieldType>
        void makeRelativeRhoFlux
        (
            const RhoFieldType& rho,
            FieldField<fvsPatchField, scalar>& phi
        ) const;

        //- Make the given absolute mass/vol flux relative within the MRF region
        template<class RhoFieldType>
        void makeRelativeRhoFlux
        (
            const RhoFieldType& rho,
            Field<scalar>& phi,
            const label patchi
        ) const;

        //- Make the given relative mass/vol flux absolute within the MRF region
        template<class RhoFieldType>
        void makeAbsoluteRhoFlux
        (
            const RhoFieldType& rho,
            surfaceScalarField& phi
        ) const;

        //- No copy construct
        MRFZone(const MRFZone&) = delete;

        //- No copy assignment
        void operator=(const MRFZone&) = delete;


public:

    // Declare name of the class and its debug switch
    ClassName("MRFZone");


    // Constructors

        //- Construct from fvMesh
        MRFZone
        (
            const word& name,
            const fvMesh& mesh,
            const dictionary& dict,
            const word& cellZoneName = word::null
        );

        //- Return clone
        autoPtr<MRFZone> clone() const
        {
            NotImplemented;
            return nullptr;
        }


    // Member Functions

        //- Return const access to the MRF region name
        inline const word& name() const;

        //- Return const access to the MRF active flag
        inline bool active() const;

        //- Return const access to the MRF origin
        inline const vector& origin() const;

        //- Return const access to the MRF axis
        inline const vector& axis() const;

        //- Return the current Omega vector
        vector Omega() const;

        //- Update the mesh corresponding to given map
        void updateMesh(const mapPolyMesh& mpm)
        {
            // Only updates face addressing
            setMRFFaces();
        }

        //- Add the Coriolis force contribution to the acceleration field
        void addCoriolis
        (
            const volVectorField& U,
            volVectorField& ddtU
        ) const;

        //- Add the Coriolis force contribution to the momentum equation
        //  Adds to the lhs of the equation; optionally add to rhs
        void addCoriolis
        (
            fvVectorMatrix& UEqn,
            const bool rhs = false
        ) const;

        //- Add the Coriolis force contribution to the momentum equation
        //  Adds to the lhs of the equation; optionally add to rhs
        void addCoriolis
        (
            const volScalarField& rho,
            fvVectorMatrix& UEqn,
            const bool rhs = false
        ) const;

        //- Make the given absolute velocity relative within the MRF region
        void makeRelative(volVectorField& U) const;

        //- Make the given absolute flux relative within the MRF region
        void makeRelative(surfaceScalarField& phi) const;

        //- Make the given absolute boundary flux relative
        //  within the MRF region
        void makeRelative(FieldField<fvsPatchField, scalar>& phi) const;

        //- Make the given absolute patch flux relative
        //  within the MRF region
        void makeRelative(Field<scalar>& phi, const label patchi) const;

        //- Make the given absolute mass-flux relative within the MRF region
        void makeRelative
        (
            const surfaceScalarField& rho,
            surfaceScalarField& phi
        ) const;

        //- Make the given relative velocity absolute within the MRF region
        void makeAbsolute(volVectorField& U) const;

        //- Make the given relative flux absolute within the MRF region
        void makeAbsolute(surfaceScalarField& phi) const;

        //- Make the given relative mass-flux absolute within the MRF region
        void makeAbsolute
        (
            const surfaceScalarField& rho,
            surfaceScalarField& phi
        ) const;

        //- Correct the boundary velocity for the rotation of the MRF region
        void correctBoundaryVelocity(volVectorField& U) const;

        //- Zero the MRF region of the given field
        template<class Type>
        void zero(GeometricField<Type, fvsPatchField, surfaceMesh>& phi) const;

        //- Update MRFZone faces if the mesh topology changes
        void update();


    // I-O

        //- Write
        void writeData(Ostream& os) const;

        //- Read MRF dictionary
        bool read(const dictionary& dict);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "cfdTools/general/MRF/MRFZoneTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "cfdTools/general/MRF/MRFZoneI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
