set(_FILES
  Test-SphericalTensor.C
)
add_executable(Test-SphericalTensor ${_FILES})
target_compile_features(Test-SphericalTensor PUBLIC cxx_std_11)
target_include_directories(Test-SphericalTensor PUBLIC
  .
)
