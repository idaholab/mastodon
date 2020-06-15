//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "InterfaceKernel.h"

/**
 * DG kernel for interfacing diffusion between two variables on adjacent blocks
 */
class FluidStructureInterface : public InterfaceKernel
{
public:
  static InputParameters validParams();

  FluidStructureInterface(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual(Moose::DGResidualType type) override;
  virtual Real computeQpJacobian(Moose::DGJacobianType type) override;

  const MaterialProperty<Real> & _D;
  const MaterialProperty<Real> & _D_neighbor;
  const VariableValue & _u_dotdot;
  const VariableValue & _du_dotdot_du;

  // const VariableGradient & _grad_press;

  // const MaterialProperty<Real> & _D_neighbor;
};
