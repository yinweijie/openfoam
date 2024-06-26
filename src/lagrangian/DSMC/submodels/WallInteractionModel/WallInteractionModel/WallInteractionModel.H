/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
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
    Foam::WallInteractionModel

Description
    Templated wall interaction model class

SourceFiles
    WallInteractionModel.C
    WallInteractionModelNew.C

\*---------------------------------------------------------------------------*/

#ifndef WallInteractionModel_H
#define WallInteractionModel_H

#include "db/IOobjects/IOdictionary/IOdictionary.H"
#include "memory/autoPtr/autoPtr.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Class WallInteractionModel Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class WallInteractionModel
{
    // Private data

        //- Reference to the owner cloud class
        CloudType& owner_;

        //- Reference to the cloud dictionary
        const dictionary& dict_;

        //- The coefficients dictionary
        const dictionary coeffDict_;


public:

    //- Runtime type information
    TypeName("WallInteractionModel");

    //- Declare runtime constructor selection table
    declareRunTimeSelectionTable
    (
        autoPtr,
        WallInteractionModel,
        dictionary,
        (
            const dictionary& dict,
            CloudType& owner
        ),
        (dict, owner)
    );


    // Constructors

        //- Construct null from owner
        WallInteractionModel(CloudType& owner);

        //- Construct from components
        WallInteractionModel
        (
            const dictionary& dict,
            CloudType& owner,
            const word& type
        );


    //- Destructor
    virtual ~WallInteractionModel();


    //- Selector
    static autoPtr<WallInteractionModel<CloudType>> New
    (
        const dictionary& dict,
        CloudType& owner
    );


    // Access

        //- Return the owner cloud object
        const CloudType& owner() const;

        //- Return non-const access to the owner cloud object
        CloudType& owner();

        //- Return the dictionary
        const dictionary& dict() const;

        //- Return the coefficients dictionary
        const dictionary& coeffDict() const;


    // Member Functions

        //- Apply wall correction
        virtual void correct(typename CloudType::parcelType& p) = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#define makeWallInteractionModel(CloudType)                                    \
                                                                               \
    defineNamedTemplateTypeNameAndDebug(WallInteractionModel<CloudType>, 0);   \
                                                                               \
    defineTemplateRunTimeSelectionTable                                        \
    (                                                                          \
        WallInteractionModel<CloudType>,                                       \
        dictionary                                                             \
    );


#define makeWallInteractionModelType(SS, CloudType)                            \
                                                                               \
    defineNamedTemplateTypeNameAndDebug(SS<CloudType>, 0);                     \
                                                                               \
    WallInteractionModel<CloudType>::                                          \
        adddictionaryConstructorToTable<SS<CloudType>>                         \
            add##SS##CloudType##ConstructorToTable_;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "submodels/WallInteractionModel/WallInteractionModel/WallInteractionModel.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
