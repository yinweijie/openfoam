set(_FILES
  pendulum.C
)
add_executable(Test-pendulum ${_FILES})
target_compile_features(Test-pendulum PUBLIC cxx_std_11)
target_include_directories(Test-pendulum PUBLIC
  .
)
