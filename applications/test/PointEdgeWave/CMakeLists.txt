set(_FILES
  Test-PointEdgeWave.C
)
add_executable(Test-PointEdgeWave ${_FILES})
target_compile_features(Test-PointEdgeWave PUBLIC cxx_std_11)
target_include_directories(Test-PointEdgeWave PUBLIC
  .
)
