set(_FILES
  compressibleTransportModel/compressibleTransportModel.C
)
add_library(compressibleTransportModels ${_FILES})
target_compile_features(compressibleTransportModels PUBLIC cxx_std_11)
set_property(TARGET compressibleTransportModels PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(compressibleTransportModels PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(compressibleTransportModels PUBLIC finiteVolume)
target_include_directories(compressibleTransportModels PUBLIC
  .
)
install(TARGETS compressibleTransportModels DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
