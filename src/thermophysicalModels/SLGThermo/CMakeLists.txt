set(_FILES
  SLGThermo/SLGThermo.C
)
add_library(SLGThermo ${_FILES})
target_compile_features(SLGThermo PUBLIC cxx_std_11)
set_property(TARGET SLGThermo PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(SLGThermo PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(SLGThermo PUBLIC reactionThermophysicalModels)
target_include_directories(SLGThermo PUBLIC
  .
)
install(TARGETS SLGThermo DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
