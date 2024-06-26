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

\*---------------------------------------------------------------------------*/

#include "meshes/meshShapes/face/face.H"
#include "containers/Lists/DynamicList/DynamicList.H"

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<int SizeMin>
Foam::label Foam::face::triangles
(
    const UList<point>& points,
    DynamicList<face, SizeMin>& triFaces
) const
{
    label triI = triFaces.size();
    label quadI = 0;
    faceList quadFaces;

    // adjust the addressable size (and allocate space if needed)
    triFaces.setSize(triI + nTriangles());

    return split(SPLITTRIANGLE, points, triI, quadI, triFaces, quadFaces);
}


template<class Type>
Type Foam::face::average
(
    const UList<point>& meshPoints,
    const Field<Type>& fld
) const
{
    // Calculate the average by breaking the face into triangles and
    // area-weighted averaging their averages

    // If the face is a triangle, do a direct calculation
    if (size() == 3)
    {
        return
            (1.0/3.0)
           *(
               fld[operator[](0)]
             + fld[operator[](1)]
             + fld[operator[](2)]
            );
    }

    label nPoints = size();

    point centrePoint = Zero;
    Type cf = Zero;

    for (label pI=0; pI<nPoints; pI++)
    {
        centrePoint += meshPoints[operator[](pI)];
        cf += fld[operator[](pI)];
    }

    centrePoint /= nPoints;
    cf /= nPoints;

    scalar sumA = 0;
    Type sumAf = Zero;

    for (label pI=0; pI<nPoints; pI++)
    {
        // Calculate 3*triangle centre field value
        Type ttcf  =
        (
            fld[operator[](pI)]
          + fld[operator[]((pI + 1) % nPoints)]
          + cf
        );

        // Calculate 2*triangle area
        scalar ta = Foam::mag
        (
            (meshPoints[operator[](pI)] - centrePoint)
          ^ (meshPoints[operator[]((pI + 1) % nPoints)] - centrePoint)
        );

        sumA += ta;
        sumAf += ta*ttcf;
    }

    if (sumA > VSMALL)
    {
        return sumAf/(3*sumA);
    }
    else
    {
        return cf;
    }
}


// ************************************************************************* //
