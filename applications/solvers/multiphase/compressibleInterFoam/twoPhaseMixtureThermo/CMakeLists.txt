set(_FILES
  twoPhaseMixtureThermo.C
)
add_library(twoPhaseMixtureThermo ${_FILES})
target_compile_features(twoPhaseMixtureThermo PUBLIC cxx_std_11)
set_property(TARGET twoPhaseMixtureThermo PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(twoPhaseMixtureThermo PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(twoPhaseMixtureThermo PUBLIC finiteVolume fluidThermophysicalModels interfaceProperties specie)
target_include_directories(twoPhaseMixtureThermo PUBLIC
  .
)
install(TARGETS twoPhaseMixtureThermo DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
