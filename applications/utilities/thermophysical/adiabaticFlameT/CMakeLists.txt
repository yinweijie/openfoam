set(_FILES
  adiabaticFlameT.C
)
add_executable(adiabaticFlameT ${_FILES})
target_compile_features(adiabaticFlameT PUBLIC cxx_std_11)
target_link_libraries(adiabaticFlameT PUBLIC specie)
target_include_directories(adiabaticFlameT PUBLIC
  .
)
