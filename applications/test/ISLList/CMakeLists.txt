set(_FILES
  Test-ISLList.C
)
add_executable(Test-ISLList ${_FILES})
target_compile_features(Test-ISLList PUBLIC cxx_std_11)
target_include_directories(Test-ISLList PUBLIC
  .
)
