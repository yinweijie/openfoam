/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "sampledSet/triSurfaceMeshPointSet/triSurfaceMeshPointSet.H"
#include "meshSearch/meshSearch.H"
#include "containers/Lists/DynamicList/DynamicList.H"
#include "meshes/polyMesh/polyMesh.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "searchableSurfaces/triSurfaceMesh/triSurfaceMesh.H"
#include "db/Time/TimeOpenFOAM.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(triSurfaceMeshPointSet, 0);
    addToRunTimeSelectionTable(sampledSet, triSurfaceMeshPointSet, word);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::triSurfaceMeshPointSet::calcSamples
(
    DynamicList<point>& samplingPts,
    DynamicList<label>& samplingCells,
    DynamicList<label>& samplingFaces,
    DynamicList<label>& samplingSegments,
    DynamicList<scalar>& samplingCurveDist
) const
{
    forAll(sampleCoords_, sampleI)
    {
        label celli = searchEngine().findCell(sampleCoords_[sampleI]);

        if (celli != -1)
        {
            samplingPts.append(sampleCoords_[sampleI]);
            samplingCells.append(celli);
            samplingFaces.append(-1);
            samplingSegments.append(0);
            samplingCurveDist.append(1.0 * sampleI);
        }
    }
}


void Foam::triSurfaceMeshPointSet::genSamples()
{
    // Storage for sample points
    DynamicList<point> samplingPts;
    DynamicList<label> samplingCells;
    DynamicList<label> samplingFaces;
    DynamicList<label> samplingSegments;
    DynamicList<scalar> samplingCurveDist;

    calcSamples
    (
        samplingPts,
        samplingCells,
        samplingFaces,
        samplingSegments,
        samplingCurveDist
    );

    samplingPts.shrink();
    samplingCells.shrink();
    samplingFaces.shrink();
    samplingSegments.shrink();
    samplingCurveDist.shrink();

    // Move into *this
    setSamples
    (
        std::move(samplingPts),
        std::move(samplingCells),
        std::move(samplingFaces),
        std::move(samplingSegments),
        std::move(samplingCurveDist)
    );

    if (debug)
    {
        write(Info);
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::triSurfaceMeshPointSet::triSurfaceMeshPointSet
(
    const word& name,
    const polyMesh& mesh,
    const meshSearch& searchEngine,
    const dictionary& dict
)
:
    sampledSet(name, mesh, searchEngine, dict),
    surfaceName_(dict.get<word>("surface"))
{
    // Get or load surface

    const auto* surfPtr =
        mesh.time().cfindObject<triSurfaceMesh>(surfaceName_);

    if (surfPtr)
    {
        // Note: should use localPoints() instead of points() but assume
        // trisurface is compact.
        sampleCoords_ = surfPtr->points();
    }
    else
    {
        sampleCoords_ = triSurface
        (
            IOobject
            (
                surfaceName_,
                mesh.time().constant(),     // instance
                "triSurface",               // local
                mesh.time(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            ),
            dictionary::null
        ).points();
    }

    genSamples();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::point Foam::triSurfaceMeshPointSet::getRefPoint
(
    const List<point>& pts
) const
{
    if (pts.size())
    {
        // Use first samplePt as starting point
        return pts.first();
    }

    return Zero;
}


// ************************************************************************* //
