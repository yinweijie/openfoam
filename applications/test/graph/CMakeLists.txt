set(_FILES
  Test-graph.C
)
add_executable(Test-graph ${_FILES})
target_compile_features(Test-graph PUBLIC cxx_std_11)
target_include_directories(Test-graph PUBLIC
  .
)
