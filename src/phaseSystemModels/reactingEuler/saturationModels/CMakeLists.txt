set(_FILES
  saturationModel/saturationModel.C
  Antoine/Antoine.C
  AntoineExtended/AntoineExtended.C
  ArdenBuck/ArdenBuck.C
  polynomial/polynomial.C
  function1/function1.C
  constantSaturationConditions/constantSaturationConditions.C
)
add_library(saturationModel ${_FILES})
target_compile_features(saturationModel PUBLIC cxx_std_11)
set_property(TARGET saturationModel PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(saturationModel PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(saturationModel PUBLIC finiteVolume)
target_include_directories(saturationModel PUBLIC
  .
)
install(TARGETS saturationModel DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
