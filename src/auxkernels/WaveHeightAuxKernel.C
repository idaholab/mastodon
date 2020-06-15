//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "WaveHeightAuxKernel.h"
#include <iostream>

registerMooseObject("MastodonApp", WaveHeightAuxKernel);

InputParameters
WaveHeightAuxKernel::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Calculates the current velocity using Newmark method.");
  params.addRequiredCoupledVar("pressure", "pressure variable");
  params.addRequiredParam<Real>("dens", "fluid density");
  params.addRequiredParam<Real>("grav", "acceleration due to gravity");
  return params;
}

WaveHeightAuxKernel::WaveHeightAuxKernel(const InputParameters & parameters)
  : AuxKernel(parameters),
    _press(coupledValue("pressure")),
    _dens(getParam<Real>("dens")),
    _grav(getParam<Real>("grav"))
{
}

Real
WaveHeightAuxKernel::computeValue()
{
  return _press[_qp]/(_dens * _grav);
}
