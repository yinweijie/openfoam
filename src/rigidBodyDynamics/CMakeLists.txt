set(_FILES
  bodies/rigidBody/rigidBody.C
  bodies/masslessBody/masslessBody.C
  bodies/jointBody/jointBody.C
  bodies/compositeBody/compositeBody.C
  bodies/subBody/subBody.C
  bodies/sphere/sphere.C
  bodies/cuboid/cuboid.C
  joints/joint/joint.C
  joints/null/nullJoint.C
  joints/composite/compositeJoint.C
  joints/floating/floatingJoint.C
  joints/Rx/Rx.C
  joints/Ry/Ry.C
  joints/Rz/Rz.C
  joints/Ra/Ra.C
  joints/Rs/Rs.C
  joints/Rzyx/Rzyx.C
  joints/Rxyz/Rxyz.C
  joints/Ryxz/Ryxz.C
  joints/Px/Px.C
  joints/Py/Py.C
  joints/Pz/Pz.C
  joints/Pa/Pa.C
  joints/Pxyz/Pxyz.C
  restraints/restraint/rigidBodyRestraint.C
  restraints/restraint/rigidBodyRestraintNew.C
  restraints/linearSpring/linearSpring.C
  restraints/linearDamper/linearDamper.C
  restraints/linearAxialAngularSpring/linearAxialAngularSpring.C
  restraints/sphericalAngularDamper/sphericalAngularDamper.C
  restraints/prescribedRotation/prescribedRotation.C
  restraints/externalForce/externalForce.C
  restraints/softWall/softWall.C
  rigidBodyModel/rigidBodyModel.C
  rigidBodyModel/forwardDynamics.C
  rigidBodyModelState/rigidBodyModelState.C
  rigidBodyModelState/rigidBodyModelStateIO.C
  rigidBodyMotion/rigidBodyMotion.C
  rigidBodyMotion/rigidBodyMotionIO.C
  rigidBodySolvers/rigidBodySolver/rigidBodySolver.C
  rigidBodySolvers/rigidBodySolver/rigidBodySolverNew.C
  rigidBodySolvers/symplectic/symplectic.C
  rigidBodySolvers/Newmark/Newmark.C
  rigidBodySolvers/CrankNicolson/CrankNicolson.C
)
add_library(rigidBodyDynamics ${_FILES})
target_compile_features(rigidBodyDynamics PUBLIC cxx_std_11)
set_property(TARGET rigidBodyDynamics PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(rigidBodyDynamics PUBLIC WM_LABEL_SIZE=${WM_LABEL_SIZE} WM_${WM_PRECISION} NoRepository OPENFOAM=${OPENFOAM_VERSION})
target_link_libraries(rigidBodyDynamics PUBLIC OpenFOAM)
target_include_directories(rigidBodyDynamics PUBLIC
  .
)
install(TARGETS rigidBodyDynamics DESTINATION ${CMAKE_INSTALL_LIBDIR}/openfoam EXPORT openfoam-targets)
