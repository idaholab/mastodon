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
#include "StressDivergenceIsolator.h"

// MOOSE includes
#include "Assembly.h"
#include "Material.h"
#include "MooseVariable.h"
#include "SystemBase.h"
#include "NonlinearSystem.h"
#include "MooseMesh.h"

// libmesh includes
#include "libmesh/quadrature.h"

registerMooseObject("MastodonApp", StressDivergenceIsolator);

InputParameters
StressDivergenceIsolator::validParams()
{
  InputParameters params = Kernel::validParams();
  params.addClassDescription("Kernel for isolator element");
  params.addRequiredParam<unsigned int>(
      "component",
      "An integer corresponding to the direction "
      "the variable this kernel acts in. (0 for x, "
      "1 for y, 2 for z, 3 for rot_x, 4 for rot_y and 5 for rot_z).");
  params.addRequiredCoupledVar("displacements", "The displacement variables for isolator.");
  params.addRequiredCoupledVar("rotations", "The rotation variables for the isolator.");
  params.addParam<Real>(
      "zeta",
      0.0,
      "Name of material property or a constant real number defining the zeta parameter for the "
      "Rayleigh damping.");
  params.addRangeCheckedParam<Real>(
      "alpha", 0.0, "alpha >= -0.3333 & alpha <= 0.0", "alpha parameter for HHT time integration");
  params.addParam<bool>("static_initialization",
                        false,
                        "Set to true to get the system to "
                        "equilibrium under gravity by running a "
                        "quasi-static analysis (by solving Ku = F) "
                        "in the first time step");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

StressDivergenceIsolator::StressDivergenceIsolator(const InputParameters & parameters)
  : Kernel(parameters),
    _component(getParam<unsigned int>("component")),
    _ndisp(coupledComponents("displacements")),
    _disp_var(_ndisp),
    _nrot(coupledComponents("rotations")),
    _rot_var(_nrot),
    _Fg(getMaterialPropertyByName<ColumnMajorMatrix>("global_forces")),
    _Kg(getMaterialPropertyByName<ColumnMajorMatrix>("global_stiffness_matrix")),
    _zeta(getParam<Real>("zeta")),
    _alpha(getParam<Real>("alpha")),
    _isDamped(_zeta != 0.0 || std::abs(_alpha) > 0.0),
    _Fg_old(_isDamped ? &getMaterialPropertyOld<ColumnMajorMatrix>("global_forces") : nullptr),
    _Fg_older(_isDamped ? &getMaterialPropertyOlder<ColumnMajorMatrix>("global_forces") : nullptr),
    _static_initialization(getParam<bool>("static_initialization"))
{
  if (_component > 5)
    mooseError("Error in StressDivergenceIsolator block ",
               name(),
               ". Please enter an integer value between 0 and 5 for the 'component' parameter.");

  if (_ndisp != _nrot)
    mooseError("Error in StressDivergenceIsolator block ",
               name(),
               ". The number of displacement and rotation variables should be the same.");

  for (unsigned int i = 0; i < _ndisp; ++i)
    _disp_var[i] = coupled("displacements", i);

  for (unsigned int i = 0; i < _nrot; ++i)
    _rot_var[i] = coupled("rotations", i);
}

void
StressDivergenceIsolator::computeResidual()
{
  prepareVectorTag(_assembly, _var.number());

  mooseAssert(_local_re.size() == 2, "Isolator element only has two nodes.");

  ColumnMajorMatrix global_force_res = _Fg[0];

  // add contributions from stiffness proportional damping (non-zero _zeta) or HHT time integration
  // (non-zero _alpha)
  if (_isDamped && _dt > 0.0 && !(_static_initialization && _t <= 2 * _dt))
    global_force_res = global_force_res * (1.0 + _alpha + (1.0 + _alpha) * _zeta / _dt) -
                       (*_Fg_old)[0] * (_alpha + (1.0 + 2.0 * _alpha) * _zeta / _dt) +
                       (*_Fg_older)[0] * (_alpha * _zeta / _dt);

  _local_re(0) = global_force_res(_component);

  _local_re(1) = global_force_res(_component + 6);

  accumulateTaggedLocalResidual();

  if (_has_save_in)
    for (unsigned int i = 0; i < _save_in.size(); ++i)
      _save_in[i]->sys().solution().add_vector(_local_re, _save_in[i]->dofIndices());
}

void
StressDivergenceIsolator::computeJacobian()
{
  prepareMatrixTag(_assembly, _var.number(), _var.number());

  // i and j are looping over nodes
  for (unsigned int i = 0; i < _test.size(); ++i)
    for (unsigned int j = 0; j < _phi.size(); ++j)
      _local_ke(i, j) += _Kg[0](i * 6 + _component, j * 6 + _component);

  // scaling factor for Rayliegh damping and HHT time integration
  if (_isDamped && _dt > 0.0 && !(_static_initialization && _t == _dt))
    _local_ke *= (1.0 + _alpha + (1.0 + _alpha) * _zeta / _dt);

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
StressDivergenceIsolator::computeOffDiagJacobian(const unsigned int jvar_num)
// coupling one variable to another (disp x to disp y, etc)
{
  if (jvar_num == _var.number())
    // jacobian calculation if jvar is the same as the current variable i.e.,
    // diagonal elements
    computeJacobian();

  else
  // jacobian calculation for off-diagonal elements
  {
    unsigned int coupled_component = 0;
    bool coupled = false;
    // finding which variable jvar is
    for (unsigned int i = 0; i < _ndisp; ++i)
    {
      if (jvar_num == _disp_var[i])
      {
        coupled_component = i;
        coupled = true;
        break;
      }
      else if (jvar_num == _rot_var[i])
      {
        coupled_component = i + 3;
        coupled = true;
        break;
      }
    }

    prepareMatrixTag(_assembly, _var.number(), jvar_num);

    // getting the jacobian from assembly
    // DenseMatrix<Number> & ke = _assembly.jacobianBlock(_var.number(), jvar_num);
    if (coupled)
    {
      for (unsigned int i = 0; i < _test.size(); ++i)
        for (unsigned int j = 0; j < _phi.size(); ++j)
          _local_ke(i, j) += _Kg[0](i * 6 + _component, j * 6 + coupled_component);
    }

    // scaling factor for Rayliegh damping and HHT time integration
    if (_isDamped && _dt > 0.0 && !(_static_initialization && _t == _dt))
      _local_ke *= (1.0 + _alpha + (1.0 + _alpha) * _zeta / _dt);

    accumulateTaggedLocalMatrix();
  }
}
