/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2016-2017 Wikki Ltd
    Copyright (C) 2020-2023 OpenCFD Ltd.
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

#include "faMesh/faMesh.H"
#include "faMesh/faMeshBoundaryHalo.H"
#include "faMesh/faGlobalMeshData/faGlobalMeshData.H"
#include "db/Time/TimeOpenFOAM.H"
#include "meshes/polyMesh/polyMesh.H"
#include "meshes/primitiveMesh/primitiveMesh.H"
#include "include/demandDrivenData.H"
#include "containers/IndirectLists/IndirectList/IndirectList.H"
#include "fields/areaFields/areaFields.H"
#include "fields/edgeFields/edgeFields.H"
#include "faMesh/faMeshLduAddressing.H"
#include "faMesh/faPatches/constraint/processor/processorFaPatch.H"
#include "faMesh/faPatches/constraint/wedge/wedgeFaPatch.H"
#include "faMesh/faPatches/faPatch/faPatchData.H"
#include "global/debug/registerSwitch.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(faMesh, 0);

    int faMesh::geometryOrder_
    (
        debug::optimisationSwitch("fa:geometryOrder", 2)
    );
    registerOptSwitch
    (
        "fa:geometryOrder",
        int,
        faMesh::geometryOrder_
    );
}


const Foam::word Foam::faMesh::prefix("finite-area");

Foam::word Foam::faMesh::meshSubDir = "faMesh";

const int Foam::faMesh::quadricsFit_ = 0;  // Tuning (experimental)


// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

const Foam::objectRegistry* Foam::faMesh::registry(const polyMesh& pMesh)
{
    // This will change in the near future
    return &static_cast<const objectRegistry&>(pMesh);
}


const Foam::faMesh& Foam::faMesh::mesh
(
    const polyMesh& pMesh
)
{
    // This will change in the near future
    return pMesh.lookupObject<faMesh>("faMesh");
}


// * * * * * * * * * * * * * * * Local Functions * * * * * * * * * * * * * * //

namespace Foam
{

// Convert patch names to face labels. Preserve patch order
static labelList selectPatchFaces
(
    const polyBoundaryMesh& pbm,
    const wordRes& polyPatchNames
)
{
    const labelList patchIDs
    (
        pbm.indices(polyPatchNames, true)  // useGroups
    );

    if (patchIDs.empty())
    {
        FatalErrorInFunction
            << "No matching patches: " << polyPatchNames << nl
            << exit(FatalError);
    }

    label nFaceLabels = 0;
    for (const label patchi : patchIDs)
    {
        nFaceLabels += pbm[patchi].size();
    }

    labelList faceLabels(nFaceLabels);

    nFaceLabels = 0;
    for (const label patchi : patchIDs)
    {
        for (const label facei : pbm[patchi].range())
        {
            faceLabels[nFaceLabels] = facei;
            ++nFaceLabels;
        }
    }

    return faceLabels;
}

} // End namespace Foam


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::faMesh::checkBoundaryEdgeLabelRange
(
    const labelUList& edgeLabels
) const
{
    label nErrors = 0;

    for (const label edgei : edgeLabels)
    {
        if (edgei < nInternalEdges_ || edgei >= nEdges_)
        {
            if (!nErrors++)
            {
                FatalErrorInFunction
                    << "Boundary edge label out of range "
                    << nInternalEdges_ << ".." << (nEdges_-1) << nl
                    << "   ";
            }

            FatalError<< ' ' << edgei;
        }
    }

    if (nErrors)
    {
        FatalError << nl << exit(FatalError);
    }
}


