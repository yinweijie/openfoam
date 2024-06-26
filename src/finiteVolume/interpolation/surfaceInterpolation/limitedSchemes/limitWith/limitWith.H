/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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
    Foam::limitWith

Group
    grpFvLimitedSurfaceInterpolationSchemes

Description
    limitWith differencing scheme limits the specified scheme with the
    specified limiter.

SourceFiles
    limitWith.C

\*---------------------------------------------------------------------------*/

#ifndef limitWith_H
#define limitWith_H

#include "interpolation/surfaceInterpolation/surfaceInterpolationScheme/surfaceInterpolationScheme.H"
#include "interpolation/surfaceInterpolation/limitedSchemes/limitedSurfaceInterpolationScheme/limitedSurfaceInterpolationScheme.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class limitWith Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class limitWith
:
    public surfaceInterpolationScheme<Type>
{
    // Private Member Functions

        //- Interpolation scheme
        tmp<surfaceInterpolationScheme<Type>> tInterp_;

        //- Limiter
        tmp<limitedSurfaceInterpolationScheme<Type>> tLimiter_;


        //- No copy construct
        limitWith(const limitWith&) = delete;

        //- No copy assignment
        void operator=(const limitWith&) = delete;


public:

    //- Runtime type information
    TypeName("limitWith");


    // Constructors

        //- Construct from mesh and Istream.
        //  The name of the flux field is read from the Istream and looked-up
        //  from the mesh objectRegistry
        limitWith
        (
            const fvMesh& mesh,
            Istream& is
        )
        :
            surfaceInterpolationScheme<Type>(mesh),
            tInterp_
            (
                surfaceInterpolationScheme<Type>::New(mesh, is)
            ),
            tLimiter_
            (
                limitedSurfaceInterpolationScheme<Type>::New(mesh, is)
            )
        {}

        //- Construct from mesh, faceFlux and Istream
        limitWith
        (
            const fvMesh& mesh,
            const surfaceScalarField& faceFlux,
            Istream& is
        )
        :
            surfaceInterpolationScheme<Type>(mesh),
            tInterp_
            (
                surfaceInterpolationScheme<Type>::New(mesh, faceFlux, is)
            ),
            tLimiter_
            (
                limitedSurfaceInterpolationScheme<Type>::New(mesh, faceFlux, is)
            )
        {}


    // Member Functions

        //- Return the interpolation weighting factors
        virtual tmp<surfaceScalarField> weights
        (
            const GeometricField<Type, fvPatchField, volMesh>& vf
        ) const
        {
            return tLimiter_().weights
            (
                vf,
                tInterp_().weights(vf),
                tLimiter_().limiter(vf)
            );
        }

        //- Return true if this scheme uses an explicit correction
        virtual bool corrected() const
        {
            return tInterp_().corrected();
        }

        //- Return the explicit correction to the face-interpolate
        //  for the given field
        virtual tmp<GeometricField<Type, fvsPatchField, surfaceMesh>>
        correction(const GeometricField<Type, fvPatchField, volMesh>& vf) const
        {
            return tLimiter_().limiter(vf)*tInterp_().correction(vf);
        }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
