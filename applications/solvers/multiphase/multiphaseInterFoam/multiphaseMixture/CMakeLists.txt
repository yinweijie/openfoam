set(_FILES
  phase/phase.C
  alphaContactAngle/alphaContactAngleFvPatchScalarField.C
  multiphaseMixture.C
)
add_library(multiphaseInterFoam ${_FILES})
target_compile_features(multiphaseInterFoam PUBLIC cxx_std_11)
set_property(TARGET multiphaseInterFoam PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(multiphaseInterFoam PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(multiphaseInterFoam PUBLIC finiteVolume incompressibleTransportModels interfaceProperties meshTools)
target_include_directories(multiphaseInterFoam PUBLIC
  .
)
install(TARGETS multiphaseInterFoam DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
