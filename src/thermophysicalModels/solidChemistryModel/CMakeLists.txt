set(_FILES
  basicSolidChemistryModel/basicSolidChemistryModel.C
  basicSolidChemistryModel/basicSolidChemistryModelNew.C
  basicSolidChemistryModel/basicSolidChemistryModels.C
  solidChemistrySolver/makeSolidChemistrySolvers.C
)
add_library(solidChemistryModel ${_FILES})
target_compile_features(solidChemistryModel PUBLIC cxx_std_11)
set_property(TARGET solidChemistryModel PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(solidChemistryModel PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(solidChemistryModel PUBLIC chemistryModel compressibleTransportModels reactionThermophysicalModels solidThermo thermophysicalProperties)
target_include_directories(solidChemistryModel PUBLIC
  .
)
install(TARGETS solidChemistryModel DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
