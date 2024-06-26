/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2018-2021 OpenCFD Ltd.
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

Application
    mapFields

Group
    grpPreProcessingUtilities

Description
    Maps volume fields from one mesh to another, reading and
    interpolating all fields present in the time directory of both cases.

    Parallel and non-parallel cases are handled without the need to reconstruct
    them first.

\*---------------------------------------------------------------------------*/

#include "cfdTools/general/include/fvCFD.H"
#include "meshToMesh0/meshToMesh0.H"
#include "fvMesh/fvPatches/constraint/processor/processorFvPatch.H"
#include "MapMeshes.H"
#include "decompositionModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int readNumProcs
(
    const argList& args,
    const word& optionName,
    const Time& runTime
)
{
    return decompositionMethod::nDomains
    (
        IOdictionary
        (
            IOobject::selectIO
            (
                IOobject
                (
                    decompositionModel::canonicalName,
                    runTime.system(),
                    runTime,
                    IOobject::MUST_READ,
                    IOobject::NO_WRITE,
                    IOobject::NO_REGISTER
                ),
                args.getOrDefault<fileName>(optionName, "")
            )
        )
    );
}


void mapConsistentMesh
(
    const fvMesh& meshSource,
    const fvMesh& meshTarget,
    const meshToMesh0::order& mapOrder,
    const bool subtract
)
{
    if (subtract)
    {
        MapConsistentMesh<minusEqOp>
        (
            meshSource,
            meshTarget,
            mapOrder
        );
    }
    else
    {
        MapConsistentMesh<eqOp>
        (
            meshSource,
            meshTarget,
            mapOrder
        );
    }
}


void mapSubMesh
(
    const fvMesh& meshSource,
    const fvMesh& meshTarget,
    const HashTable<word>& patchMap,
    const wordList& cuttingPatches,
    const meshToMesh0::order& mapOrder,
    const bool subtract
)
{
    if (subtract)
    {
        MapSubMesh<minusEqOp>
        (
            meshSource,
            meshTarget,
            patchMap,
            cuttingPatches,
            mapOrder
        );
    }
    else
    {
        MapSubMesh<eqOp>
        (
            meshSource,
            meshTarget,
            patchMap,
            cuttingPatches,
            mapOrder
        );
    }
}


void mapConsistentSubMesh
(
    const fvMesh& meshSource,
    const fvMesh& meshTarget,
    const meshToMesh0::order& mapOrder,
    const bool subtract
)
{
    if (subtract)
    {
        MapConsistentSubMesh<minusEqOp>
        (
            meshSource,
            meshTarget,
            mapOrder
        );
    }
    else
    {
        MapConsistentSubMesh<eqOp>
        (
            meshSource,
            meshTarget,
            mapOrder
        );
    }
}


