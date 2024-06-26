/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2022 OpenCFD Ltd.
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

#include "topoSet/pointSources/clipPlaneToPoint/clipPlaneToPoint.H"
#include "meshes/polyMesh/polyMesh.H"
#include "db/runTimeSelection/construction/addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(clipPlaneToPoint, 0);
    addToRunTimeSelectionTable(topoSetSource, clipPlaneToPoint, word);
    addToRunTimeSelectionTable(topoSetSource, clipPlaneToPoint, istream);
    addToRunTimeSelectionTable(topoSetPointSource, clipPlaneToPoint, word);
    addToRunTimeSelectionTable(topoSetPointSource, clipPlaneToPoint, istream);
    addNamedToRunTimeSelectionTable
    (
        topoSetPointSource,
        clipPlaneToPoint,
        word,
        clipPlane
    );
    addNamedToRunTimeSelectionTable
    (
        topoSetPointSource,
        clipPlaneToPoint,
        istream,
        clipPlane
    );
}


Foam::topoSetSource::addToUsageTable Foam::clipPlaneToPoint::usage_
(
    clipPlaneToPoint::typeName,
    "\n    Usage: clipPlaneToPoint (px py pz) (nx ny nz)\n\n"
    "    Select points above the plane\n\n"
);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::clipPlaneToPoint::combine(topoSet& set, const bool add) const
{
    // Mesh points above plane

    const pointField& ctrs = mesh_.points();

    forAll(ctrs, elemi)
    {
        if (((ctrs[elemi] - point_) & normal_) > 0)
        {
            addOrDelete(set, elemi, add);
        }
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::clipPlaneToPoint::clipPlaneToPoint
(
    const polyMesh& mesh,
    const point& basePoint,
    const vector& normal
)
:
    topoSetPointSource(mesh),
    point_(basePoint),
    normal_(normal)
{}


Foam::clipPlaneToPoint::clipPlaneToPoint
(
    const polyMesh& mesh,
    const dictionary& dict
)
:
    clipPlaneToPoint
    (
        mesh,
        dict.get<vector>("point"),
        dict.get<vector>("normal")
    )
{}


Foam::clipPlaneToPoint::clipPlaneToPoint
(
    const polyMesh& mesh,
    Istream& is
)
:
    topoSetPointSource(mesh),
    point_(checkIs(is)),
    normal_(checkIs(is))
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void Foam::clipPlaneToPoint::applyToSet
(
    const topoSetSource::setAction action,
    topoSet& set
) const
{
    if (action == topoSetSource::ADD || action == topoSetSource::NEW)
    {
        if (verbose_)
        {
            Info<< "    Adding points above plane at "
                << point_ << " with normal " << normal_ << endl;
        }

        combine(set, true);
    }
    else if (action == topoSetSource::SUBTRACT)
    {
        if (verbose_)
        {
            Info<< "    Removing points above plane at "
                << point_ << " with normal " << normal_ << endl;
        }

        combine(set, false);
    }
}


// ************************************************************************* //
