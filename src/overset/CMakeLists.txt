set(_FILES
  cellCellStencil/cellCellStencil/cellCellStencil.C
  cellCellStencil/cellVolumeWeight/cellVolumeWeightCellCellStencil.C
  cellCellStencil/cellCellStencil/cellCellStencilObject.C
  cellCellStencil/inverseDistance/inverseDistanceCellCellStencil.C
  cellCellStencil/inverseDistance/waveMethod.C
  cellCellStencil/inverseDistance/meshToMeshData.C
  cellCellStencil/trackingInverseDistance/voxelMeshSearch.C
  cellCellStencil/trackingInverseDistance/trackingInverseDistanceCellCellStencil.C
  cellCellStencil/leastSquares/leastSquaresCellCellStencil.C
  oversetFvMesh/oversetFvMeshBase.C
  oversetFvMesh/dynamicOversetFvMesh/dynamicOversetFvMesh.C
  oversetFvMesh/staticOversetFvMesh/staticOversetFvMesh.C
  fvMeshPrimitiveLduAddressing/fvMeshPrimitiveLduAddressing.C
  oversetPolyPatch/oversetPolyPatch.C
  oversetPolyPatch/oversetFvPatch.C
  oversetPolyPatch/oversetFvPatchFields.C
  oversetPolyPatch/oversetFvsPatchFields.C
  oversetPolyPatch/oversetPointPatch.C
  oversetPolyPatch/oversetPointPatchFields.C
  oversetAdjustPhi/oversetAdjustPhi.C
  regionsToCell/regionsToCell.C
  lduPrimitiveProcessorInterface/GAMG/calculatedProcessorGAMGInterface.C
  lduPrimitiveProcessorInterface/GAMG/calculatedProcessorGAMGInterfaceField.C
  oversetCoupledPolyPatch/oversetLduInterfaceField/oversetLduInterfaceField.C
  oversetCoupledPolyPatch/oversetLduInterface/oversetLduInterface.C
  oversetCoupledPolyPatch/oversetGAMGInterface/oversetGAMGInterface.C
  oversetCoupledPolyPatch/oversetGAMGInterfaceField/oversetGAMGInterfaceField.C
  oversetPatchPhiErr/oversetPatchPhiErr.C
)
add_library(overset ${_FILES})
target_compile_features(overset PUBLIC cxx_std_11)
set_property(TARGET overset PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(overset PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(overset PUBLIC decompositionMethods dynamicFvMesh sampling)
target_include_directories(overset PUBLIC
  .
)
install(TARGETS overset DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
