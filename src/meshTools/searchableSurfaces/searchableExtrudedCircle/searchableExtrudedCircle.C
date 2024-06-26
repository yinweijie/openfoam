/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 OpenFOAM Foundation
    Copyright (C) 2019-2022 OpenCFD Ltd.
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

#include "searchableSurfaces/searchableExtrudedCircle/searchableExtrudedCircle.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "db/Time/TimeOpenFOAM.H"
#include "edgeMesh/edgeMesh.H"
#include "algorithms/indexedOctree/indexedOctree.H"
#include "algorithms/indexedOctree/treeDataEdge.H"
#include "interpolations/interpolationWeights/linearInterpolationWeights/linearInterpolationWeights.H"
#include "primitives/quaternion/quaternion.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(searchableExtrudedCircle, 0);
    addToRunTimeSelectionTable
    (
        searchableSurface,
        searchableExtrudedCircle,
        dict
    );
    addNamedToRunTimeSelectionTable
    (
        searchableSurface,
        searchableExtrudedCircle,
        dict,
        extrudedCircle
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::searchableExtrudedCircle::searchableExtrudedCircle
(
    const IOobject& io,
    const dictionary& dict
)
:
    searchableSurface(io),
    eMeshPtr_
    (
        edgeMesh::New
        (
            IOobject
            (
                dict.get<word>("file"),             // name
                io.time().constant(),               // instance
                "geometry",                         // local
                io.time(),                          // registry
                IOobject::MUST_READ,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            ).objectPath()
        )
    ),
    radius_(dict.get<scalar>("radius"))
{
    const edgeMesh& eMesh = eMeshPtr_();

    const pointField& points = eMesh.points();
    const edgeList& edges = eMesh.edges();
    bounds() = boundBox(points, false);

    // Make the boundBox into a perfect cube around its centre
    const scalar halfWidth = mag(0.5*bounds().span());

    bounds().reset(bounds().centre());
    bounds().grow(halfWidth);

    // Slightly extended bb. Slightly off-centred just so on symmetric
    // geometry there are less face/edge aligned items.
    treeBoundBox bb(bounds());
    bb.grow(ROOTVSMALL);

    edgeTree_.reset
    (
        new indexedOctree<treeDataEdge>
        (
            treeDataEdge(edges, points),   // All edges

            bb,     // overall search domain
            8,      // maxLevel
            10,     // leafsize
            3.0     // duplicity
        )
    );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::searchableExtrudedCircle::~searchableExtrudedCircle()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::wordList& Foam::searchableExtrudedCircle::regions() const
{
    if (regions_.empty())
    {
        regions_.setSize(1);
        regions_[0] = "region0";
    }
    return regions_;
}


Foam::label Foam::searchableExtrudedCircle::size() const
{
    return eMeshPtr_().points().size();
}


Foam::tmp<Foam::pointField> Foam::searchableExtrudedCircle::coordinates() const
{
    return eMeshPtr_().points();
}


void Foam::searchableExtrudedCircle::boundingSpheres
(
    pointField& centres,
    scalarField& radiusSqr
) const
{
    centres = eMeshPtr_().points();
    radiusSqr.setSize(centres.size());
    radiusSqr = Foam::sqr(radius_);
    // Add a bit to make sure all points are tested inside
    radiusSqr += Foam::sqr(SMALL);
}


void Foam::searchableExtrudedCircle::findNearest
(
    const pointField& samples,
    const scalarField& nearestDistSqr,
    List<pointIndexHit>& info
) const
{
    const indexedOctree<treeDataEdge>& tree = edgeTree_();

    info.setSize(samples.size());

    forAll(samples, i)
    {
        const scalar nearestDist = Foam::sqrt(nearestDistSqr[i]);
        const scalar searchDistSqr = Foam::sqr(nearestDist+radius_);

        // Find nearest on central edge
        info[i] = tree.findNearest(samples[i], searchDistSqr);

        if (info[i].hit())
        {
            // Derive distance to nearest surface from distance to nearest edge
            const vector d(samples[i] - info[i].point());
            const scalar s(mag(d));

            if (s < ROOTVSMALL)
            {
                // Point is on edge. TBD.
                info[i].setMiss();
            }
            else
            {
                const scalar distToSurface = radius_-s;
                if (mag(distToSurface) > nearestDist)
                {
                    info[i].setMiss();
                }
                else
                {
                    info[i].setPoint(info[i].point() + d/s*radius_);
                }
            }
        }
    }
}


void Foam::searchableExtrudedCircle::findParametricNearest
(
    const point& start,
    const point& end,
    const scalarField& rawLambdas,
    const scalarField& nearestDistSqr,
    List<pointIndexHit>& info
) const
{
    const edgeMesh& mesh = eMeshPtr_();
    const indexedOctree<treeDataEdge>& tree = edgeTree_();
    const auto& treeData = tree.shapes();
    const edgeList& edges = mesh.edges();
    const pointField& points = mesh.points();
    const labelListList& pointEdges = mesh.pointEdges();

    const scalar maxDistSqr = bounds().magSqr();

    // Normalise lambdas
    const scalarField lambdas
    (
        (rawLambdas-rawLambdas[0])
       /(rawLambdas.last()-rawLambdas[0])
    );


    // Nearest point on curve and local axis direction
    pointField curvePoints(lambdas.size());
    vectorField axialVecs(lambdas.size());

    const pointIndexHit startInfo = tree.findNearest(start, maxDistSqr);
    curvePoints[0] = startInfo.hitPoint();
    axialVecs[0] = treeData.line(startInfo.index()).vec();

    const pointIndexHit endInfo = tree.findNearest(end, maxDistSqr);
    curvePoints.last() = endInfo.hitPoint();
    axialVecs.last() = treeData.line(endInfo.index()).vec();



    scalarField curveLambdas(points.size(), -1.0);

    {
        scalar endDistance = -1.0;

        // Determine edge lengths walking from start to end.

        const point& start = curvePoints[0];
        const point& end = curvePoints.last();

        label edgei = startInfo.index();
        const edge& startE = edges[edgei];

        label pointi = startE[0];
        if ((startE.vec(points)&(end-start)) > 0)
        {
            pointi = startE[1];
        }

        curveLambdas[pointi] = mag(points[pointi]-curvePoints[0]);
        label otherPointi = startE.otherVertex(pointi);
        curveLambdas[otherPointi] = -mag(points[otherPointi]-curvePoints[0]);

        //Pout<< "for point:" << points[pointi] << " have distance "
        //    << curveLambdas[pointi] << endl;


        while (true)
        {
            const labelList& pEdges = pointEdges[pointi];
            if (pEdges.size() == 1)
            {
                break;
            }
            else if (pEdges.size() != 2)
            {
                FatalErrorInFunction << "Curve " << name()
                    << " is not a single path. This is not supported"
                    << exit(FatalError);
                break;
            }

            label oldEdgei = edgei;
            if (pEdges[0] == oldEdgei)
            {
                edgei = pEdges[1];
            }
            else
            {
                edgei = pEdges[0];
            }

            if (edgei == endInfo.index())
            {
                endDistance = curveLambdas[pointi] + mag(end-points[pointi]);

                //Pout<< "Found end edge:" << edges[edgei].centre(points)
                //    << " endPt:" << end
                //    << " point before:" << points[pointi]
                //    << " accumulated length:" << endDistance << endl;
            }


            label oldPointi = pointi;
            pointi = edges[edgei].otherVertex(oldPointi);

            if (curveLambdas[pointi] >= 0)
            {
                break;
            }

            curveLambdas[pointi] =
                curveLambdas[oldPointi] + edges[edgei].mag(points);
        }

        // Normalise curveLambdas
        forAll(curveLambdas, i)
        {
            if (curveLambdas[i] >= 0)
            {
                curveLambdas[i] /= endDistance;
            }
        }
    }



    // Interpolation engine
    linearInterpolationWeights interpolator(curveLambdas);

    // Find wanted location along curve
    labelList indices;
    scalarField weights;
    for (label i = 1; i < curvePoints.size()-1; i++)
    {
        interpolator.valueWeights(lambdas[i], indices, weights);

        if (indices.size() == 1)
        {
            // On outside of curve. Choose one of the connected edges.
            label pointi = indices[0];
            const point& p0 = points[pointi];
            label edge0 = pointEdges[pointi][0];
            const edge& e0 = edges[edge0];
            axialVecs[i] = e0.vec(points);
            curvePoints[i] = weights[0]*p0;
        }
        else if (indices.size() == 2)
        {
            const point& p0 = points[indices[0]];
            const point& p1 = points[indices[1]];
            axialVecs[i] = p1-p0;
            curvePoints[i] = weights[0]*p0+weights[1]*p1;
        }
    }
    axialVecs /= mag(axialVecs);


    // Now we have the lambdas, curvePoints and axialVecs.



    info.setSize(lambdas.size());
    info = pointIndexHit();

    // Given the current lambdas interpolate radial direction inbetween
    // endpoints (all projected onto the starting coordinate system)
    quaternion qStart;
    vector radialStart;
    {
        radialStart = start-curvePoints[0];
        radialStart.removeCollinear(axialVecs[0]);
        radialStart.normalise();

        qStart = quaternion(radialStart, 0.0);

        info[0] = pointIndexHit(true, start, 0);
    }

    quaternion qProjectedEnd;
    {
        vector radialEnd(end-curvePoints.last());
        radialEnd.removeCollinear(axialVecs.last());
        radialEnd.normalise();

        vector projectedEnd = radialEnd;
        projectedEnd.removeCollinear(axialVecs[0]);
        projectedEnd.normalise();

        qProjectedEnd = quaternion(projectedEnd, 0.0);

        info.last() = pointIndexHit(true, end, 0);
    }

    for (label i = 1; i < lambdas.size()-1; i++)
    {
        quaternion q(slerp(qStart, qProjectedEnd, lambdas[i]));
        vector radialDir(q.transform(radialStart));

        radialDir.removeCollinear(axialVecs[i]);
        radialDir.normalise();

        info[i] = pointIndexHit(true, curvePoints[i]+radius_*radialDir, 0);
    }
}


void Foam::searchableExtrudedCircle::getRegion
(
    const List<pointIndexHit>& info,
    labelList& region
) const
{
    region.setSize(info.size());
    region = 0;
}


void Foam::searchableExtrudedCircle::getNormal
(
    const List<pointIndexHit>& info,
    vectorField& normal
) const
{
    const edgeMesh& mesh = eMeshPtr_();
    const indexedOctree<treeDataEdge>& tree = edgeTree_();
    const edgeList& edges = mesh.edges();
    const pointField& points = mesh.points();

    normal.setSize(info.size());
    normal = Zero;

    const scalar distSqr = bounds().magSqr();

    forAll(info, i)
    {
        if (info[i].hit())
        {
            // Find nearest on curve
            pointIndexHit curvePt = tree.findNearest(info[i].point(), distSqr);

            normal[i] = info[i].hitPoint()-curvePt.hitPoint();

            // Subtract axial direction
            const vector axialVec = edges[curvePt.index()].unitVec(points);

            normal[i].removeCollinear(axialVec);
            normal[i].normalise();
        }
    }
}


// ************************************************************************* //
