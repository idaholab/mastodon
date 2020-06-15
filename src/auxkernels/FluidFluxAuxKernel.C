//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FluidFluxAuxKernel.h"
#include <iostream>

registerMooseObject("MastodonApp", FluidFluxAuxKernel);

InputParameters
FluidFluxAuxKernel::validParams()
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription("Calculates the flux on the fluid side.");
  params.addRequiredCoupledVar("pressure", "pressure variable");
  params.addRequiredParam<Real>("fluiddens", "fluid density");
  // params.addRequiredCoupledVar("pressure", "pressure variable");
  return params;
}

FluidFluxAuxKernel::FluidFluxAuxKernel(const InputParameters & parameters)
  : AuxKernel(parameters),
    // _grad_u(gradient())
    _grad_press(coupledGradient("pressure")),
    _fluiddens(getParam<Real>("fluiddens"))
{
}

Real
FluidFluxAuxKernel::computeValue()
{
  // std::vector<Real> normals;
  // normals[0] = 1;
  // normals[1] = 0;
  // normals[2] = 0;
  // return _grad_u[_qp];// * _normals[_qp];
  // std::cout << _press[_qp](0) << std::endl;
  // std::cout << _press[_qp](1) << std::endl;
  // std::cout << _grad_press[_qp](1) << std::endl;
  return _fluiddens * _grad_press[_qp](0) + _fluiddens * _grad_press[_qp](1) + _fluiddens * _grad_press[_qp](2);
}
