set(_FILES
  icoFoam.C
)
add_executable(icoFoam ${_FILES})
target_compile_features(icoFoam PUBLIC cxx_std_11)
target_link_libraries(icoFoam PUBLIC finiteVolume meshTools)
target_include_directories(icoFoam PUBLIC
  .
  ${PROJECT_SOURCE_DIR}/src/OpenFOAM/include
)
