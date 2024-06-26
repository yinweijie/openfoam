set(_FILES
  blockVertices/blockVertex/blockVertex.C
  blockVertices/pointVertex/pointVertex.C
  blockVertices/projectVertex/projectVertex.C
  blockVertices/namedVertex/namedVertex.C
  blockEdges/bezier/bezier.C
  blockEdges/blockEdge/blockEdge.C
  blockEdges/lineDivide/lineDivide.C
  blockEdges/lineEdge/lineEdge.C
  blockEdges/polyLineEdge/polyLine.C
  blockEdges/polyLineEdge/polyLineEdge.C
  blockEdges/arcEdge/arcEdge.C
  blockEdges/BSplineEdge/BSpline.C
  blockEdges/BSplineEdge/BSplineEdge.C
  blockEdges/splineEdge/CatmullRomSpline.C
  blockEdges/splineEdge/splineEdge.C
  blockEdges/projectEdge/projectEdge.C
  blockEdges/projectCurveEdge/projectCurveEdge.C
  blockFaces/blockFace/blockFace.C
  blockFaces/projectFace/projectFace.C
  gradingDescriptor/gradingDescriptor.C
  gradingDescriptor/gradingDescriptors.C
  blockDescriptor/blockDescriptor.C
  blockDescriptor/blockDescriptorEdges.C
  blocks/block/block.C
  blocks/block/blockCreate.C
  blocks/namedBlock/namedBlock.C
  blockMesh/blockMesh.C
  blockMesh/blockMeshCreate.C
  blockMesh/blockMeshTopology.C
  blockMesh/blockMeshCheck.C
  blockMesh/blockMeshMergeGeometrical.C
  blockMesh/blockMeshMergeTopological.C
  blockMeshTools/blockMeshTools.C
  PDRblockMesh/PDRblock.C
  PDRblockMesh/PDRblockBlockMesh.C
  PDRblockMesh/PDRblockCreate.C
  PDRblockMesh/PDRblockLocation.C
  PDRblockMesh/PDRblockOuter.C
)
add_library(blockMeshLib ${_FILES})
target_compile_features(blockMeshLib PUBLIC cxx_std_11)
set_property(TARGET blockMeshLib PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(blockMeshLib PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(blockMeshLib PUBLIC meshTools)
set_target_properties(blockMeshLib PROPERTIES OUTPUT_NAME blockMesh)
target_include_directories(blockMeshLib PUBLIC
  .
)
install(TARGETS blockMeshLib DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