wordList addProcessorPatches
(
    const fvMesh& meshTarget,
    const wordList& cuttingPatches
)
{
    // Add the processor patches to the cutting list
    HashTable<label> cuttingPatchTable;
    forAll(cuttingPatches, i)
    {
        cuttingPatchTable.insert(cuttingPatches[i], i);
    }

    forAll(meshTarget.boundary(), patchi)
    {
        if (isA<processorFvPatch>(meshTarget.boundary()[patchi]))
        {
            if
            (
               !cuttingPatchTable.found
                (
                    meshTarget.boundaryMesh()[patchi].name()
                )
            )
            {
                cuttingPatchTable.insert
                (
                    meshTarget.boundaryMesh()[patchi].name(),
                    -1
                );
            }
        }
    }

    return cuttingPatchTable.toc();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    argList::addNote
    (
        "Map volume fields from one mesh to another"
    );
    argList::noParallel();
    argList::addArgument("sourceCase");

    argList::addOption
    (
        "sourceTime",
        "scalar|'latestTime'",
        "Specify the source time"
    );
    argList::addOption
    (
        "sourceRegion",
        "word",
        "Specify the source region"
    );
    argList::addOption
    (
        "targetRegion",
        "word",
        "Specify the target region"
    );
    argList::addBoolOption
    (
        "parallelSource",
        "The source is decomposed"
    );
    argList::addBoolOption
    (
        "parallelTarget",
        "The target is decomposed"
    );
    argList::addBoolOption
    (
        "consistent",
        "Source and target geometry and boundary conditions identical"
    );
    argList::addOption
    (
        "mapMethod",
        "word",
        "Specify the mapping method"
    );
    argList::addBoolOption
    (
        "subtract",
        "Subtract mapped source from target"
    );
    argList::addOption
    (
        "sourceDecomposeParDict",
        "file",
        "Read decomposePar dictionary from specified location"
    );
    argList::addOption
    (
        "targetDecomposeParDict",
        "file",
        "Read decomposePar dictionary from specified location"
    );


    argList args(argc, argv);
    if (!args.check())
    {
        FatalError.exit();
    }
    #include "include/foamDlOpenLibs.H"

    fileName rootDirTarget(args.rootPath());
    fileName caseDirTarget(args.globalCaseName());

    const auto casePath = args.get<fileName>(1);
    const fileName rootDirSource = casePath.path().toAbsolute();
    const fileName caseDirSource = casePath.name();

    Info<< "Source: " << rootDirSource << ' ' << caseDirSource;
    word sourceRegion(polyMesh::defaultRegion);
    if (args.readIfPresent("sourceRegion", sourceRegion))
    {
        Info<< " (region " << sourceRegion << ')';
    }
    Info<< endl;

    Info<< "Target: " << rootDirTarget << ' ' << caseDirTarget;
    word targetRegion(polyMesh::defaultRegion);
    if (args.readIfPresent("targetRegion", targetRegion))
    {
        Info<< " (region " << targetRegion << ')';
    }
    Info<< endl;

    const bool parallelSource = args.found("parallelSource");
    const bool parallelTarget = args.found("parallelTarget");
    const bool consistent = args.found("consistent");

    meshToMesh0::order mapOrder = meshToMesh0::INTERPOLATE;
    if (args.found("mapMethod"))
    {
        const word mapMethod(args["mapMethod"]);
        if (mapMethod == "mapNearest")
        {
            mapOrder = meshToMesh0::MAP;
        }
        else if (mapMethod == "interpolate")
        {
            mapOrder = meshToMesh0::INTERPOLATE;
        }
        else if (mapMethod == "cellPointInterpolate")
        {
            mapOrder = meshToMesh0::CELL_POINT_INTERPOLATE;
        }
        else
        {
            FatalErrorInFunction
                << "Unknown mapMethod " << mapMethod << ". Valid options are: "
                << "mapNearest, interpolate and cellPointInterpolate"
                << exit(FatalError);
        }

        Info<< "Mapping method: " << mapMethod << endl;
    }

    const bool subtract = args.found("subtract");
    if (subtract)
    {
        Info<< "Subtracting mapped source field from target" << endl;
    }


    #include "createTimes.H"

    HashTable<word> patchMap;
    wordList cuttingPatches;

    if (!consistent)
    {
        IOdictionary mapFieldsDict
        (
            IOobject
            (
                "mapFieldsDict",
                runTimeTarget.system(),
                runTimeTarget,
                IOobject::MUST_READ_IF_MODIFIED,
                IOobject::NO_WRITE,
                IOobject::NO_REGISTER
            )
        );

        mapFieldsDict.readEntry("patchMap", patchMap);
        mapFieldsDict.readEntry("cuttingPatches", cuttingPatches);
    }

    if (parallelSource && !parallelTarget)
    {
        const int nProcs = readNumProcs
        (
            args,
            "sourceDecomposeParDict",
            runTimeSource
        );

        Info<< "Create target mesh\n" << endl;

        fvMesh meshTarget
        (
            IOobject
            (
                targetRegion,
                runTimeTarget.timeName(),
                runTimeTarget
            )
        );

        Info<< "Target mesh size: " << meshTarget.nCells() << endl;

        for (int proci=0; proci<nProcs; proci++)
        {
            Info<< nl << "Source processor " << proci << endl;

            Time runTimeSource
            (
                Time::controlDictName,
                rootDirSource,
                caseDirSource/("processor" + Foam::name(proci))
            );

            #include "setTimeIndex.H"

            fvMesh meshSource
            (
                IOobject
                (
                    sourceRegion,
                    runTimeSource.timeName(),
                    runTimeSource
                )
            );

            Info<< "mesh size: " << meshSource.nCells() << endl;

            if (consistent)
            {
                mapConsistentSubMesh
                (
                    meshSource,
                    meshTarget,
                    mapOrder,
                    subtract
                );
            }
            else
            {
                mapSubMesh
                (
                    meshSource,
                    meshTarget,
                    patchMap,
                    cuttingPatches,
                    mapOrder,
                    subtract
                );
            }
        }
    }
    else if (!parallelSource && parallelTarget)
    {
        const int nProcs = readNumProcs
        (
            args,
            "targetDecomposeParDict",
            runTimeTarget
        );


        Info<< "Create source mesh\n" << endl;

        #include "setTimeIndex.H"

        fvMesh meshSource
        (
            IOobject
            (
                sourceRegion,
                runTimeSource.timeName(),
                runTimeSource
            )
        );

        Info<< "Source mesh size: " << meshSource.nCells() << endl;

        for (int proci=0; proci<nProcs; proci++)
        {
            Info<< nl << "Target processor " << proci << endl;

            Time runTimeTarget
            (
                Time::controlDictName,
                rootDirTarget,
                caseDirTarget/("processor" + Foam::name(proci))
            );

            fvMesh meshTarget
            (
                IOobject
                (
                    targetRegion,
                    runTimeTarget.timeName(),
                    runTimeTarget
                )
            );

            Info<< "mesh size: " << meshTarget.nCells() << endl;

            if (consistent)
            {
                mapConsistentSubMesh
                (
                    meshSource,
                    meshTarget,
                    mapOrder,
                    subtract
                );
            }
            else
            {
                mapSubMesh
                (
                    meshSource,
                    meshTarget,
                    patchMap,
                    addProcessorPatches(meshTarget, cuttingPatches),
                    mapOrder,
                    subtract
                );
            }
        }
    }
    else if (parallelSource && parallelTarget)
    {
        const int nProcsSource = readNumProcs
        (
            args,
            "sourceDecomposeParDict",
            runTimeSource
        );
        const int nProcsTarget = readNumProcs
        (
            args,
            "targetDecomposeParDict",
            runTimeTarget
        );

        List<boundBox> bbsTarget(nProcsTarget);
        List<bool> bbsTargetSet(nProcsTarget, false);

        for (int procISource=0; procISource<nProcsSource; procISource++)
        {
            Info<< nl << "Source processor " << procISource << endl;

            Time runTimeSource
            (
                Time::controlDictName,
                rootDirSource,
                caseDirSource/("processor" + Foam::name(procISource))
            );

            #include "setTimeIndex.H"

            fvMesh meshSource
            (
                IOobject
                (
                    sourceRegion,
                    runTimeSource.timeName(),
                    runTimeSource
                )
            );

            Info<< "mesh size: " << meshSource.nCells() << endl;

            boundBox bbSource(meshSource.bounds());

            for (int procITarget=0; procITarget<nProcsTarget; procITarget++)
            {
                if
                (
                    !bbsTargetSet[procITarget]
                  || (
                      bbsTargetSet[procITarget]
                   && bbsTarget[procITarget].overlaps(bbSource)
                     )
                )
                {
                    Info<< nl << "Target processor " << procITarget << endl;

                    Time runTimeTarget
                    (
                        Time::controlDictName,
                        rootDirTarget,
                        caseDirTarget/("processor" + Foam::name(procITarget))
                    );

                    fvMesh meshTarget
                    (
                        IOobject
                        (
                            targetRegion,
                            runTimeTarget.timeName(),
                            runTimeTarget
                        )
                    );

                    Info<< "mesh size: " << meshTarget.nCells() << endl;

                    bbsTarget[procITarget] = meshTarget.bounds();
                    bbsTargetSet[procITarget] = true;

                    if (bbsTarget[procITarget].overlaps(bbSource))
                    {
                        if (consistent)
                        {
                            mapConsistentSubMesh
                            (
                                meshSource,
                                meshTarget,
                                mapOrder,
                                subtract
                            );
                        }
                        else
                        {
                            mapSubMesh
                            (
                                meshSource,
                                meshTarget,
                                patchMap,
                                addProcessorPatches(meshTarget, cuttingPatches),
                                mapOrder,
                                subtract
                            );
                        }
                    }
                }
            }
        }
    }
    else
    {
        #include "setTimeIndex.H"

        Info<< "Create meshes\n" << endl;

        fvMesh meshSource
        (
            IOobject
            (
                sourceRegion,
                runTimeSource.timeName(),
                runTimeSource
            )
        );

        fvMesh meshTarget
        (
            IOobject
            (
                targetRegion,
                runTimeTarget.timeName(),
                runTimeTarget
            )
        );

        Info<< "Source mesh size: " << meshSource.nCells() << tab
            << "Target mesh size: " << meshTarget.nCells() << nl << endl;

        if (consistent)
        {
            mapConsistentMesh(meshSource, meshTarget, mapOrder, subtract);
        }
        else
        {
            mapSubMesh
            (
                meshSource,
                meshTarget,
                patchMap,
                cuttingPatches,
                mapOrder,
                subtract
            );
        }
    }

    Info<< "\nEnd\n" << endl;

    return 0;
}


// ************************************************************************* //
