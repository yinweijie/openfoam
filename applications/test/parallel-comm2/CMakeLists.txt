set(_FILES
  Test-parallel-comm2.C
)
add_executable(Test-parallel-comm2 ${_FILES})
target_compile_features(Test-parallel-comm2 PUBLIC cxx_std_11)
target_include_directories(Test-parallel-comm2 PUBLIC
  .
)
