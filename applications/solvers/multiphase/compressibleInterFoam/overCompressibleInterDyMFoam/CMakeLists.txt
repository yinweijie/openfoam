set(_FILES
  overCompressibleInterDyMFoam.C
)
add_executable(overCompressibleInterDyMFoam ${_FILES})
target_compile_features(overCompressibleInterDyMFoam PUBLIC cxx_std_11)
target_link_libraries(overCompressibleInterDyMFoam PUBLIC VoFphaseCompressibleTurbulenceModels topoChangerFvMesh twoPhaseSurfaceTension waveModels)
target_include_directories(overCompressibleInterDyMFoam PUBLIC
  .
)
