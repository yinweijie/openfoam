/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011 OpenFOAM Foundation
    Copyright (C) 2023 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM, distributed under GPL-3.0-or-later.

Description
    Add -region option

Required Classes
    - Foam::argList

\*---------------------------------------------------------------------------*/

{
    Foam::argList::addOption
    (
        "region",
        "name",
        "Specify mesh region (default: region0)"
    );
}


// ************************************************************************* //