void Foam::faMesh::initPatch() const
{
    patchPtr_.reset
    (
        new uindirectPrimitivePatch
        (
            UIndirectList<face>(mesh().faces(), faceLabels_),
            mesh().points()
        )
    );
    // Could set some basic primitive data here...
    // nEdges_ = patchPtr_->nEdges();
    // nInternalEdges_ = patchPtr_->nInternalEdges();
    // nFaces_ = patchPtr_->size();
    // nPoints_ = patchPtr_->nPoints();
    bndConnectPtr_.reset(nullptr);
    haloMapPtr_.reset(nullptr);
    haloFaceCentresPtr_.reset(nullptr);
    haloFaceNormalsPtr_.reset(nullptr);
}


void Foam::faMesh::setPrimitiveMeshData()
{
    DebugInFunction << "Setting primitive data" << endl;

    const uindirectPrimitivePatch& bp = patch();
    const labelListList& edgeFaces = bp.edgeFaces();

    // Dimensions

    nEdges_ = bp.nEdges();
    nInternalEdges_ = bp.nInternalEdges();
    nFaces_ = bp.size();
    nPoints_ = bp.nPoints();

    edges_.resize(nEdges_);
    edgeOwner_.resize(nEdges_);
    edgeNeighbour_.resize(nInternalEdges_);

    // Internal edges
    for (label edgei = 0; edgei < nInternalEdges_; ++edgei)
    {
        edges_[edgei] = bp.edges()[edgei];

        edgeOwner_[edgei] = edgeFaces[edgei][0];

        edgeNeighbour_[edgei] = edgeFaces[edgei][1];
    }

    // Continue with boundary edges
    label edgei = nInternalEdges_;

    for (const faPatch& p : boundary())
    {
        for (const label patchEdgei : p.edgeLabels())
        {
            edges_[edgei] = bp.edges()[patchEdgei];

            edgeOwner_[edgei] = edgeFaces[patchEdgei][0];

            ++edgei;
        }
    }
}


void Foam::faMesh::clearHalo() const
{
    DebugInFunction << "Clearing halo information" << endl;

    haloMapPtr_.reset(nullptr);
    haloFaceCentresPtr_.reset(nullptr);
    haloFaceNormalsPtr_.reset(nullptr);
}


void Foam::faMesh::clearGeomNotAreas() const
{
    DebugInFunction << "Clearing geometry" << endl;

    clearHalo();
    patchPtr_.reset(nullptr);
    polyPatchFacesPtr_.reset(nullptr);
    polyPatchIdsPtr_.reset(nullptr);
    bndConnectPtr_.reset(nullptr);
    deleteDemandDrivenData(SPtr_);
    deleteDemandDrivenData(patchStartsPtr_);
    deleteDemandDrivenData(LePtr_);
    deleteDemandDrivenData(magLePtr_);
    deleteDemandDrivenData(faceCentresPtr_);
    deleteDemandDrivenData(edgeCentresPtr_);
    deleteDemandDrivenData(faceAreaNormalsPtr_);
    deleteDemandDrivenData(edgeAreaNormalsPtr_);
    pointAreaNormalsPtr_.reset(nullptr);
    deleteDemandDrivenData(faceCurvaturesPtr_);
    deleteDemandDrivenData(edgeTransformTensorsPtr_);
}


void Foam::faMesh::clearGeom() const
{
    DebugInFunction << "Clearing geometry" << endl;

    clearGeomNotAreas();
    deleteDemandDrivenData(S0Ptr_);
    deleteDemandDrivenData(S00Ptr_);
    deleteDemandDrivenData(correctPatchPointNormalsPtr_);
}


void Foam::faMesh::clearAddressing() const
{
    DebugInFunction << "Clearing addressing" << endl;

    deleteDemandDrivenData(lduPtr_);
}


