/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2015 OpenFOAM Foundation
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
    Foam::mapDistributeLagrangian

Description
    Mesh-to-mesh mapping information for particles

\*---------------------------------------------------------------------------*/

#ifndef mapDistributeLagrangian_H
#define mapDistributeLagrangian_H

#include "meshes/polyMesh/mapPolyMesh/mapDistribute/mapDistribute.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class mapPolyMesh;

/*---------------------------------------------------------------------------*\
                   Class mapDistributeLagrangian Declaration
\*---------------------------------------------------------------------------*/

class mapDistributeLagrangian
{
    // Private Data

        //- Map to distribute particles
        const mapDistribute particleMap_;

        //- Per element in subsetted mesh the cell label
        const labelListList constructCellLabels_;


public:

    // Constructors

        //- Move construct from components
        mapDistributeLagrangian
        (
            const label nNewParticles,
            labelListList&& subParticleMap,
            labelListList&& constructParticleMap,
            labelListList&& constructCellLabels
        )
        :
            particleMap_
            (
                nNewParticles,
                std::move(subParticleMap),
                std::move(constructParticleMap)
            ),
            constructCellLabels_(std::move(constructCellLabels))
        {}


    // Member Functions

        // Access

            //- Distribution map
            const mapDistribute& particleMap() const
            {
                return particleMap_;
            }

            //- Per received particle the destination cell label
            const labelListList& constructCellLabels() const
            {
                return constructCellLabels_;
            }


        // Edit

            //- Distribute list of lagrangian data
            template<class T>
            void distributeLagrangianData(List<T>& lst) const
            {
                particleMap_.distribute(lst);
            }

            //- Correct for topo change.
            void updateMesh(const mapPolyMesh&)
            {
                NotImplemented;
            }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
