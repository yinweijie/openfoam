set(_FILES
  kinematicParcelFoam.C
)
add_executable(kinematicParcelFoam ${_FILES})
target_compile_features(kinematicParcelFoam PUBLIC cxx_std_11)
target_link_libraries(kinematicParcelFoam PUBLIC atmosphericModels faOptions lagrangianTurbulence regionFaModels surfaceFilmDerivedFvPatchFields topoChangerFvMesh)
target_include_directories(kinematicParcelFoam PUBLIC
  .
)
