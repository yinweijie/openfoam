set(_FILES
  simpleReactingParcelFoam.C
)
add_executable(simpleReactingParcelFoam ${_FILES})
target_compile_features(simpleReactingParcelFoam PUBLIC cxx_std_11)
target_link_libraries(simpleReactingParcelFoam PUBLIC faOptions fvOptions lagrangianTurbulence regionFaModels surfaceFilmModels)
target_include_directories(simpleReactingParcelFoam PUBLIC
  .
)
