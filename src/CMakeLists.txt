# Define this directory for furture include use.
set(OPENFOAM_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
  add_subdirectory(OSspecific/MSwindows)
else()
  add_subdirectory(OSspecific/POSIX)
endif()

if (MPI_FOUND)
  add_subdirectory(Pstream/mpi)
else()
  add_subdirectory(Pstream/dummy)
endif()

add_subdirectory(OpenFOAM)

add_subdirectory(fileFormats)
add_subdirectory(surfMesh)
add_subdirectory(meshTools)

add_subdirectory(finiteArea)
add_subdirectory(finiteVolume)

add_subdirectory(mesh/blockMesh)
add_subdirectory(mesh/extrudeModel)
add_subdirectory(dynamicMesh)
add_subdirectory(genericPatchFields)

add_subdirectory(parallel/decompose/decompositionMethods)
add_subdirectory(parallel/decompose/faDecompose)
if(METIS_FOUND)
  add_subdirectory(parallel/decompose/metisDecomp)
endif()
if(SCOTCH_FOUND)
  add_subdirectory(parallel/decompose/scotchDecomp)
endif()
if(PTSCOTCH_FOUND)
  add_subdirectory(parallel/decompose/ptscotchDecomp)
endif()
if(KAHIP_FOUND)
  add_subdirectory(parallel/decompose/kahipDecomp)
endif()

add_subdirectory(lagrangian/basic)
add_subdirectory(lagrangian/distributionModels)

add_subdirectory(parallel/decompose/decompose)

add_subdirectory(parallel/reconstruct/reconstruct)
add_subdirectory(parallel/reconstruct/faReconstruct)
add_subdirectory(parallel/distributed)

add_subdirectory(dynamicFvMesh)
add_subdirectory(topoChangerFvMesh)

add_subdirectory(sampling)
add_subdirectory(transportModels/twoPhaseMixture)
add_subdirectory(transportModels/geometricVoF)
add_subdirectory(transportModels/twoPhaseProperties)
add_subdirectory(transportModels/interfaceProperties)
add_subdirectory(transportModels/immiscibleIncompressibleTwoPhaseMixture)
add_subdirectory(transportModels/incompressible)
add_subdirectory(transportModels/compressible)
add_subdirectory(randomProcesses)

add_subdirectory(ODE)

add_subdirectory(thermophysicalModels/chemistryModel)
add_subdirectory(thermophysicalModels/radiation)
add_subdirectory(thermophysicalModels/basic)
add_subdirectory(thermophysicalModels/specie)
add_subdirectory(thermophysicalModels/barotropicCompressibilityModel)
add_subdirectory(thermophysicalModels/reactionThermo)
add_subdirectory(thermophysicalModels/laminarFlameSpeed)
add_subdirectory(thermophysicalModels/SLGThermo)
add_subdirectory(thermophysicalModels/thermophysicalProperties)
add_subdirectory(thermophysicalModels/solidThermo)
add_subdirectory(thermophysicalModels/solidChemistryModel)
add_subdirectory(thermophysicalModels/solidSpecie)

add_subdirectory(TurbulenceModels/turbulenceModels)
add_subdirectory(TurbulenceModels/incompressible)
add_subdirectory(TurbulenceModels/compressible)
add_subdirectory(TurbulenceModels/schemes)

add_subdirectory(combustionModels)

add_subdirectory(overset)

add_subdirectory(fvOptions)

add_subdirectory(fvMotionSolver)

add_subdirectory(mesh/snappyHexMesh)

add_subdirectory(renumber/renumberMethods)
if(HAVE_BOOST)
  add_subdirectory(renumber/SloanRenumber)
endif()
if(HAVE_ZOLTAN)
  add_subdirectory(renumber/zoltanRenumber)
endif()

add_subdirectory(fvAgglomerationMethods/pairPatchAgglomeration)
if(HAVE_MGRIDGEN)
  add_subdirectory(fvAgglomerationMethods/MGridGenGamgAgglomeration)
endif()

add_subdirectory(waveModels)

add_subdirectory(engine)

add_subdirectory(conversion)
if(HAVE_LIBCCMIO)
  add_subdirectory(conversion/ccm)
endif()

add_subdirectory(phaseSystemModels/reactingEuler/saturationModels)
add_subdirectory(phaseSystemModels/reactingEuler/multiphaseSystem)
add_subdirectory(phaseSystemModels/reactingEuler/twoPhaseSystem)
add_subdirectory(phaseSystemModels/reactingEuler/twoPhaseCompressibleTurbulenceModels)
add_subdirectory(phaseSystemModels/multiphaseInter/compressibleMultiPhaseTurbulenceModels)
add_subdirectory(phaseSystemModels/multiphaseInter/phasesSystem)
add_subdirectory(phaseSystemModels/multiphaseEuler/multiphaseSystem)
add_subdirectory(phaseSystemModels/twoPhaseInter/incompressibleInterPhaseTransportModel)
add_subdirectory(phaseSystemModels/twoPhaseInter/VoFphaseIncompressibleTurbulenceModels)
add_subdirectory(phaseSystemModels/twoPhaseEuler/twoPhaseSystem)
add_subdirectory(phaseSystemModels/twoPhaseEuler/phaseCompressibleTurbulenceModels)

add_subdirectory(functionObjects/field)
add_subdirectory(functionObjects/forces)
add_subdirectory(functionObjects/initialisation)
add_subdirectory(functionObjects/utilities)
add_subdirectory(functionObjects/solvers)
add_subdirectory(functionObjects/phaseSystems)
add_subdirectory(functionObjects/randomProcesses)

add_subdirectory(thermoTools)

add_subdirectory(regionModels/regionModel)
add_subdirectory(regionModels/pyrolysisModels)
add_subdirectory(regionModels/surfaceFilmModels)
add_subdirectory(regionModels/surfaceFilmModels/derivedFvPatchFields/wallFunctions)
add_subdirectory(regionModels/thermalBaffleModels)
add_subdirectory(regionModels/regionCoupling)

add_subdirectory(faOptions)

add_subdirectory(regionFaModels)

add_subdirectory(lagrangian/solidParticle)
add_subdirectory(lagrangian/intermediate)
add_subdirectory(lagrangian/turbulence)
add_subdirectory(lagrangian/spray)
add_subdirectory(lagrangian/DSMC)
add_subdirectory(lagrangian/coalCombustion)

add_subdirectory(lagrangian/molecularDynamics/molecularMeasurements)
add_subdirectory(lagrangian/molecularDynamics/potential)
add_subdirectory(lagrangian/molecularDynamics/molecule)

add_subdirectory(functionObjects/lagrangian)

add_subdirectory(lumpedPointMotion)
add_subdirectory(rigidBodyDynamics)
add_subdirectory(sixDoFRigidBodyState)
add_subdirectory(rigidBodyMeshMotion)
add_subdirectory(sixDoFRigidBodyMotion)
add_subdirectory(atmosphericModels)
add_subdirectory(optimisation/adjointOptimisation/adjoint)

add_subdirectory(dynamicFaMesh/interfaceTrackingFvMesh)
