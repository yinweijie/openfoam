set(_FILES
  decompositionInformation.C
  decompositionModel.C
  dimFieldDecomposer.C
  fvFieldDecomposer.C
  fvFieldDecomposerCache.C
  pointFieldDecomposer.C
  pointFieldDecomposerCache.C
  lagrangianFieldDecomposer.C
  lagrangianFieldDecomposerCache.C
)
add_library(decompose ${_FILES})
target_compile_features(decompose PUBLIC cxx_std_11)
set_property(TARGET decompose PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(decompose PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(decompose PUBLIC decompositionMethods lagrangian)
target_include_directories(decompose PUBLIC
  .
)
install(TARGETS decompose DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
