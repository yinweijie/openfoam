set(_FILES
  Test-FieldFields2.C
)
add_executable(Test-FieldFields2 ${_FILES})
target_compile_features(Test-FieldFields2 PUBLIC cxx_std_11)
target_include_directories(Test-FieldFields2 PUBLIC
  .
)
