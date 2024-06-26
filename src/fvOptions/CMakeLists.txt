set(_FILES
  cellSetOption/cellSetOption.C
  interRegionOption/interRegionOption.C
  sources/general/codedSource/codedFvSources.C
  sources/general/semiImplicitSource/semiImplicitSources.C
  sources/derived/acousticDampingSource/acousticDampingSource.C
  sources/derived/actuationDiskSource/actuationDiskSource.C
  sources/derived/buoyancyEnergy/buoyancyEnergy.C
  sources/derived/buoyancyForce/buoyancyForce.C
  sources/derived/directionalPressureGradientExplicitSource/directionalPressureGradientExplicitSource.C
  sources/derived/explicitPorositySource/explicitPorositySource.C
  sources/derived/jouleHeatingSource/jouleHeatingSource.C
  sources/derived/meanVelocityForce/meanVelocityForce.C
  sources/derived/meanVelocityForce/patchMeanVelocityForce/patchMeanVelocityForce.C
  sources/derived/multiphaseStabilizedTurbulence/multiphaseStabilizedTurbulence.C
  sources/derived/phaseLimitStabilization/phaseLimitStabilization.C
  sources/derived/radialActuationDiskSource/radialActuationDiskSource.C
  sources/derived/rotorDiskSource/rotorDiskSource.C
  sources/derived/rotorDiskSource/bladeModel/bladeModel.C
  sources/derived/rotorDiskSource/profileModel/profileModel.C
  sources/derived/rotorDiskSource/profileModel/profileModelList.C
  sources/derived/rotorDiskSource/profileModel/lookup/lookupProfile.C
  sources/derived/rotorDiskSource/profileModel/series/seriesProfile.C
  sources/derived/rotorDiskSource/trimModel/trimModel/trimModel.C
  sources/derived/rotorDiskSource/trimModel/trimModel/trimModelNew.C
  sources/derived/rotorDiskSource/trimModel/fixed/fixedTrim.C
  sources/derived/rotorDiskSource/trimModel/targetCoeff/targetCoeffTrim.C
  sources/derived/solidificationMeltingSource/solidificationMeltingSource.C
  sources/derived/tabulatedAccelerationSource/tabulatedAccelerationSource.C
  sources/derived/tabulatedAccelerationSource/tabulated6DoFAcceleration/tabulated6DoFAcceleration.C
  sources/derived/viscousDissipation/viscousDissipation.C
  sources/derived/buoyancyTurbSource/buoyancyTurbSource.C
  sources/derived/patchCellsSource/patchCellsSource.C
  sources/derived/fanMomentumSource/fanMomentumSource.C
  sources/derived/heatExchangerSource/heatExchangerSource.C
  sources/derived/heatExchangerSource/heatExchangerModels/heatExchangerModel/heatExchangerModel.C
  sources/derived/heatExchangerSource/heatExchangerModels/heatExchangerModel/heatExchangerModelNew.C
  sources/derived/heatExchangerSource/heatExchangerModels/effectivenessTable/effectivenessTable.C
  sources/derived/heatExchangerSource/heatExchangerModels/referenceTemperature/referenceTemperature.C
  sources/interRegion/interRegionHeatTransfer/interRegionHeatTransferModel/interRegionHeatTransferModel.C
  sources/interRegion/interRegionHeatTransfer/constantHeatTransfer/constantHeatTransfer.C
  sources/interRegion/interRegionHeatTransfer/tabulatedHeatTransfer/tabulatedHeatTransfer.C
  sources/interRegion/interRegionHeatTransfer/tabulatedNTUHeatTransfer/tabulatedNTUHeatTransfer.C
  sources/interRegion/interRegionHeatTransfer/variableHeatTransfer/variableHeatTransfer.C
  sources/interRegion/interRegionExplicitPorositySource/interRegionExplicitPorositySource.C
  constraints/general/fixedValueConstraint/fixedValueConstraints.C
  constraints/general/mapFieldConstraint/mapFieldConstraints.C
  constraints/derived/fixedTemperatureConstraint/fixedTemperatureConstraint.C
  constraints/derived/velocityDampingConstraint/velocityDampingConstraint.C
  corrections/limitTemperature/limitTemperature.C
  corrections/limitVelocity/limitVelocity.C
  corrections/limitTurbulenceViscosity/limitTurbulenceViscosity.C
)
add_library(fvOptions ${_FILES})
target_compile_features(fvOptions PUBLIC cxx_std_11)
set_property(TARGET fvOptions PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(fvOptions PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(fvOptions PUBLIC compressibleTurbulenceModels incompressibleTurbulenceModels overset reactionThermophysicalModels)
target_include_directories(fvOptions PUBLIC
  .
)
install(TARGETS fvOptions DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
