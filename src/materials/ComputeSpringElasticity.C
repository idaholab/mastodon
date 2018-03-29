// MASTODON includes
#include "ComputeSpringElasticity.h"

// MOOSE includes
#include "MooseMesh.h"
#include "Assembly.h"
#include "NonlinearSystem.h"
#include "MooseVariable.h"
#include "RankTwoTensor.h"

// libmesh includes
#include "libmesh/quadrature.h"
#include "libmesh/utility.h"

registerMooseObject("MastodonApp", ComputeSpringElasticity);

template <>
InputParameters
validParams<ComputeSpringElasticity>()
{
  InputParameters params = validParams<Material>();
  params.addClassDescription("Compute the forces and the stiffness matric corresponding to a two-noded spring element.");
  params.addRequiredCoupledVar("kx", "Axial stiffness of the spring");
  params.addRequiredCoupledVar("ky", "Shear stiffness in the y direction of the spring.");
  params.addRequiredCoupledVar("kz", "Shear stiffness in the z direction of the spring.");
  params.addRequiredCoupledVar("krx", "Torsional stiffness of the spring.");
  params.addRequiredCoupledVar("kry", "Rotational stiffness in the y direction of the spring.");
  params.addRequiredCoupledVar("krz", "Rotational stiffness in the z direction of the spring.");
  return params;
}

ComputeSpringElasticity::ComputeSpringElasticity(const InputParameters & parameters)
  : Material(parameters),
    _kx(coupledValue("kx")),
    _ky(coupledValue("ky")),
    _kz(coupledValue("kz")),
    _krx(coupledValue("krx")),
    _kry(coupledValue("kry")),
    _krz(coupledValue("krz")),
    _deformations(getMaterialPropertyByName<RealVectorValue>("deformations")),
    _rotations(getMaterialPropertyByName<RealVectorValue>("rotations")),
    _spring_forces(declareProperty<RealVectorValue>("forces")),
    _spring_moments(declareProperty<RealVectorValue>("moments")),
    _kdd(declareProperty<RankTwoTensor>("displacement_stiffness_matrix")),
    _krr(declareProperty<RankTwoTensor>("rotation_stiffness_matrix")),
    // _kdr(declareProperty<RankTwoTensor>("displacement_rotation_stiffness_matrix")),
    _total_global_to_local_rotation(getMaterialPropertyByName<RankTwoTensor>("total_global_to_local_rotation"))
{
}

void
ComputeSpringElasticity::initQpStatefulProperties()
{
  // computing spring forces
  computeForces();

  // computing spring stiffness matrix
  computeStiffnessMatrix();
}

void
ComputeSpringElasticity::computeForces()
// spring forces = K * deformations
// RealVectorValue spring_forces = dis_stiffness * _deformations;
// // spring moments = Kr * rotations
// RealVectorValue spring_moments = rot_stiffness * _rotations;
{
  RealVectorValue spring_forces_local(3, 0.0);
  RealVectorValue spring_moments_local(3, 0.0);

  // forces
  spring_forces_local(0) = _kx[_qp] * _deformations[_qp](0);
  spring_forces_local(1) = _ky[_qp] * _deformations[_qp](1);
  spring_forces_local(2) = _kz[_qp] * _deformations[_qp](2);
  // convert local forces to global
  _spring_forces[_qp] = _total_global_to_local_rotation[0].transpose() * spring_forces_local;

  // moments
  spring_moments_local(0) = _krx[_qp] * _rotations[_qp](0);
  spring_moments_local(1) = _kry[_qp] * _rotations[_qp](1);
  spring_moments_local(2) = _krz[_qp] * _rotations[_qp](2);
  // convert local moments to global
  _spring_moments[_qp] = _total_global_to_local_rotation[0].transpose() * spring_moments_local;
}

void
ComputeSpringElasticity::computeStiffnessMatrix()
{
  // The stiffness matrix is of the form
  // |  kdd  kdr  |
  // |  krd  krr  |
  // where kdd, krr, krd and kdr are all RankTwoTensors (3x3 matrix) and
  // matrix symmetry is assumed, namely, krd = kdr.transpose()

  // calculating deformation stiffnesses
  RankTwoTensor kdd_local;
  kdd_local(0,0) = _kx[_qp];
  kdd_local(1,1) = _ky[_qp];
  kdd_local(2,2) = _kz[_qp];
  // convert stiffness matrix from local to global
  _kdd[_qp] = _total_global_to_local_rotation[0].transpose() * kdd_local * _total_global_to_local_rotation[0];

  // calculating rotational stiffness
  RankTwoTensor krr_local;
  krr_local(0,0) = _krx[_qp];
  krr_local(1,1) = _kry[_qp];
  krr_local(2,2) = _krz[_qp];
  // convert stiffness matrix from local to global
  _krr[_qp] = _total_global_to_local_rotation[0].transpose() * krr_local * _total_global_to_local_rotation[0];

  // calculating stiffness matrix relating displacement and rotational variables
  // RankTwoTensor kdr_local;
  // // This matrix is empty for linear elastic spring elements
  // _kdr[_qp] = _total_global_to_local_rotation[0].transpose() * kdr_local * _total_global_to_local_rotation[0];
}
