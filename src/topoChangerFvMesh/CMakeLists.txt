set(_FILES
  topoChangerFvMesh/topoChangerFvMesh.C
  rawTopoChangerFvMesh/rawTopoChangerFvMesh.C
  dynamicMotionSolverTopoFvMesh/dynamicMotionSolverTopoFvMesh.C
  movingConeTopoFvMesh/movingConeTopoFvMesh.C
)
add_library(topoChangerFvMesh ${_FILES})
target_compile_features(topoChangerFvMesh PUBLIC cxx_std_11)
set_property(TARGET topoChangerFvMesh PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(topoChangerFvMesh PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(topoChangerFvMesh PUBLIC dynamicFvMesh)
target_include_directories(topoChangerFvMesh PUBLIC
  .
)
install(TARGETS topoChangerFvMesh DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
