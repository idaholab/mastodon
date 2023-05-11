/*************************************************/
/*           DO NOT MODIFY THIS HEADER           */
/*                                               */
/*                     MASTODON                  */
/*                                               */
/*    (c) 2015 Battelle Energy Alliance, LLC     */
/*            ALL RIGHTS RESERVED                */
/*                                               */
/*   Prepared by Battelle Energy Alliance, LLC   */
/*     With the U. S. Department of Energy       */
/*                                               */
/*     See COPYRIGHT for full restrictions       */
/*************************************************/

// MASTODON includes

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

InputParameters
StressDivergenceSpring::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("Kernel for spring element");
  params.addRequiredParam<unsigned int>(
      "component",
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
    _spring_forces_global(getMaterialPropertyByName<ColumnMajorMatrix>("global_forces")),
    _spring_moments_global(getMaterialPropertyByName<ColumnMajorMatrix>("global_moments")),
    _kdd(getMaterialPropertyByName<ColumnMajorMatrix>("displacement_stiffness_matrix")),
    _krr(getMaterialPropertyByName<ColumnMajorMatrix>("rotation_stiffness_matrix")),
    _total_global_to_local_rotation(
        getMaterialPropertyByName<ColumnMajorMatrix>("total_global_to_local_rotation"))
{
  if (_component > 5)
    mooseError("Error in StressDivergenceSpring block ",
               name(),
               ". Please enter an integer value between 0 and 5 for the 'component' parameter.");

  if (_ndisp != _nrot)
    mooseError("Error in StressDivergenceSpring block ",
               name(),
               ". The number of displacement and rotation variables should be the same.");

  for (unsigned int i = 0; i < _ndisp; ++i)
    _disp_var[i] = coupled("displacements", i);

  for (unsigned int i = 0; i < _nrot; ++i)
    _rot_var[i] = coupled("rotations", i);
}

void
StressDivergenceSpring::computeResidual()
{
  // Accessing residual vector, re, from MOOSE assembly
  prepareVectorTag(_assembly, _var.number());
  mooseAssert(_local_re.size() == 2, "Spring element has and only has two nodes.");

  // Calculating residual for node 0 (external forces on node 0)
  if (_component < 3)
    _local_re(0) = -_spring_forces_global[0](_component);
  else
    _local_re(0) = -_spring_moments_global[0](_component - 3);

  // External force on node 1 = -1 * external force on node 0
  _local_re(1) = -_local_re(0);

  accumulateTaggedLocalResidual();

  if (_has_save_in)
    for (unsigned int i = 0; i < _save_in.size(); ++i)
      _save_in[i]->sys().solution().add_vector(_local_re, _save_in[i]->dofIndices());
}

void
StressDivergenceSpring::computeJacobian()
{
  // Access Jacobian; size is n x n (n is number of nodes)
  prepareMatrixTag(_assembly, _var.number(), _var.number());

  for (unsigned int i = 0; i < _test.size(); ++i)
    for (unsigned int j = 0; j < _phi.size(); ++j)
    {
      if (_component < 3)
        _local_ke(i, j) += (i == j ? 1 : -1) * _kdd[0](_component, _component);
      else
        _local_ke(i, j) += (i == j ? 1 : -1) * _krr[0](_component - 3, _component - 3);
    }

  accumulateTaggedLocalMatrix();

  if (_has_diag_save_in)
  {
    unsigned int rows = _local_ke.m();
    DenseVector<Number> diag(rows);
    for (unsigned int i = 0; i < rows; ++i)
      diag(i) = _local_ke(i, i);

    for (unsigned int i = 0; i < _diag_save_in.size(); ++i)
      _diag_save_in[i]->sys().solution().add_vector(diag, _diag_save_in[i]->dofIndices());
  }
}

void
StressDivergenceSpring::computeOffDiagJacobian(const unsigned int jvar_num)
// coupling one variable to another (disp x to disp y, etc)
{
  if (jvar_num == _var.number())
    // jacobian calculation if jvar is the same as the current variable i.e.,
    // diagonal elements
    computeJacobian();

  // Off-diagonal elements are zero for linear elastic spring
}
