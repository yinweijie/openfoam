set(_FILES
  Test-QRMatrix.C
)
add_executable(Test-QRMatrix ${_FILES})
target_compile_features(Test-QRMatrix PUBLIC cxx_std_11)
target_include_directories(Test-QRMatrix PUBLIC
  .
)
