set(_FILES
  uncoupledKinematicParcelFoam.C
)
add_executable(uncoupledKinematicParcelFoam ${_FILES})
target_compile_features(uncoupledKinematicParcelFoam PUBLIC cxx_std_11)
target_link_libraries(uncoupledKinematicParcelFoam PUBLIC faOptions fvOptions lagrangianTurbulence regionFaModels surfaceFilmModels)
target_include_directories(uncoupledKinematicParcelFoam PUBLIC
  .
)
