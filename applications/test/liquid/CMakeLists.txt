set(_FILES
  Test-liquid.C
)
add_executable(Test-liquid ${_FILES})
target_compile_features(Test-liquid PUBLIC cxx_std_11)
target_include_directories(Test-liquid PUBLIC
  .
)
