set(_FILES
  interPhaseChangeFoam.C
)
add_executable(interPhaseChangeFoam ${_FILES})
target_compile_features(interPhaseChangeFoam PUBLIC cxx_std_11)
target_link_libraries(interPhaseChangeFoam PUBLIC fvOptions incompressibleTurbulenceModels interfaceProperties phaseChangeTwoPhaseMixtures sampling twoPhaseProperties)
target_include_directories(interPhaseChangeFoam PUBLIC
  .
)
