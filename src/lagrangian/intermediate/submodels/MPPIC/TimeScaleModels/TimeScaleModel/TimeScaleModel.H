/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2013-2016 OpenFOAM Foundation
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
    Foam::TimeScaleModel

Group
    grpLagrangianIntermediateMPPICTimeScaleSubModels

Description
    Base class for time scale models.

SourceFiles
    TimeScaleModel.C

\*---------------------------------------------------------------------------*/

#ifndef TimeScaleModel_H
#define TimeScaleModel_H

#include "global/constants/constants.H"
#include "fields/Fields/Field/Field.H"
#include "fields/FieldFields/FieldField/FieldField.H"
#include "db/dictionary/dictionary.H"
#include "db/runTimeSelection/construction/runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                        Class TimeScaleModel Declaration
\*---------------------------------------------------------------------------*/

class TimeScaleModel
{
private:

    //- Private member functions

        //- No copy assignment
        void operator=(const TimeScaleModel&) = delete;


protected:

    // Protected data

        //- Close pack volume fraction
        scalar alphaPacked_;

        //- Coefficient of restitution
        scalar e_;


public:

    //- Runtime type information
    TypeName("timeScaleModel");

    //- Declare runtime constructor selection table
    declareRunTimeSelectionTable
    (
        autoPtr,
        TimeScaleModel,
        dictionary,
        (const dictionary& dict),
        (dict)
    );


    //- Constructors

        //- Construct from components
        TimeScaleModel(const dictionary& dict);

        //- Construct a copy
        TimeScaleModel(const TimeScaleModel& sm);

        //- Construct and return a clone
        virtual autoPtr<TimeScaleModel> clone() const = 0;


    //- Selector
    static autoPtr<TimeScaleModel> New
    (
        const dictionary& dict
    );


    //- Destructor
    virtual ~TimeScaleModel();


    //- Member Functions

        //- Time scale
        virtual tmp<FieldField<Field, scalar>> oneByTau
        (
            const FieldField<Field, scalar>& alpha,
            const FieldField<Field, scalar>& r32,
            const FieldField<Field, scalar>& uSqr,
            const FieldField<Field, scalar>& f
        ) const = 0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
