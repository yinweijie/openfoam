set(_FILES
  Test-ICharStream1.C
)
add_executable(Test-ICharStream1 ${_FILES})
target_compile_features(Test-ICharStream1 PUBLIC cxx_std_11)
target_include_directories(Test-ICharStream1 PUBLIC
  .
)
