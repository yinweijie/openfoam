set(_FILES
  cv2DControls/cv2DControls.C
)
add_library(cv2DMesh ${_FILES})
target_compile_features(cv2DMesh PUBLIC cxx_std_11)
set_property(TARGET cv2DMesh PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(cv2DMesh PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_include_directories(cv2DMesh PUBLIC
  .
)
install(TARGETS cv2DMesh DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
