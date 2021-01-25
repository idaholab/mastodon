
//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "SideIntegralPostprocessor.h"
#include "RankTwoTensor.h"

/**
 * This postprocessor computes the integral of the force multiplied by the leverarm on a sideset in
 * direction _dir
 */
template <bool is_ad>
class SidesetMomentTempl : public SideIntegralPostprocessor
{
public:
  static InputParameters validParams();

  SidesetMomentTempl(const InputParameters & parameters);

protected:
  virtual Real computeQpIntegral() override;

private:
  /// the stress tensor
  const GenericMaterialProperty<RankTwoTensor, is_ad> * _tensor;

  /// the direction along which the force is computed
  const unsigned int * _stress_dir;

  /// the pressure
  const VariableValue * const _p;

  /// the reference point on the sideset about which the moment is computed
  const RealVectorValue & _ref_point;

  /// the direction along which the lever arm is computed
  const unsigned int & _leverarm_direction;
};

typedef SidesetMomentTempl<false> SidesetMoment;
typedef SidesetMomentTempl<true> ADSidesetMoment;
