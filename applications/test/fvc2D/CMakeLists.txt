set(_FILES
  Test-fvc2D.C
)
add_executable(Test-fvc2D ${_FILES})
target_compile_features(Test-fvc2D PUBLIC cxx_std_11)
target_include_directories(Test-fvc2D PUBLIC
  .
)
