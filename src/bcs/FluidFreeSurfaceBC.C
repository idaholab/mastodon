//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FluidFreeSurfaceBC.h"

registerMooseObject("MooseApp", FluidFreeSurfaceBC);

defineLegacyParams(FluidFreeSurfaceBC);

InputParameters
FluidFreeSurfaceBC::validParams()
{
   InputParameters params = IntegratedBC::validParams();
   params.addParam<Real>("alpha", 1, "No idea.");
   return params;
}

FluidFreeSurfaceBC::FluidFreeSurfaceBC(const InputParameters & parameters)
  : IntegratedBC(parameters), _alpha(getParam<Real>("alpha")),
   _u_dotdot(dotDot()),
   _du_dotdot_du(dotDotDu())
{
}

Real FluidFreeSurfaceBC::computeQpResidual()
{
  return _test[_i][_qp] * _alpha * _u_dotdot[_qp] / 1.;
}

Real FluidFreeSurfaceBC::computeQpJacobian()
{
  return _test[_i][_qp] * _alpha * _du_dotdot_du[_qp] * _phi[_j][_qp] / 1.;
}

// #include "FluidFreeSurfaceBC.h"
// #include "SubProblem.h"
//
// registerMooseObject("MooseApp", FluidFreeSurfaceBC);
//
// defineLegacyParams(FluidFreeSurfaceBC);
//
// InputParameters
// FluidFreeSurfaceBC::validParams()
// {
//   InputParameters params = ArrayIntegratedBC::validParams();
//   params.addParam<RealEigenVector>("alpha", "Ratio between directional gradient and solution");
//   params.addClassDescription("Imposes the Robin boundary condition $\\partial_n "
//                              "\\vec{u}=-\\frac{\\vec{\\alpha}}{2}\\vec{u}$.");
//   return params;
// }
//
// FluidFreeSurfaceBC::FluidFreeSurfaceBC(const InputParameters & parameters)
//   : ArrayIntegratedBC(parameters),
//     _alpha(isParamValid("alpha") ? getParam<RealEigenVector>("alpha")
//                                  : RealEigenVector::Ones(_count))
// {
//   _u_dotdot = &(_var.uDotDot());
// }
//
// RealEigenVector
// FluidFreeSurfaceBC::computeQpResidual()
// {
//   return _alpha.cwiseProduct((*_u_dotdot)[_qp]) * _test[_i][_qp];
// }
//
// RealEigenVector
// FluidFreeSurfaceBC::computeQpJacobian()
// {
//   return _test[_i][_qp] * _phi[_j][_qp] * _alpha;
// }
