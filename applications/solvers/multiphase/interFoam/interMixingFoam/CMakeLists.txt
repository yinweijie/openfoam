set(_FILES
  incompressibleThreePhaseMixture/incompressibleThreePhaseMixture.C
  threePhaseInterfaceProperties/threePhaseInterfaceProperties.C
  immiscibleIncompressibleThreePhaseMixture/immiscibleIncompressibleThreePhaseMixture.C
  interMixingFoam.C
)
add_executable(interMixingFoam ${_FILES})
target_compile_features(interMixingFoam PUBLIC cxx_std_11)
target_link_libraries(interMixingFoam PUBLIC twoPhaseProperties waveModels)
target_include_directories(interMixingFoam PUBLIC
  .
)
