set(_FILES
  laminarFlameSpeed/laminarFlameSpeed.C
  laminarFlameSpeed/laminarFlameSpeedNew.C
  constant/constant.C
  Gulders/Gulders.C
  GuldersEGR/GuldersEGR.C
  RaviPetersen/RaviPetersen.C
)
add_library(laminarFlameSpeedModels ${_FILES})
target_compile_features(laminarFlameSpeedModels PUBLIC cxx_std_11)
set_property(TARGET laminarFlameSpeedModels PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(laminarFlameSpeedModels PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(laminarFlameSpeedModels PUBLIC reactionThermophysicalModels)
target_include_directories(laminarFlameSpeedModels PUBLIC
  .
)
install(TARGETS laminarFlameSpeedModels DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
