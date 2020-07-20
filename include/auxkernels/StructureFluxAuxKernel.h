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

class StructureFluxAuxKernel : public AuxKernel
{
public:
  static InputParameters validParams();

  StructureFluxAuxKernel(const InputParameters & parameters);

  virtual ~StructureFluxAuxKernel() {}

protected:
  virtual Real computeValue();

  const VariableValue & _u_dotdot;
  Real _fluiddens;
};
