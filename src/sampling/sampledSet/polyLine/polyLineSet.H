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
    Foam::polyLineSet

Description
    Sample along poly line defined by a list of points (knots)

    For a dictionary specification:
    \table
        Property | Description                             | Required | Default
        type     | polyLine                                | yes      |
        axis     | x, y, z, xyz, distance                  | yes      |
        points   | The locations                           | yes      |
    \endtable

SourceFiles
    polyLineSet.C

\*---------------------------------------------------------------------------*/

#ifndef polyLineSet_H
#define polyLineSet_H

#include "sampledSet/sampledSet/sampledSet.H"
#include "containers/Lists/DynamicList/DynamicList.H"
#include "passiveParticle/passiveParticleCloud.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class polyLineSet Declaration
\*---------------------------------------------------------------------------*/

class polyLineSet
:
    public sampledSet
{
    // Private data

        //- Sampling points
        pointField sampleCoords_;


    // Private Member Functions

        //- Sample till hits boundary. Called with singleParticle at position
        //  inbetween sampleCoords_[sampleI] and sampleCoords_[sampleI+1].
        //  Returns false if end of samples reached.
        bool trackToBoundary
        (
            passiveParticleCloud& particleCloud,
            passiveParticle& singleParticle,
            label& sampleI,
            DynamicList<point>& samplingPts,
            DynamicList<label>& samplingCells,
            DynamicList<label>& samplingFaces,
            DynamicList<scalar>& samplingCurveDist
        ) const;

        //- Samples all point in sampleCoords_
        //  samplingSegments contains segmentNo for each sample.
        void calcSamples
        (
            DynamicList<point>& samplingPts,
            DynamicList<label>& samplingCells,
            DynamicList<label>& samplingFaces,
            DynamicList<label>& samplingSegments,
            DynamicList<scalar>& samplingCurveDist
        ) const;

        //- Uses calcSamples to obtain samples. Copies them into *this.
        void genSamples();


public:

    //- Runtime type information
    TypeName("polyLine");


    // Static data

        //- Tolerance when comparing points relative to difference between
        //- start and end points
        static const scalar tol;


    // Constructors

        //- Construct from components
        polyLineSet
        (
            const word& name,
            const polyMesh& mesh,
            const meshSearch& searchEngine,
            const word& axis,
            const List<point>& samplePoints
        );

        //- Construct from dictionary
        polyLineSet
        (
            const word& name,
            const polyMesh& mesh,
            const meshSearch& searchEngine,
            const dictionary& dict
        );


    //- Destructor
    virtual ~polyLineSet() = default;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
