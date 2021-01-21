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
  params.addParam<RealVectorValue>("direction", "Direction in which the force is to be computed");
  params.addCoupledVar("p", "The scalar pressure");
  params.addRequiredParam<RealVectorValue>(
      "ref_point", "Reference point on the sideset about which the moment is computed");
  params.addRequiredParam<unsigned int>("leverarm_direction", "Lever arm direction");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

template <bool is_ad>
SidesetMomentTempl<is_ad>::SidesetMomentTempl(const InputParameters & parameters)
  : SideIntegralPostprocessor(parameters),
    _tensor(isParamValid("stress_tensor")
                ? &getGenericMaterialProperty<RankTwoTensor, is_ad>("stress_tensor")
                : nullptr),
    _dir(isParamValid("direction") ? &getParam<RealVectorValue>("direction") : nullptr),
    _p(isCoupled("p") ? &coupledValue("p") : nullptr),
    _ref_point(getParam<RealVectorValue>("ref_point")),
    _leverarm_direction(getParam<unsigned int>("leverarm_direction"))
{
  if (_tensor && _p)
    mooseError(
        "In block ",
        name(),
        ", both the stress tensor and the pressure should not be provided at the same time.");
  if (_tensor && !_dir)
    mooseError("In block ",
               name(),
               ", a direction vector should be provided along with the stress tensor.");
  if (!_tensor && !_p)
    mooseError(
        "In block ", name(), ", either the stress tensor or the pressure should be provided.");
  if (_leverarm_direction > 2)
    paramError("leverarm_direction", "leverarm_direction can be either 0, 1, or 2.");
}

template <bool is_ad>
Real
SidesetMomentTempl<is_ad>::computeQpIntegral()
{
  if (_tensor)
    return _normals[_qp] * (MetaPhysicL::raw_value((*_tensor)[_qp]) * (*_dir)) *
           std::abs(_ref_point(_leverarm_direction) - _q_point[_qp](_leverarm_direction));
  else
    return (*_p)[_qp] *
           std::abs(_ref_point(_leverarm_direction) - _q_point[_qp](_leverarm_direction));
}

template class SidesetMomentTempl<false>;
template class SidesetMomentTempl<true>;
