set(_FILES
  fakeError/fakeErrorFunctionObject.C
)
add_library($(FOAM_USER_LIBBIN)/libtestFunctionObjects ${_FILES})
target_compile_features($(FOAM_USER_LIBBIN)/libtestFunctionObjects PUBLIC cxx_std_11)
set_property(TARGET $(FOAM_USER_LIBBIN)/libtestFunctionObjects PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions($(FOAM_USER_LIBBIN)/libtestFunctionObjects PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_include_directories($(FOAM_USER_LIBBIN)/libtestFunctionObjects PUBLIC
  .
)
install(TARGETS $(FOAM_USER_LIBBIN)/libtestFunctionObjects DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
