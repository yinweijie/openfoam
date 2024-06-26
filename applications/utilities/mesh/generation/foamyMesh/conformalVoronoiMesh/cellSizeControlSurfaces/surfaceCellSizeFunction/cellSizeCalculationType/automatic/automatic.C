/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2017 OpenFOAM Foundation
    Copyright (C) 2016-2022 OpenCFD Ltd.
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

#include "cellSizeControlSurfaces/surfaceCellSizeFunction/cellSizeCalculationType/automatic/automatic.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"
#include "searchableSurfaces/triSurfaceMesh/triSurfaceMesh.H"
#include "vtk/write/foamVtkSurfaceWriter.H"
#include "interpolations/primitivePatchInterpolation/primitivePatchInterpolation.H"
#include "db/Time/TimeOpenFOAM.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(automatic, 0);
    addToRunTimeSelectionTable(cellSizeCalculationType, automatic, dictionary);
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::automatic::smoothField(triSurfaceScalarField& surf)
{
    label nSmoothingIterations = 10;

    for (label iter = 0; iter < nSmoothingIterations; ++iter)
    {
        const pointField& faceCentres = surface_.faceCentres();

        forAll(surf, sI)
        {
            const labelList& faceFaces = surface_.faceFaces()[sI];

            const point& fC = faceCentres[sI];
            const scalar value = surf[sI];

            scalar newValue = 0;
            scalar totalDist = 0;

            label nFaces = 0;

            forAll(faceFaces, fI)
            {
                const label faceLabel = faceFaces[fI];
                const point& faceCentre = faceCentres[faceLabel];

                const scalar faceValue = surf[faceLabel];
                const scalar distance = mag(faceCentre - fC);

                newValue += faceValue/(distance + SMALL);

                totalDist += 1.0/(distance + SMALL);

                if (value < faceValue)
                {
                    nFaces++;
                }
            }

            // Do not smooth out the peak values
            if (nFaces == faceFaces.size())
            {
                continue;
            }

            surf[sI] = newValue/totalDist;
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::automatic::automatic
(
    const dictionary& cellSizeCalcTypeDict,
    const triSurfaceMesh& surface,
    const scalar defaultCellSize
)
:
    cellSizeCalculationType
    (
        typeName,
        cellSizeCalcTypeDict,
        surface,
        defaultCellSize
    ),
    coeffsDict_(cellSizeCalcTypeDict.optionalSubDict(typeName + "Coeffs")),
    surfaceName_(surface.searchableSurface::name()),

    readCurvature_(coeffsDict_.get<bool>("curvature")),
    readFeatureProximity_(coeffsDict_.get<bool>("featureProximity")),
    readInternalCloseness_(coeffsDict_.get<bool>("internalCloseness")),

    curvatureFile_(coeffsDict_.get<word>("curvatureFile")),
    featureProximityFile_(coeffsDict_.get<word>("featureProximityFile")),
    internalClosenessFile_(coeffsDict_.get<word>("internalClosenessFile")),

    curvatureCellSizeCoeff_
    (
        coeffsDict_.get<scalar>("curvatureCellSizeCoeff")
    ),
    maximumCellSize_
    (
        coeffsDict_.get<scalar>("maximumCellSizeCoeff") * defaultCellSize
    )
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

Foam::tmp<Foam::triSurfacePointScalarField> Foam::automatic::load()
{
    Info<< indent
        << "Calculating cell size on surface: " << surfaceName_ << endl;

    tmp<triSurfacePointScalarField> tPointCellSize
    (
        new triSurfacePointScalarField
        (
            IOobject
            (
                surfaceName_ + ".cellSize",
                surface_.searchableSurface::time().constant(),
                "triSurface",
                surface_.searchableSurface::time(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            surface_,
            dimLength,
            scalarField(surface_.nPoints(), maximumCellSize_)
        )
    );

    triSurfacePointScalarField& pointCellSize = tPointCellSize.ref();

    if (readCurvature_)
    {
        Info<< indent
            << "Reading curvature         : " << curvatureFile_ << endl;

        triSurfacePointScalarField curvature
        (
            IOobject
            (
                curvatureFile_,
                surface_.searchableSurface::time().constant(),
                "triSurface",
                surface_.searchableSurface::time(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            surface_,
            dimLength,
            true
        );

        forAll(pointCellSize, pI)
        {
            pointCellSize[pI] =
                min
                (
                    1.0
                   /max
                    (
                        (1.0/curvatureCellSizeCoeff_)*mag(curvature[pI]),
                        1.0/maximumCellSize_
                    ),
                    pointCellSize[pI]
                );
        }
    }

    PrimitivePatchInterpolation
    <
        PrimitivePatch<::Foam::List<labelledTri>, pointField>
    > patchInterpolate(surface_);

    const Map<label>& meshPointMap = surface_.meshPointMap();

    if (readInternalCloseness_)
    {
        Info<< indent
            << "Reading internal closeness: " << internalClosenessFile_ << endl;

        triSurfaceScalarField internalCloseness
        (
            IOobject
            (
                internalClosenessFile_,
                surface_.searchableSurface::time().constant(),
                "triSurface",
                surface_.searchableSurface::time(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            surface_,
            dimLength,
            true
        );

        scalarField internalClosenessPointField
        (
            patchInterpolate.faceToPointInterpolate(internalCloseness)
        );

        forAll(pointCellSize, pI)
        {
            pointCellSize[pI] =
                min
                (
                    internalClosenessPointField[meshPointMap[pI]],
                    pointCellSize[pI]
                );
        }
    }

    if (readFeatureProximity_)
    {
        Info<< indent
            << "Reading feature proximity : " << featureProximityFile_ << endl;

        triSurfaceScalarField featureProximity
        (
            IOobject
            (
                featureProximityFile_,
                surface_.searchableSurface::time().constant(),
                "triSurface",
                surface_.searchableSurface::time(),
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            surface_,
            dimLength,
            true
        );

        scalarField featureProximityPointField
        (
            patchInterpolate.faceToPointInterpolate(featureProximity)
        );

        forAll(pointCellSize, pI)
        {
            pointCellSize[pI] =
                min
                (
                    featureProximityPointField[meshPointMap[pI]],
                    pointCellSize[pI]
                );
        }
    }

    //smoothField(surfaceCellSize);

    pointCellSize.write();

    if (debug)
    {
        faceList faces(surface_.size());

        forAll(surface_, fI)
        {
            faces[fI] = surface_.triSurface::operator[](fI);
        }

        vtk::surfaceWriter vtkWriter
        (
            surface_.points(),
            faces,
            (
                surface_.searchableSurface::time().constant()
              / "triSurface"
              / surfaceName_.stem() + "_cellSize"
            )
        );


        vtkWriter.writeGeometry();

        vtkWriter.beginPointData(1);

        vtkWriter.write("cellSize", pointCellSize);
    }

    return tPointCellSize;
}


// ************************************************************************* //
