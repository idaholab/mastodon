//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "StressDivergenceSpring.h"

// MOOSE includes
#include "Assembly.h"
#include "Material.h"
#include "MooseVariable.h"
#include "SystemBase.h"
#include "RankTwoTensor.h"
#include "NonlinearSystem.h"
#include "MooseMesh.h"

// libmesh includes
#include "libmesh/quadrature.h"

registerMooseObject("MastodonApp", StressDivergenceSpring);

template <>
InputParameters
validParams<StressDivergenceSpring>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Kernel for spring element");
  params.addRequiredParam<unsigned int>("component",
                                        "An integer corresponding to the direction "
                                        "the variable this kernel acts in. (0 for x, "
                                        "1 for y, 2 for z, 3 for rot_x, 4 for rot_y and 5 for rot_z).");
  params.addRequiredCoupledVar("displacements", "The displacement variables for spring.");
  params.addRequiredCoupledVar("rotations", "The rotation variables for the spring.");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

StressDivergenceSpring::StressDivergenceSpring(const InputParameters & parameters)
  : Kernel(parameters),
    _component(getParam<unsigned int>("component")),
    _ndisp(coupledComponents("displacements")),
    _disp_var(_ndisp),
    _nrot(coupledComponents("rotations")),
    _rot_var(_nrot),
    _spring_forces(getMaterialPropertyByName<RealVectorValue>("forces")),
    _spring_moments(getMaterialPropertyByName<RealVectorValue>("moments")),
    _kdd(getMaterialPropertyByName<RankTwoTensor>("displacement_stiffness_matrix")),
    _krr(getMaterialPropertyByName<RankTwoTensor>("rotation_stiffness_matrix")),
    // _kdr(getMaterialPropertyByName<RankTwoTensor>("displacement_rotation_stiffness_matrix")),
    _total_global_to_local_rotation(getMaterialPropertyByName<RankTwoTensor>("total_global_to_local_rotation"))
{
  if (_component > 5)
    mooseError("Error in StressDivergenceSpring block ", name(), ". Please enter an integer value between 0 and 5 for the 'component' parameter.");

  if (_ndisp != _nrot)
    mooseError("Error in StressDivergenceSpring block ", name(), ". The number of displacement and rotation variables should be the same.");

  for (unsigned int i = 0; i < _ndisp; ++i)
    _disp_var[i] = coupled("displacements", i);

  for (unsigned int i = 0; i < _nrot; ++i)
    _rot_var[i] = coupled("rotations", i);
}

void
StressDivergenceSpring::computeResidual()
{
  // Accessing residual vector, re, from MOOSE assembly
  DenseVector<Number> & re = _assembly.residualBlock(_var.number());
  mooseAssert(re.size() == 2, "Spring element has and only has two nodes.");
  _local_re.resize(re.size());
  _local_re.zero();

  // Calculating residual for node 0 (external forces on node 0)
  if (_component < 3)
    _local_re(0) = -_spring_forces[0](_component);
  else
    _local_re(0) = -_spring_moments[0](_component - 3);

  // External force on node 1 = -1 * external force on node 0
  _local_re(1) = -_local_re(0);

  re += _local_re;

  if (_has_save_in)
  {
    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    for (unsigned int i = 0; i < _save_in.size(); ++i)
      _save_in[i]->sys().solution().add_vector(_local_re, _save_in[i]->dofIndices());
  }
}

void
StressDivergenceSpring::computeJacobian()
{
  // Access Jacobian; size is n x n (n is number of nodes)
  DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), _var.number());
  _local_ke.resize(ke.m(), ke.n());
  _local_ke.zero();

  for (unsigned int i = 0; i < _test.size(); ++i)
    for (unsigned int j = 0; j < _phi.size(); ++j)
    {
      if (_component < 3)
        _local_ke(i, j) += (i == j ? 1 : -1) * _kdd[0](_component, _component);
      else
        _local_ke(i, j) += (i == j ? 1 : -1) * _krr[0](_component - 3, _component - 3);
    }

  ke += _local_ke;

  if (_has_diag_save_in)
  {
    unsigned int rows = ke.m();
    DenseVector<Number> diag(rows);
    for (unsigned int i = 0; i < rows; ++i)
      diag(i) = _local_ke(i, i);

    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    for (unsigned int i = 0; i < _diag_save_in.size(); ++i)
      _diag_save_in[i]->sys().solution().add_vector(diag, _diag_save_in[i]->dofIndices());
  }
}

void
StressDivergenceSpring::computeOffDiagJacobian(MooseVariableFEBase & jvar)
// coupling one variable to another (disp x to disp y, etc)
{
  size_t jvar_num = jvar.number();
  if (jvar_num == _var.number())
  // jacobian calculation if jvar is the same as the current variable i.e.,
  // diagonal elements
    computeJacobian();

  // Off diagonal elements are zero for linear elastic spring

  // else
  // // jacobian calculation for off-diagonal elements
  // {
  //   unsigned int coupled_component = 0;
  //   bool disp_disp_coupled = false;
  //   bool rot_rot_coupled = false;
  //   bool disp_rot_coupled = false;
  //
  //   for (unsigned int i = 0; i < _ndisp; ++i) // finding which variable jvar is
  //   {
  //     if (jvar_num == _disp_var[i])
  //     {
  //       coupled_component = i;
  //       disp_disp_coupled = true;
  //       break;
  //     }
  //
  //     if (jvar_num == _rot_var[i])
  //     {
  //       coupled_component = i + 3;
  //
  //     }
  //   }
  //
  //   DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), jvar);
  //
  //   if (disp_coupled)
  //     for (unsigned int i = 0; i < _test.size(); ++i)
  //       for (unsigned int j = 0; j < _phi.size(); ++j)
  //         ke(i, j) += (i == j ? 1 : -1) * computeStiffness(_component, coupled_component);
  // }
}
