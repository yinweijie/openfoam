/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
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

#include "meshes/primitiveMesh/primitiveMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

bool Foam::primitiveMesh::checkEdgeLength
(
    const bool report,
    const scalar reportLenSqr,
    labelHashSet* setPtr
) const
{
    const pointField& points = this->points();
    const faceList& faces = this->faces();

    scalar minLenSqr = sqr(GREAT);
    scalar maxLenSqr = -sqr(GREAT);

    labelHashSet smallEdgeSet(nPoints()/100);

    forAll(faces, facei)
    {
        const face& f = faces[facei];

        forAll(f, fp)
        {
            label fp1 = f.fcIndex(fp);

            scalar magSqrE = magSqr(points[f[fp]] - points[f[fp1]]);

            if (magSqrE < reportLenSqr)
            {
                smallEdgeSet.insert(f[fp]);
                smallEdgeSet.insert(f[fp1]);
            }

            minLenSqr = min(minLenSqr, magSqrE);
            maxLenSqr = max(maxLenSqr, magSqrE);
        }
    }

    reduce(minLenSqr, minOp<scalar>());
    reduce(maxLenSqr, maxOp<scalar>());

    label nSmall = returnReduce(smallEdgeSet.size(), sumOp<label>());

    if (setPtr)
    {
        setPtr->transfer(smallEdgeSet);
    }

    if (nSmall > 0)
    {
        if (report)
        {
            Info<< "   *Edges too small, min/max edge length = "
                << sqrt(minLenSqr) << " " << sqrt(maxLenSqr)
                << ", number too small: " << nSmall << endl;
        }

        return true;
    }

    if (report)
    {
        Info<< "    Min/max edge length = "
            << sqrt(minLenSqr) << " " << sqrt(maxLenSqr) << " OK." << endl;
    }

    return false;
}


// ************************************************************************* //
