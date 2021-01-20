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

registerMooseObject("TensorMechanicsApp", SidesetMoment);
registerMooseObject("TensorMechanicsApp", ADSidesetMoment);

template <bool is_ad>
InputParameters
SidesetMomentTempl<is_ad>::validParams()
{
  InputParameters params = SideIntegralPostprocessor::validParams();
  params.addClassDescription("Computes the integrated reaction force in a user-specified direction "
                             "on a sideset from the surface traction");
  params.addRequiredParam<MaterialPropertyName>("stress_tensor", "The rank two stress tensor name");
  params.addRequiredParam<RealVectorValue>("direction",
                                           "Direction in which the force is to be computed");
  params.addRequiredParam<RealVectorValue>(
      "ref_point", "Reference point on the sideset about which the moment is computed");
  params.addRequiredParam<unsigned int>("leverarm_direction", "Lever arm direction");
  params.set<bool>("use_displaced_mesh") = true;
  return params;
}

template <bool is_ad>
SidesetMomentTempl<is_ad>::SidesetMomentTempl(const InputParameters & parameters)
  : SideIntegralPostprocessor(parameters),
    _tensor(getGenericMaterialProperty<RankTwoTensor, is_ad>("stress_tensor")),
    _dir(getParam<RealVectorValue>("direction")),
    _ref_point(getParam<RealVectorValue>("ref_point")),
    _leverarm_direction(getParam<unsigned int>("leverarm_direction"))
{
  if (_leverarm_direction > 2)
    paramError("leverarm_direction", "leverarm_direction can be either 0, 1, or 2.");
}

template <bool is_ad>
Real
SidesetMomentTempl<is_ad>::computeQpIntegral()
{
  return _normals[_qp] * (MetaPhysicL::raw_value(_tensor[_qp]) * _dir) *
         std::abs(_ref_point(_leverarm_direction) - _q_point[_qp](_leverarm_direction));
}

template class SidesetMomentTempl<false>;
template class SidesetMomentTempl<true>;
