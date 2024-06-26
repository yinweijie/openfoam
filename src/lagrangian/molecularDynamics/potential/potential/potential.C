/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2020 OpenCFD Ltd.
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

#include "potential/potential.H"

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::potential::setSiteIdList(const dictionary& moleculePropertiesDict)
{
    DynamicList<word> siteIdList;
    DynamicList<word> pairPotentialSiteIdList;

    forAll(idList_, i)
    {
        const word& id(idList_[i]);

        if (!moleculePropertiesDict.found(id))
        {
            FatalErrorInFunction
                << id << " molecule subDict not found"
                << nl << abort(FatalError);
        }

        const dictionary& molDict(moleculePropertiesDict.subDict(id));

        List<word> siteIdNames
        (
            molDict.lookup("siteIds")
        );

        forAll(siteIdNames, sI)
        {
            const word& siteId = siteIdNames[sI];

            if (!siteIdList.found(siteId))
            {
                siteIdList.append(siteId);
            }
        }

        List<word> pairPotSiteIds
        (
            molDict.lookup("pairPotentialSiteIds")
        );

        forAll(pairPotSiteIds, sI)
        {
            const word& siteId = pairPotSiteIds[sI];

            if (!siteIdNames.found(siteId))
            {
                FatalErrorInFunction
                    << siteId << " in pairPotentialSiteIds is not in siteIds: "
                    << siteIdNames << nl << abort(FatalError);
            }

            if (!pairPotentialSiteIdList.found(siteId))
            {
                pairPotentialSiteIdList.append(siteId);
            }
        }
    }

    nPairPotIds_ = pairPotentialSiteIdList.size();

    forAll(siteIdList, aSIN)
    {
        const word& siteId = siteIdList[aSIN];

        if (!pairPotentialSiteIdList.found(siteId))
        {
            pairPotentialSiteIdList.append(siteId);
        }
    }

    siteIdList_.transfer(pairPotentialSiteIdList);
}


void Foam::potential::potential::readPotentialDict()
{
    Info<< nl <<  "Reading potential dictionary:" << endl;

    IOdictionary idListDict
    (
        IOobject
        (
            "idList",
            mesh_.time().constant(),
            mesh_,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    );

    idListDict.readEntry("idList", idList_);

    setSiteIdList
    (
        IOdictionary
        (
            IOobject
            (
                "moleculeProperties",
                mesh_.time().constant(),
                mesh_,
                IOobject::MUST_READ_IF_MODIFIED,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            )
        )
    );

    List<word> pairPotentialSiteIdList
    (
        SubList<word>(siteIdList_, nPairPotIds_)
    );

    Info<< nl << "Unique site ids found: " << siteIdList_
        << nl << "Site Ids requiring a pair potential: "
        << pairPotentialSiteIdList
        << endl;

    List<word> tetherSiteIdList;
    idListDict.readIfPresent("tetherSiteIdList", tetherSiteIdList);

    IOdictionary potentialDict
    (
        IOobject
        (
            "potentialDict",
            mesh_.time().system(),
            mesh_,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    );

    potentialDict.readEntry("potentialEnergyLimit", potentialEnergyLimit_);

    List<word> remOrd;

    if (potentialDict.readIfPresent("removalOrder", remOrd))
    {
        removalOrder_.setSize(remOrd.size());

        forAll(removalOrder_, rO)
        {
            removalOrder_[rO] = idList_.find(remOrd[rO]);

            if (removalOrder_[rO] == -1)
            {
                FatalErrorInFunction
                    << "removalOrder entry: " << remOrd[rO]
                    << " not found in idList."
                    << nl << abort(FatalError);
            }
        }
    }

    // *************************************************************************
    // Pair potentials

    if (!potentialDict.found("pair"))
    {
        FatalErrorInFunction
            << "pair potential specification subDict not found"
            << abort(FatalError);
    }

    const dictionary& pairDict = potentialDict.subDict("pair");

    pairPotentials_.buildPotentials
    (
        pairPotentialSiteIdList,
        pairDict,
        mesh_
    );

    // *************************************************************************
    // Tether potentials

    if (tetherSiteIdList.size())
    {
        if (!potentialDict.found("tether"))
        {
            FatalErrorInFunction
                << "tether potential specification subDict not found"
                << abort(FatalError);
        }

        const dictionary& tetherDict = potentialDict.subDict("tether");

        tetherPotentials_.buildPotentials
        (
            siteIdList_,
            tetherDict,
            tetherSiteIdList
        );
    }

    // *************************************************************************
    // External Forces

    gravity_ = Zero;

    if (potentialDict.found("external"))
    {
        Info<< nl << "Reading external forces:" << endl;

        const dictionary& externalDict = potentialDict.subDict("external");

        // gravity
        externalDict.readIfPresent("gravity", gravity_);
    }

    Info<< nl << tab << "gravity = " << gravity_ << endl;
}


void Foam::potential::potential::readMdInitialiseDict
(
    const IOdictionary& mdInitialiseDict,
    IOdictionary& idListDict
)
{
    IOdictionary moleculePropertiesDict
    (
        IOobject
        (
            "moleculeProperties",
            mesh_.time().constant(),
            mesh_,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE,
            IOobject::NO_REGISTER
        )
    );

    DynamicList<word> idList;

    DynamicList<word> tetherSiteIdList;

    forAll(mdInitialiseDict.toc(), zone)
    {
        const dictionary& zoneDict = mdInitialiseDict.subDict
        (
            mdInitialiseDict.toc()[zone]
        );

        List<word> latticeIds
        (
            zoneDict.lookup("latticeIds")
        );

        forAll(latticeIds, i)
        {
            const word& id = latticeIds[i];

            if (!moleculePropertiesDict.found(id))
            {
                FatalErrorInFunction
                    << "Molecule type " << id
                    << " not found in moleculeProperties dictionary." << nl
                    << abort(FatalError);
            }

            if (!idList.found(id))
            {
                idList.append(id);
            }
        }

        List<word> tetherSiteIds
        (
            zoneDict.lookup("tetherSiteIds")
        );

        forAll(tetherSiteIds, t)
        {
            const word& tetherSiteId = tetherSiteIds[t];

            bool idFound = false;

            forAll(latticeIds, i)
            {
                if (idFound)
                {
                    break;
                }

                const word& id = latticeIds[i];

                List<word> siteIds
                (
                    moleculePropertiesDict.subDict(id).lookup("siteIds")
                );

                if (siteIds.found(tetherSiteId))
                {
                    idFound = true;
                }
            }

            if (idFound)
            {
                tetherSiteIdList.append(tetherSiteId);
            }
            else
            {
                FatalErrorInFunction
                    << " not found as a site of any molecule in zone." << nl
                    << abort(FatalError);
            }
        }
    }

    idList_.transfer(idList);

    tetherSiteIdList.shrink();

    idListDict.add("idList", idList_);

    idListDict.add("tetherSiteIdList", tetherSiteIdList);

    setSiteIdList(moleculePropertiesDict);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::potential::potential(const polyMesh& mesh)
:
    mesh_(mesh)
{
    readPotentialDict();
}


Foam::potential::potential
(
    const polyMesh& mesh,
    const IOdictionary& mdInitialiseDict,
    IOdictionary& idListDict
)
:
    mesh_(mesh)
{
    readMdInitialiseDict(mdInitialiseDict, idListDict);
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::potential::~potential()
{}


// ************************************************************************* //
