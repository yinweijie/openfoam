/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2015-2017 OpenFOAM Foundation
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

Typedef
    Foam::globalIOLists

Description
    Typedefs for globalIOLists of primitive types. These are fully compatible
    with 'normal' IOLists except have global filePath() scope.

\*---------------------------------------------------------------------------*/

#ifndef Foam_globalIOLists_H
#define Foam_globalIOLists_H

#include "fields/Fields/primitiveFields.H"
#include "db/IOobjects/GlobalIOList/GlobalIOList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
    typedef GlobalIOList<label> labelGlobalIOList;
    typedef GlobalIOList<scalar> scalarGlobalIOList;
    typedef GlobalIOList<vector> vectorGlobalIOList;
    typedef GlobalIOList<sphericalTensor> sphericalTensorGlobalIOList;
    typedef GlobalIOList<symmTensor> symmTensorGlobalIOList;
    typedef GlobalIOList<tensor> tensorGlobalIOList;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
