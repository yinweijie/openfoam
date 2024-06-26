/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2012-2017 OpenFOAM Foundation
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
    Foam::regionModels::regionModelFunctionObjectList

Description
    List of cloud function objects

SourceFiles
    regionModelFunctionObjectListI.H
    regionModelFunctionObjectList.C

\*---------------------------------------------------------------------------*/

#ifndef regionModelFunctionObjectList_H
#define regionModelFunctionObjectList_H

#include "containers/PtrLists/PtrList/PtrList.H"
#include "regionModelFunctionObject/regionModelFunctionObject/regionModelFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{

class regionModel;

/*---------------------------------------------------------------------------*\
                Class regionModelFunctionObjectList Declaration
\*---------------------------------------------------------------------------*/

class regionModelFunctionObjectList
:
    public PtrList<regionModelFunctionObject>
{
protected:

    // Protected Data

        //- Reference to the region region model
        regionModel& regionModel_;

        //- Dictionary
        const dictionary dict_;


public:

    // Constructors

        //- Null constructor
        regionModelFunctionObjectList(regionModel& region);

        //- Construct from mesh
        regionModelFunctionObjectList
        (
            regionModel& region,
            const dictionary& dict,
            const bool readFields = true
        );

        //- Construct copy
        regionModelFunctionObjectList
        (
            const regionModelFunctionObjectList& rmfol
        );


    //- Destructor
    virtual ~regionModelFunctionObjectList();


    // Member Functions

        // Access

            //- Return const access to the cloud region
            inline const regionModel& region() const;

            //- Return reference to the cloud region
            inline regionModel& region();

            //- Return the forces dictionary
            inline const dictionary& dict() const;


        // Evaluation

            //- Pre-evolve hook
            virtual void preEvolveRegion();

            //- Post-evolve hook
            virtual void postEvolveRegion();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace regionModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "regionModelFunctionObject/regionModelFunctionObject/regionModelFunctionObjectListI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
