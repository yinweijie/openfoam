set(_FILES
  sizeDistribution/sizeDistribution.C
  phaseForces/phaseForces.C
)
add_library(phaseFunctionObjects ${_FILES})
target_compile_features(phaseFunctionObjects PUBLIC cxx_std_11)
set_property(TARGET phaseFunctionObjects PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(phaseFunctionObjects PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(phaseFunctionObjects PUBLIC dynamicMesh fieldFunctionObjects meshTools reactingMultiphaseSystem)
target_include_directories(phaseFunctionObjects PUBLIC
  .
)
install(TARGETS phaseFunctionObjects DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
