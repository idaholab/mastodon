//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef STRESSDIVERGENCESPRING_H
#define STRESSDIVERGENCESPRING_H

#include "Kernel.h"

// Forward Declarations
class StressDivergenceSpring;
class RankTwoTensor;

template <>
InputParameters validParams<StressDivergenceSpring>();

class StressDivergenceSpring : public Kernel
{
public:
  StressDivergenceSpring(const InputParameters & parameters);
  virtual void computeResidual() override;
  virtual void computeJacobian() override;
  virtual void computeOffDiagJacobian(MooseVariableFEBase & jvar) override;
  using Kernel::computeOffDiagJacobian;

protected:
  virtual Real computeQpResidual() override { return 0.0; }

  /// Direction along which force/moment is calculated
  const unsigned int _component;

  /// Number of coupled displacement variables
  unsigned int _ndisp;

  /// Variable numbers corresponding to displacement variables
  std::vector<unsigned int> _disp_var;

  /// Number of coupled rotational variables
  unsigned int _nrot;

  /// Variable numbers corresponding to rotational variables
  std::vector<unsigned int> _rot_var;

  /// Spring forces
  const MaterialProperty<RealVectorValue> & _spring_forces;

  /// Spring moments
  const MaterialProperty<RealVectorValue> & _spring_moments;

  /// Displacement stiffness matrix
  const MaterialProperty<RankTwoTensor> & _kdd;

  /// Rotation stiffness matrix
  const MaterialProperty<RankTwoTensor> & _krr;

  /// Displacement rotation stiffness matrix
  // const MaterialProperty<RankTwoTensor> & _kdr;

  /// Rotation stiffness matrix
  const MaterialProperty<RankTwoTensor> & _total_global_to_local_rotation;
};

#endif // STRESSDIVERGENCESPRING_H
