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
    Foam::regionModels::surfaceFilmModels::filmSubModelBase

Description
    Base class for surface film sub-models

SourceFiles
    filmSubModelBaseI.H
    filmSubModelBase.C

\*---------------------------------------------------------------------------*/

#ifndef filmSubModelBase_H
#define filmSubModelBase_H

#include "surfaceFilmRegionModel/surfaceFilmRegionModel.H"
#include "primitives/subModelBase/subModelBase.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace regionModels
{
namespace surfaceFilmModels
{

/*---------------------------------------------------------------------------*\
                      Class filmSubModelBase Declaration
\*---------------------------------------------------------------------------*/

class filmSubModelBase
:
    public subModelBase
{
protected:

    // Protected data

        //- Reference to the film surface film model
        surfaceFilmRegionModel& filmModel_;


public:

    // Constructors

        //- Construct null
        filmSubModelBase(surfaceFilmRegionModel& film);

        //- Construct from film film without name
        filmSubModelBase
        (
            surfaceFilmRegionModel& film,
            const dictionary& dict,
            const word& baseName,
            const word& modelType,
            const word& dictExt = "Coeffs"
        );

        //- Construct from film film with name
        filmSubModelBase
        (
            const word& modelName,
            surfaceFilmRegionModel& film,
            const dictionary& dict,
            const word& baseName,
            const word& modelType
        );


    //- Destructor
    virtual ~filmSubModelBase();


    // Member Functions

        // Access

            //- Flag to indicate when to write a property
            virtual bool writeTime() const;

            //- Return const access to the film surface film model
            inline const surfaceFilmRegionModel& film() const;

            //- Return the reference to the film surface film model
            inline surfaceFilmRegionModel& film();

            template<class FilmType>
            inline const FilmType& filmType() const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace surfaceFilmModels
} // End namespace regionModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "submodels/filmSubModelBaseI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "submodels/filmSubModelBaseTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
