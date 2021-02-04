//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SidesetMoment.h"
#include "metaphysicl/raw_type.h"

registerMooseObject("MastodonApp", SidesetMoment);
registerMooseObject("MastodonApp", ADSidesetMoment);

template <bool is_ad>
InputParameters
SidesetMomentTempl<is_ad>::validParams()
{
  InputParameters params = SideIntegralPostprocessor::validParams();
  params.addClassDescription("Computes the product of integrated reaction force and lever arm in a "
                             "user-specified direction "
                             "on a sideset from the surface traction");
  params.addParam<MaterialPropertyName>("stress_tensor", "The rank two stress tensor name");
  params.addParam<RealVectorValue>("stress_direction", "Stress direction");
  params.addCoupledVar("pressure", "The scalar pressure");
  params.addRequiredParam<RealVectorValue>(
      "reference_point", "Reference point on the sideset about which the moment is computed");
  params.addRequiredParam<RealVectorValue>("moment_direction", "Moment direction");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

template <bool is_ad>
SidesetMomentTempl<is_ad>::SidesetMomentTempl(const InputParameters & parameters)
  : SideIntegralPostprocessor(parameters),
    _tensor(isParamValid("stress_tensor")
                ? &getGenericMaterialProperty<RankTwoTensor, is_ad>("stress_tensor")
                : nullptr),
    _stress_direction(isParamValid("stress_direction")
                          ? &getParam<RealVectorValue>("stress_direction")
                          : nullptr),
    _pressure(isCoupled("pressure") ? &coupledValue("pressure") : nullptr),
    _reference_point(getParam<RealVectorValue>("reference_point")),
    _moment_direction(&getParam<RealVectorValue>("moment_direction"))
{
  if (_tensor && _pressure)
    mooseError(
        "In block ",
        name(),
        ", both the stress tensor and the pressure should not be provided at the same time.");
  if (_tensor && !_stress_direction)
    mooseError("In block ",
               name(),
               ", a direction vector should be provided along with the stress tensor.");
  if (!_tensor && !_pressure)
    mooseError(
        "In block ", name(), ", either the stress tensor or the pressure should be provided.");
}

template <bool is_ad>
Real
SidesetMomentTempl<is_ad>::computeQpIntegral()
{
  if (_tensor)
    _force_vector =
        (_normals[_qp] * (MetaPhysicL::raw_value((*_tensor)[_qp]) * (*_stress_direction))) *
        _normals[_qp];
  else
    _force_vector = ((*_pressure)[_qp]) * _normals[_qp];

  _moment_vector(0) = (_force_vector(1) * (_q_point[_qp] - _reference_point)(2) -
                       _force_vector(2) * (_q_point[_qp] - _reference_point)(1));
  _moment_vector(1) = (_force_vector(2) * (_q_point[_qp] - _reference_point)(0) -
                       _force_vector(0) * (_q_point[_qp] - _reference_point)(2));
  _moment_vector(2) = (_force_vector(0) * (_q_point[_qp] - _reference_point)(1) -
                       _force_vector(1) * (_q_point[_qp] - _reference_point)(0));

  return _moment_vector * (*_moment_direction);
}

template class SidesetMomentTempl<false>;
template class SidesetMomentTempl<true>;
