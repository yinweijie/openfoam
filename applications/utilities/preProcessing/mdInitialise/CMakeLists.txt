set(_FILES
  mdInitialise.C
)
add_executable(mdInitialise ${_FILES})
target_compile_features(mdInitialise PUBLIC cxx_std_11)
target_link_libraries(mdInitialise PUBLIC dynamicMesh molecularMeasurements molecule potential)
target_include_directories(mdInitialise PUBLIC
  .
)
