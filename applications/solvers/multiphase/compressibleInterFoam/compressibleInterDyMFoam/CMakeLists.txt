set(_FILES
  compressibleInterDyMFoam.C
)
add_executable(compressibleInterDyMFoam ${_FILES})
target_compile_features(compressibleInterDyMFoam PUBLIC cxx_std_11)
target_link_libraries(compressibleInterDyMFoam PUBLIC VoFphaseCompressibleTurbulenceModels dynamicFvMesh fvOptions twoPhaseSurfaceTension)
target_include_directories(compressibleInterDyMFoam PUBLIC
  .
)
