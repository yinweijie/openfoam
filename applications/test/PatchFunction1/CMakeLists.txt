set(_FILES
  Test-PatchFunction1.C
)
add_executable(Test-PatchFunction1 ${_FILES})
target_compile_features(Test-PatchFunction1 PUBLIC cxx_std_11)
target_include_directories(Test-PatchFunction1 PUBLIC
  .
)
