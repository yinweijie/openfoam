set(_FILES
  Test-string.C
)
add_executable(Test-string ${_FILES})
target_compile_features(Test-string PUBLIC cxx_std_11)
target_include_directories(Test-string PUBLIC
  .
)