void Foam::faMesh::clearOut() const
{
    clearGeom();
    clearAddressing();
    globalMeshDataPtr_.reset(nullptr);
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

void Foam::faMesh::syncGeom()
{
    if (UPstream::parRun())
    {
        // areaCentres()
        if (faceCentresPtr_)
        {
            faceCentresPtr_->boundaryFieldRef()
                .evaluateCoupled<processorFaPatch>();
        }

        // faceAreaNormals()
        if (faceAreaNormalsPtr_)
        {
            faceAreaNormalsPtr_->boundaryFieldRef()
                .evaluateCoupled<processorFaPatch>();
        }
    }
}


bool Foam::faMesh::init(const bool doInit)
{
    if (doInit)
    {
        setPrimitiveMeshData();
    }

    // Create global mesh data
    if (UPstream::parRun())
    {
        (void)globalData();
    }

    // Calculate topology for the patches (processor-processor comms etc.)
    boundary_.updateMesh();

    // Calculate the geometry for the patches (transformation tensors etc.)
    boundary_.calcGeometry();

    syncGeom();

    return false;
}


Foam::faMesh::faMesh(const polyMesh& pMesh, const Foam::zero)
:
    faMesh(pMesh, labelList(), static_cast<IOobjectOption>(pMesh))
{}


Foam::faMesh::faMesh(const faMesh& baseMesh, const Foam::zero)
:
    faMesh
    (
        baseMesh,
        labelList(),
        IOobjectOption(IOobjectOption::NO_READ, IOobjectOption::NO_WRITE)
    )
{}


Foam::faMesh::faMesh
(
    const polyMesh& pMesh,
    const bool doInit
)
:
    MeshObject<polyMesh, Foam::UpdateableMeshObject, faMesh>(pMesh),
    faSchemes(mesh()),
    edgeInterpolation(*this),
    faSolution(mesh()),
    faceLabels_
    (
        IOobject
        (
            "faceLabels",
            time().findInstance(meshDir(), "faceLabels"),
            faMesh::meshSubDir,
            faMesh::thisDb(),
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    ),
    boundary_
    (
        IOobject
        (
            "faBoundary",
            // Allow boundary file that is newer than faceLabels
            time().findInstance
            (
                meshDir(),
                "faBoundary",
                IOobject::MUST_READ,
                faceLabels_.instance()
            ),
            faMesh::meshSubDir,
            faMesh::thisDb(),
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        ),
        *this
    ),
    comm_(UPstream::worldComm),
    curTimeIndex_(time().timeIndex()),

    patchPtr_(nullptr),
    polyPatchFacesPtr_(nullptr),
    polyPatchIdsPtr_(nullptr),
    bndConnectPtr_(nullptr),
    lduPtr_(nullptr),

    SPtr_(nullptr),
    S0Ptr_(nullptr),
    S00Ptr_(nullptr),
    patchStartsPtr_(nullptr),
    LePtr_(nullptr),
    magLePtr_(nullptr),
    faceCentresPtr_(nullptr),
    edgeCentresPtr_(nullptr),
    faceAreaNormalsPtr_(nullptr),
    edgeAreaNormalsPtr_(nullptr),
    pointAreaNormalsPtr_(nullptr),
    faceCurvaturesPtr_(nullptr),
    edgeTransformTensorsPtr_(nullptr),
    correctPatchPointNormalsPtr_(nullptr),
    globalMeshDataPtr_(nullptr),

    haloMapPtr_(nullptr),
    haloFaceCentresPtr_(nullptr),
    haloFaceNormalsPtr_(nullptr)
{
    DebugInFunction << "Creating from IOobject" << endl;

    setPrimitiveMeshData();

    if (doInit)
    {
        faMesh::init(false);  // do not init lower levels
    }

    if (doInit)
    {
        // Read some optional fields
        // - logic as per fvMesh

        IOobject rio
        (
            "name",
            time().timeName(),
            faMesh::meshSubDir,
            faMesh::thisDb(),
            IOobject::LAZY_READ,
            IOobject::NO_WRITE,
            IOobject::NO_REGISTER
        );

        // Read old surface areas (if present)
        rio.resetHeader("S0");
        if (returnReduceOr(rio.typeHeaderOk<regIOobject>(false)))
        {
            S0Ptr_ = new DimensionedField<scalar, areaMesh>
            (
                rio,
                *this,
                dimensionedScalar(dimArea, Zero)
            );
        }
    }
}


Foam::faMesh::faMesh
(
    const polyMesh& pMesh,
    labelList&& faceLabels
)
:
    faMesh
    (
        pMesh,
        std::move(faceLabels),
        static_cast<IOobjectOption>(pMesh)
    )
{}


Foam::faMesh::faMesh
(
    const polyMesh& pMesh,
    labelList&& faceLabels,
    IOobjectOption ioOpt
)
:
    MeshObject<polyMesh, Foam::UpdateableMeshObject, faMesh>(pMesh),
    faSchemes(mesh(), ioOpt.readOpt()),
    edgeInterpolation(*this),
    faSolution(mesh(), ioOpt.readOpt()),
    faceLabels_
    (
        IOobject
        (
            "faceLabels",
            mesh().facesInstance(),
            faMesh::meshSubDir,
            faMesh::thisDb(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        std::move(faceLabels)
    ),
    boundary_
    (
        IOobject
        (
            "faBoundary",
            mesh().facesInstance(),
            faMesh::meshSubDir,
            faMesh::thisDb(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        *this,
        label(0)
    ),
    comm_(UPstream::worldComm),
    curTimeIndex_(time().timeIndex()),

    patchPtr_(nullptr),
    polyPatchFacesPtr_(nullptr),
    polyPatchIdsPtr_(nullptr),
    bndConnectPtr_(nullptr),
    lduPtr_(nullptr),

    SPtr_(nullptr),
    S0Ptr_(nullptr),
    S00Ptr_(nullptr),
    patchStartsPtr_(nullptr),
    LePtr_(nullptr),
    magLePtr_(nullptr),
    faceCentresPtr_(nullptr),
    edgeCentresPtr_(nullptr),
    faceAreaNormalsPtr_(nullptr),
    edgeAreaNormalsPtr_(nullptr),
    pointAreaNormalsPtr_(nullptr),
    faceCurvaturesPtr_(nullptr),
    edgeTransformTensorsPtr_(nullptr),
    correctPatchPointNormalsPtr_(nullptr),
    globalMeshDataPtr_(nullptr),

    haloMapPtr_(nullptr),
    haloFaceCentresPtr_(nullptr),
    haloFaceNormalsPtr_(nullptr)
{
    // Not yet much for primitive mesh data possible...
    nPoints_ = 0;
    nEdges_ = 0;
    nInternalEdges_ = 0;
    nFaces_ = faceLabels_.size();
}


Foam::faMesh::faMesh
(
    const faMesh& baseMesh,
    labelList&& faceLabels,
    IOobjectOption ioOpt
)
:
    MeshObject<polyMesh, Foam::UpdateableMeshObject, faMesh>(baseMesh.mesh()),
    faSchemes
    (
        faMesh::thisDb(),
        ioOpt.readOpt(),
        static_cast<const dictionary*>(baseMesh.hasSchemes())
    ),
    edgeInterpolation(*this),
    faSolution
    (
        faMesh::thisDb(),
        ioOpt.readOpt(),
        static_cast<const dictionary*>(baseMesh.hasSolution())
    ),
    faceLabels_
    (
        IOobject
        (
            "faceLabels",
            mesh().facesInstance(),
            faMesh::meshSubDir,
            faMesh::thisDb(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        std::move(faceLabels)
    ),
    boundary_
    (
        IOobject
        (
            "faBoundary",
            mesh().facesInstance(),
            faMesh::meshSubDir,
            faMesh::thisDb(),
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        *this,
        label(0)
    ),
    comm_(UPstream::worldComm),
    curTimeIndex_(time().timeIndex()),

    patchPtr_(nullptr),
    polyPatchFacesPtr_(nullptr),
    polyPatchIdsPtr_(nullptr),
    bndConnectPtr_(nullptr),
    lduPtr_(nullptr),

    SPtr_(nullptr),
    S0Ptr_(nullptr),
    S00Ptr_(nullptr),
    patchStartsPtr_(nullptr),
    LePtr_(nullptr),
    magLePtr_(nullptr),
    faceCentresPtr_(nullptr),
    edgeCentresPtr_(nullptr),
    faceAreaNormalsPtr_(nullptr),
    edgeAreaNormalsPtr_(nullptr),
    pointAreaNormalsPtr_(nullptr),
    faceCurvaturesPtr_(nullptr),
    edgeTransformTensorsPtr_(nullptr),
    correctPatchPointNormalsPtr_(nullptr),
    globalMeshDataPtr_(nullptr),

    haloMapPtr_(nullptr),
    haloFaceCentresPtr_(nullptr),
    haloFaceNormalsPtr_(nullptr)
{
    // Not yet much for primitive mesh data possible...
    nPoints_ = 0;
    nEdges_ = 0;
    nInternalEdges_ = 0;
    nFaces_ = faceLabels_.size();
}


Foam::faMesh::faMesh(const polyPatch& pp, const bool doInit)
:
    faMesh
    (
        pp.boundaryMesh().mesh(),
        identity(pp.range())
    )
{
    DebugInFunction << "Creating from polyPatch:" << pp.name() << endl;

    // Add single faPatch "default", but with processor connections
    faPatchList newPatches
    (
        createOnePatch("default")
    );

    addFaPatches(newPatches);

    setPrimitiveMeshData();

    if (doInit)
    {
        faMesh::init(false);  // do not init lower levels
    }
}


Foam::faMesh::faMesh
(
    const polyMesh& pMesh,
    const dictionary& faMeshDefinition,
    const bool doInit
)
:
    faMesh
    (
        pMesh,
        selectPatchFaces
        (
            pMesh.boundaryMesh(),
            faMeshDefinition.get<wordRes>("polyMeshPatches")
        )
    )
{
    DebugInFunction << "Creating from definition (dictionary)" << endl;

    faPatchList newPatches
    (
        createPatchList
        (
            faMeshDefinition.subDict("boundary"),

            // Optional 'empty' patch
            faMeshDefinition.getOrDefault<word>("emptyPatch", word::null),

            // Optional specification for default patch
            faMeshDefinition.findDict("defaultPatch")
        )
    );

    addFaPatches(newPatches);

    if (doInit)
    {
        faMesh::init(false);  // do not init lower levels
    }

    if (doInit)
    {
        // Read old surface areas (if present)
        // - logic as per fvMesh

        IOobject rio
        (
            "name",
            time().timeName(),
            faMesh::meshSubDir,
            faMesh::thisDb(),
            IOobject::LAZY_READ,
            IOobject::NO_WRITE,
            IOobject::NO_REGISTER
        );

        // Read old surface areas (if present)
        rio.resetHeader("S0");
        if (returnReduceOr(rio.typeHeaderOk<regIOobject>(false)))
        {
            S0Ptr_ = new DimensionedField<scalar, areaMesh>
            (
                rio,
                *this,
                dimensionedScalar(dimArea, Zero)
            );
        }
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::faMesh::~faMesh()
{
    clearOut();
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const Foam::faSchemes* Foam::faMesh::hasSchemes() const
{
    return static_cast<const faSchemes*>(this);
}


const Foam::faSolution* Foam::faMesh::hasSolution() const
{
    return static_cast<const faSolution*>(this);
}


const Foam::faSchemes& Foam::faMesh::schemes() const
{
    return static_cast<const faSchemes&>(*this);
}


Foam::faSchemes& Foam::faMesh::schemes()
{
    return static_cast<faSchemes&>(*this);
}


const Foam::faSolution& Foam::faMesh::solution() const
{
    return static_cast<const faSolution&>(*this);
}


Foam::faSolution& Foam::faMesh::solution()
{
    return static_cast<faSolution&>(*this);
}


Foam::fileName Foam::faMesh::meshDir() const
{
    return mesh().dbDir()/faMesh::meshSubDir;
}


const Foam::Time& Foam::faMesh::time() const
{
    return mesh().time();
}


const Foam::fileName& Foam::faMesh::pointsInstance() const
{
    return mesh().pointsInstance();
}


const Foam::fileName& Foam::faMesh::facesInstance() const
{
    return mesh().facesInstance();
}


bool Foam::faMesh::hasDb() const
{
    return true;
}


const Foam::objectRegistry& Foam::faMesh::thisDb() const
{
    return mesh().thisDb();
}


const Foam::word& Foam::faMesh::regionName() const
{
    return polyMesh::regionName(thisDb().name());
}


Foam::labelList Foam::faMesh::faceCells() const
{
    const labelList& faceOwner = this->mesh().faceOwner();

    labelList list(faceLabels_);

    for (label& val : list)
    {
        // Transcribe from faceId to cellId (owner)
        val = faceOwner[val];
    }

    return list;
}


void Foam::faMesh::removeFiles(const fileName& instanceDir) const
{
    fileName meshFilesPath = thisDb().time().path()/instanceDir/meshDir();

    Foam::rm(meshFilesPath/"faceLabels");
    Foam::rm(meshFilesPath/"faBoundary");
}


void Foam::faMesh::removeFiles() const
{
    removeFiles(thisDb().instance());
}


const Foam::labelList& Foam::faMesh::patchStarts() const
{
    if (!patchStartsPtr_)
    {
        calcPatchStarts();
    }

    return *patchStartsPtr_;
}


const Foam::edgeVectorField& Foam::faMesh::Le() const
{
    if (!LePtr_)
    {
        calcLe();
    }

    return *LePtr_;
}


const Foam::edgeScalarField& Foam::faMesh::magLe() const
{
    if (!magLePtr_)
    {
        calcMagLe();
    }

    return *magLePtr_;
}


const Foam::areaVectorField& Foam::faMesh::areaCentres() const
{
    if (!faceCentresPtr_)
    {
        calcFaceCentres();
    }

    return *faceCentresPtr_;
}


const Foam::edgeVectorField& Foam::faMesh::edgeCentres() const
{
    if (!edgeCentresPtr_)
    {
        calcEdgeCentres();
    }

    return *edgeCentresPtr_;
}


const Foam::DimensionedField<Foam::scalar, Foam::areaMesh>&
Foam::faMesh::S() const
{
    if (!SPtr_)
    {
        calcS();
    }

    return *SPtr_;
}


const Foam::DimensionedField<Foam::scalar, Foam::areaMesh>&
Foam::faMesh::S0() const
{
    if (!S0Ptr_)
    {
        FatalErrorInFunction
            << "S0 is not available"
            << abort(FatalError);
    }

    return *S0Ptr_;
}


const Foam::DimensionedField<Foam::scalar, Foam::areaMesh>&
Foam::faMesh::S00() const
{
    if (!S00Ptr_)
    {
        S00Ptr_ = new DimensionedField<scalar, areaMesh>
        (
            IOobject
            (
                "S00",
                time().timeName(),
                faMesh::thisDb(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            S0()
        );

        S0Ptr_->writeOpt(IOobject::AUTO_WRITE);
    }

    return *S00Ptr_;
}


const Foam::areaVectorField& Foam::faMesh::faceAreaNormals() const
{
    if (!faceAreaNormalsPtr_)
    {
        calcFaceAreaNormals();
    }

    return *faceAreaNormalsPtr_;
}


const Foam::edgeVectorField& Foam::faMesh::edgeAreaNormals() const
{
    if (!edgeAreaNormalsPtr_)
    {
        calcEdgeAreaNormals();
    }

    return *edgeAreaNormalsPtr_;
}


const Foam::vectorField& Foam::faMesh::pointAreaNormals() const
{
    if (!pointAreaNormalsPtr_)
    {
        pointAreaNormalsPtr_.reset(new vectorField(nPoints()));

        calcPointAreaNormals(*pointAreaNormalsPtr_);

        if (quadricsFit_ > 0)
        {
            calcPointAreaNormalsByQuadricsFit(*pointAreaNormalsPtr_);
        }
    }

    return *pointAreaNormalsPtr_;
}


const Foam::areaScalarField& Foam::faMesh::faceCurvatures() const
{
    if (!faceCurvaturesPtr_)
    {
        calcFaceCurvatures();
    }

    return *faceCurvaturesPtr_;
}


const Foam::FieldField<Foam::Field, Foam::tensor>&
Foam::faMesh::edgeTransformTensors() const
{
    if (!edgeTransformTensorsPtr_)
    {
        calcEdgeTransformTensors();
    }

    return *edgeTransformTensorsPtr_;
}


bool Foam::faMesh::hasGlobalData() const noexcept
{
    return bool(globalMeshDataPtr_);
}


const Foam::faGlobalMeshData& Foam::faMesh::globalData() const
{
    if (!globalMeshDataPtr_)
    {
        globalMeshDataPtr_.reset(new faGlobalMeshData(*this));
    }

    return *globalMeshDataPtr_;
}


const Foam::lduAddressing& Foam::faMesh::lduAddr() const
{
    if (!lduPtr_)
    {
        calcLduAddressing();
    }

    return *lduPtr_;
}


bool Foam::faMesh::movePoints()
{
    // Grab point motion from polyMesh
    const vectorField& newPoints = mesh().points();

    // Grab old time areas if the time has been incremented
    if (curTimeIndex_ < time().timeIndex())
    {
        if (S00Ptr_ && S0Ptr_)
        {
            DebugInfo<< "Copy old-old S" << endl;
            *S00Ptr_ = *S0Ptr_;
        }

        if (S0Ptr_)
        {
            DebugInfo<< "Copy old S" << endl;
            *S0Ptr_ = S();
        }
        else
        {
            DebugInfo<< "Creating old cell volumes." << endl;

            S0Ptr_ = new DimensionedField<scalar, areaMesh>
            (
                IOobject
                (
                    "S0",
                    time().timeName(),
                    faMesh::thisDb(),
                    IOobject::NO_READ,
                    IOobject::NO_WRITE,
                    IOobject::NO_REGISTER
                ),
                S()
            );
        }

        curTimeIndex_ = time().timeIndex();
    }

    clearGeomNotAreas();

    if (patchPtr_)
    {
        patchPtr_->movePoints(newPoints);
    }

    // Move boundary points
    boundary_.movePoints(newPoints);

    // Move interpolation
    edgeInterpolation::movePoints();

    // Note: Fluxes were dummy?

    syncGeom();

    return true;
}


bool Foam::faMesh::correctPatchPointNormals(const label patchID) const
{
    if
    (
        bool(correctPatchPointNormalsPtr_)
     && patchID >= 0 && patchID < boundary().size()
    )
    {
        return (*correctPatchPointNormalsPtr_)[patchID];
    }

    return false;
}


Foam::boolList& Foam::faMesh::correctPatchPointNormals() const
{
    if (!correctPatchPointNormalsPtr_)
    {
        correctPatchPointNormalsPtr_ = new boolList(boundary().size(), false);
    }

    return *correctPatchPointNormalsPtr_;
}


bool Foam::faMesh::write(const bool writeOnProc) const
{
    faceLabels_.write();
    boundary_.write();

    return false;
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

bool Foam::faMesh::operator!=(const faMesh& m) const
{
    return &m != this;
}


bool Foam::faMesh::operator==(const faMesh& m) const
{
    return &m == this;
}


// ************************************************************************* //
