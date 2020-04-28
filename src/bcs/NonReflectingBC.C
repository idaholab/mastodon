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
#include "Function.h"
#include "MooseError.h"
#include "MooseMesh.h"
#include "NonReflectingBC.h"

registerMooseObject("MastodonApp", NonReflectingBC);

InputParameters
NonReflectingBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  params.addClassDescription("Applies Lysmer damper in the normal and "
                             "tangential directions to soil boundary.");
  params += NonReflectingBC::commonParameters();
  params.addRequiredParam<unsigned int>("component",
                                        "The direction in which the Lysmer damper is applied.");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

InputParameters
NonReflectingBC::commonParameters()
{
  InputParameters params = emptyInputParameters();
  params.addCoupledVar("displacements",
                       "The vector of displacement variables. "
                       "The size of this vector must be same "
                       "as the number of dimensions.");
  params.addCoupledVar("velocities",
                       "The vector of velocity variables that "
                       "are coupled to the displacement "
                       "variables. The size of this vector must "
                       "be same as that of displacements.");
  params.addCoupledVar("accelerations",
                       "The vector of acceleration variables that are coupled "
                       "to the displacement variables. The size of this vector "
                       "must be same as that of displacements.");
  params.addRequiredParam<Real>("beta", "The beta parameter for Newmark time integration.");
  params.addRequiredParam<Real>("gamma", "The gamma parameter for Newmark time integration.");
  params.addParam<Real>("alpha", 0.0, "The alpha parameter for HHT time integration.");
  params.addRequiredRangeCheckedParam<Real>("density", "density>0.0", "Density of the material.");
  params.addRequiredRangeCheckedParam<Real>(
      "p_wave_speed", "p_wave_speed>0.0", "P-wave speed of the material.");
  params.addRequiredRangeCheckedParam<Real>(
      "shear_wave_speed", "shear_wave_speed>0.0", "shear wave speed of the material.");
  return params;
}

NonReflectingBC::NonReflectingBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _component(getParam<unsigned int>("component")),
    _ndisp(coupledComponents("displacements")),
    _disp(3),
    _disp_var(3),
    _disp_old(3),
    _vel_old(3),
    _accel_old(3),
    _beta(getParam<Real>("beta")),
    _gamma(getParam<Real>("gamma")),
    _alpha(getParam<Real>("alpha")),
    _density(getParam<Real>("density")),
    _p_wave_speed(getParam<Real>("p_wave_speed")),
    _shear_wave_speed(getParam<Real>("shear_wave_speed"))
{

  // Error checking on variable vectors
  if (_ndisp != _mesh.dimension())
    mooseError("The number of variables listed in the 'displacements' parameter in \"",
               name(),
               "\" block must match the mesh dimension.");

  if (coupledComponents("velocities") != _mesh.dimension())
    mooseError("The number of variables listed in the 'velocities' parameter in \"",
               name(),
               "\" block must match the mesh dimension.");

  if (coupledComponents("accelerations") != _mesh.dimension())
    mooseError("The number of variables listed in the 'accelerations' parameter in \"",
               name(),
               "\" block must match the mesh dimension.");

  if (_component >= _mesh.dimension())
    mooseError(
        "The 'component' parameter in \"", name(), "\" block should be less than mesh dimension.");

  // Populate coupled variable information
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    _disp[i] = &coupledValue("displacements", i);
    _disp_var[i] = coupled("displacements", i);
    _disp_old[i] = &coupledValueOld("displacements", i);
    _vel_old[i] = &coupledValueOld("velocities", i);
    _accel_old[i] = &coupledValueOld("accelerations", i);
  }
}

Real
NonReflectingBC::computeQpResidual()
{
  std::vector<Real> vel(3, 0.0);
  Real accel(0.0);

  Real normal_vel = 0.0;
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    accel = 1. / _beta * (((*_disp[i])[_qp] - (*_disp_old[i])[_qp]) / (_dt * _dt) -
                          (*_vel_old[i])[_qp] / _dt - (*_accel_old[i])[_qp] * (0.5 - _beta));
    vel[i] =
        (*_vel_old[i])[_qp] + (_dt * (1. - _gamma)) * (*_accel_old[i])[_qp] + _gamma * _dt * accel;
    vel[i] = (1. + _alpha) * vel[i] - _alpha * (*_vel_old[i])[_qp]; // HHT time integration
    normal_vel += vel[i] * _normals[_qp](i);
  }
  // residual is test[i][_qp] *( density* V_p * normal component of velocity +
  // density * V_s* tangential component of velocity)
  return _test[_i][_qp] * _density *
         (_p_wave_speed * normal_vel * _normals[_qp](_component) +
          _shear_wave_speed * (vel[_component] - normal_vel * _normals[_qp](_component)));
}

Real
NonReflectingBC::computeQpJacobian()
{
  return _test[_i][_qp] * _density *
         ((_p_wave_speed - _shear_wave_speed) * _normals[_qp](_component) *
              _normals[_qp](_component) +
          _shear_wave_speed) *
         (1. + _alpha) * _gamma / _beta / _dt * _phi[_j][_qp];
}

Real
NonReflectingBC::computeQpOffDiagJacobian(unsigned int jvar)
{
  unsigned int coupled_component = 0;
  bool active(false);

  for (unsigned int i = 0; i < _ndisp; ++i)
    if (jvar == _disp_var[i])
    {
      coupled_component = i;
      active = true;
    }

  if (active)
    return _test[_i][_qp] * _density * (_p_wave_speed - _shear_wave_speed) *
           _normals[_qp](_component) * _normals[_qp](coupled_component) * (1. + _alpha) * _gamma /
           _beta / _dt * _phi[_j][_qp];

  return 0.0;
}
