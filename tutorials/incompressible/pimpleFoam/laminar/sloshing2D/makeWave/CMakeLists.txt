set(_FILES
  makeWave.C
)
add_executable(makeWave ${_FILES})
target_compile_features(makeWave PUBLIC cxx_std_11)
target_link_libraries(makeWave PUBLIC interfaceTrackingFvMesh)
target_include_directories(makeWave PUBLIC
  .
)
