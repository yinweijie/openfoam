set(_FILES
  Test-quaternion.C
)
add_executable(Test-quaternion ${_FILES})
target_compile_features(Test-quaternion PUBLIC cxx_std_11)
target_include_directories(Test-quaternion PUBLIC
  .
)
