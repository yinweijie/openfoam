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
    Foam::InjectionModelList

Description
    List of injection models

SourceFiles
    InjectionModelListList.C

\*---------------------------------------------------------------------------*/

#ifndef InjectionModelList_H
#define InjectionModelList_H

#include "containers/PtrLists/PtrList/PtrList.H"
#include "submodels/Kinematic/InjectionModel/InjectionModel/InjectionModelPascal.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Class InjectionModelList Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class InjectionModelList
:
    public PtrList<InjectionModel<CloudType>>
{
public:

    // Constructors

        //- Construct null from owner
        InjectionModelList(CloudType& owner);

        //- Construct from dictionary and cloud owner
        InjectionModelList(const dictionary& dict, CloudType& owner);

        //- Construct copy
        InjectionModelList(const InjectionModelList<CloudType>& im);

        //- Construct and return a clone
        virtual autoPtr<InjectionModelList<CloudType>> clone() const
        {
            return autoPtr<InjectionModelList<CloudType>>
            (
                new InjectionModelList<CloudType>(*this)
            );
        }


    //- Destructor
    virtual ~InjectionModelList() = default;



    // Member Functions

        // Access

            //- Return the minimum start-of-injection time
            scalar timeStart() const;

            //- Return the maximum end-of-injection time
            scalar timeEnd() const;

            //- Volume of parcels to introduce relative to SOI
            scalar volumeToInject(const scalar time0, const scalar time1);

            //- Return the average parcel mass
            scalar averageParcelMass();


        // Edit

            //- Set injector locations when mesh is updated
            void updateMesh();


        // Per-injection event functions

            //- Main injection loop
            template<class TrackCloudType>
            void inject
            (
                TrackCloudType& cloud,
                typename CloudType::parcelType::trackingData& td
            );

            //- Main injection loop - steady-state
            template<class TrackCloudType>
            void injectSteadyState
            (
                TrackCloudType& cloud,
                typename CloudType::parcelType::trackingData& td,
                const scalar trackTime
            );


        // I-O

            //- Write injection info
            virtual void info();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "submodels/Kinematic/InjectionModel/InjectionModel/InjectionModelListPascal.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
