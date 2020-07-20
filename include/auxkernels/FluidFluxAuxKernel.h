//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "AuxKernel.h"

class FluidFluxAuxKernel : public AuxKernel
{
public:
  static InputParameters validParams();

  FluidFluxAuxKernel(const InputParameters & parameters);

  virtual ~FluidFluxAuxKernel() {}

protected:
  virtual Real computeValue();

  // const VariableGradient & _grad_u;
  const VariableGradient & _grad_press;
  Real _fluiddens;
};
