set(_FILES
  passivePositionParticleCloud.C
  parLagrangianDistributor.C
  parLagrangianDistributorFields.C
  parPointFieldDistributor.C
  parFaFieldDistributorCache.C
  parFvFieldDistributor.C
  parFvFieldDistributorFields.C
  loadOrCreateMesh.C
  redistributePar.C
)
add_executable(redistributePar ${_FILES})
target_compile_features(redistributePar PUBLIC cxx_std_11)
target_link_libraries(redistributePar PUBLIC decompose genericPatchFields metisDecomp ptscotchDecomp snappyHexMeshLib)
if (KAHIP_FOUND)
  target_link_libraries(redistributePar PUBLIC kahipDecomp)
endif()
if (SCOTCH_FOUND)
  target_link_libraries(redistributePar PUBLIC scotchDecomp)
endif()
target_include_directories(redistributePar PUBLIC
  .
)
