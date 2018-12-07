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
#include "SeismicForce.h"

registerMooseObject("MastodonApp", SeismicForce);

template <>
InputParameters
validParams<SeismicForce>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addClassDescription("Applies a seismic force on a given boundary in a "
                             "given direction proportional to input velocity.");
  params.addRequiredParam<unsigned int>("component",
                                        "The direction in which the force is applied.");
  params.addRequiredParam<unsigned int>("vel_component",
                                        "The direction in which the input velocity is applied.");
  params.addParam<Real>("factor", 1.0, "Scaling factor to be applied to the force.");
  params.addParam<FunctionName>("velocity_function",
                                "The function that describes the input ground velocity.");
  params.addCoupledVar("velocity", "The variable that describes the input velocity.");
  params.addRequiredRangeCheckedParam<Real>(
      "density", "density>0.0", "Density of the underlying bedrock.");
  params.addRequiredRangeCheckedParam<Real>(
      "p_wave_speed", "p_wave_speed>0.0", "P-wave speed of the underlying bedrock.");
  params.addRequiredRangeCheckedParam<Real>(
      "shear_wave_speed", "shear_wave_speed>0.0", "shear wave speed of the underlying bedrock.");
  params.addParam<Real>(
      "alpha", 0.0, "The alpha parameter required for HHT time integration scheme.");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

SeismicForce::SeismicForce(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _component(getParam<unsigned int>("component")),
    _vel_component(getParam<unsigned int>("vel_component")),
    _factor(getParam<Real>("factor")),
    _alpha(getParam<Real>("alpha")),
    _density(getParam<Real>("density")),
    _p_wave_speed(getParam<Real>("p_wave_speed")),
    _shear_wave_speed(getParam<Real>("shear_wave_speed"))
{
  if (_component >= _mesh.dimension())
    mooseError("Invalid value for 'component' (",
               _component,
               ") given in \"",
               name(),
               "\" block, it must be a value from 0 to ",
               _mesh.dimension() - 1,
               ".");

  if (_vel_component >= _mesh.dimension())
    mooseError("Invalid value for 'vel_component' (",
               _vel_component,
               ") given in \"",
               name(),
               "\" block, it must be a value from 0 to ",
               _mesh.dimension() - 1,
               ".");

  if (!isParamValid("velocity_function") && !isParamValid("velocity"))
    mooseError("A function ('velocity_function') or variable ('velocity') "
               "describing the input velocity must be supplied in the \"",
               name(),
               "\" block.");
}

Real
SeismicForce::computeQpResidual()
{
  std::vector<Real> vel(3, 0.0);
  std::vector<Real> tangential_vel(3, 0.0);
  Real normal_vel(0.0);

  if (isParamValid("velocity_function"))
  {
    Function * const velocity_function = &getFunction("velocity_function");
    vel[_vel_component] = velocity_function->value(_t + _alpha * _dt, _q_point[_qp]);
  }
  else
  {
    const VariableValue & velocity = coupledValue("velocity");
    vel[_vel_component] = velocity[_qp];
  }

  normal_vel = vel[_vel_component] * _normals[_qp](_vel_component);

  for (unsigned int i = 0; i < _mesh.dimension(); i++)
    tangential_vel[i] = vel[i] - normal_vel * _normals[_qp](i);

  return _factor * _test[_i][_qp] * _density *
         (_p_wave_speed * normal_vel * _normals[_qp](_component) +
          _shear_wave_speed * tangential_vel[_component]);
}
