set(_FILES
  Test-fieldMapping.C
)
add_executable(Test-fieldMapping ${_FILES})
target_compile_features(Test-fieldMapping PUBLIC cxx_std_11)
target_include_directories(Test-fieldMapping PUBLIC
  .
)
