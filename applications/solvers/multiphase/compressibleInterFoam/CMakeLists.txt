set(_FILES
  compressibleInterFoam.C
)
add_executable(compressibleInterFoam ${_FILES})
target_compile_features(compressibleInterFoam PUBLIC cxx_std_11)
target_link_libraries(compressibleInterFoam PUBLIC VoFphaseCompressibleTurbulenceModels fvOptions twoPhaseSurfaceTension)
target_include_directories(compressibleInterFoam PUBLIC
  .
)
