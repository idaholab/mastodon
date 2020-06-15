//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "StructureFluxAuxKernel.h"
#include <iostream>

registerMooseObject("MastodonApp", StructureFluxAuxKernel);

InputParameters
StructureFluxAuxKernel::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Calculates the flux on the structure side.");
  params.addRequiredCoupledVar("dispx", "displacement variable");
  params.addRequiredParam<Real>("fluiddens", "fluid density");
  return params;
}

StructureFluxAuxKernel::StructureFluxAuxKernel(const InputParameters & parameters)
  : AuxKernel(parameters),
    _u_dotdot(coupledDotDot("dispx")),
    _fluiddens(getParam<Real>("fluiddens"))
{
}

Real
StructureFluxAuxKernel::computeValue()
{
  // std::cout << _u_dotdot[_qp] << std::endl;
  return -_u_dotdot[_qp] * _fluiddens;
}
