set(_FILES
  Test-fileHandler-writing.C
)
add_executable(Test-fileHandler-writing ${_FILES})
target_compile_features(Test-fileHandler-writing PUBLIC cxx_std_11)
target_include_directories(Test-fileHandler-writing PUBLIC
  .
)
