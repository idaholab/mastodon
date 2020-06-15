//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FluidStructureInterface.h"
#include <iostream>

registerMooseObject("MastodonApp", FluidStructureInterface);

InputParameters
FluidStructureInterface::validParams()
{
  InputParameters params = InterfaceKernel::validParams();
  params.addParam<MaterialPropertyName>("D", "D", "The density.");
  params.addParam<MaterialPropertyName>("D_neighbor", "D_neighbor", "The density.");
  return params;
}

FluidStructureInterface::FluidStructureInterface(const InputParameters & parameters)
  : InterfaceKernel(parameters),
    _D(getMaterialProperty<Real>("D")),
    _D_neighbor(getNeighborMaterialProperty<Real>("D_neighbor")),
    _u_dotdot(dotDot()),
    _du_dotdot_du(dotDotDu())
    // _D_neighbor(getNeighborMaterialProperty<Real>("D_neighbor"))
{
}

Real
FluidStructureInterface::computeQpResidual(Moose::DGResidualType type)
{
  Real r = 0;

  switch (type)
  {
    // Implementation 1
    case Moose::Element:
      r = _test[_i][_qp] * _D_neighbor[_qp] * _neighbor_value[_qp];// Element is structure (interchanging enforces flux match)
      break;

    case Moose::Neighbor:
      r = _test_neighbor[_i][_qp] * -_D[_qp] * _u_dotdot[_qp]; // (-ve sign) Neighbor is fluid (interchanging enforces flux match)
      break;

    // Implementation 2
    // case Moose::Element:
    //   r = _test[_i][_qp] * _D_neighbor[_qp] * _neighbor_value[_qp];// Element is structure (interchanging enforces flux match)
    //   break;
    //
    // case Moose::Neighbor:
    //   r = _test_neighbor[_i][_qp] * -_D[_qp] * _grad_u[_qp] * _normals[_qp]; //_test_neighbor[_i][_qp] * -_D[_qp] * _u_dotdot[_qp]; // Neighbor is fluid (interchanging enforces flux match)
    //   break;

    // Implementation 3
    // case Moose::Element:
    //   r = 0; // _test[_i][_qp] * _D_neighbor[_qp] * _neighbor_value[_qp];// Element is structure (interchanging enforces flux match)
    //   break;
    //
    // case Moose::Neighbor:
    //   r = _test_neighbor[_i][_qp] * -_D[_qp] * _grad_u[_qp] * _normals[_qp]; // Neighbor is fluid (interchanging enforces flux match)
    //   break;

  }

  return r;
}

Real
FluidStructureInterface::computeQpJacobian(Moose::DGJacobianType type)
{
  Real jac = 0;
  // std::cout << _phi_neighbor[_j][_qp] << std::endl;

  switch (type)
  {
    // Implementation 1
    case Moose::ElementElement:
      break;

    case Moose::NeighborNeighbor:
      break;

    case Moose::NeighborElement:
      jac = _test_neighbor[_i][_qp] * -_D[_qp] * _du_dotdot_du[_qp];
      break;

    case Moose::ElementNeighbor:
      jac = _test[_i][_qp] * _D_neighbor[_qp] * _phi_neighbor[_j][_qp];
      break;

    // Implementation 2
    // case Moose::ElementElement:
    //   break;
    //
    // case Moose::NeighborNeighbor:
    //   break;
    //
    // case Moose::NeighborElement:
    //   jac = _test_neighbor[_i][_qp] * -_D[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp];//_test_neighbor[_i][_qp] * -_D[_qp] * _du_dotdot_du[_qp];
    //   break;
    //
    // case Moose::ElementNeighbor:
    //   jac = _test[_i][_qp] * _D_neighbor[_qp] * _phi_neighbor[_j][_qp];
    //   break;

    // Implementation 3
    // case Moose::ElementElement:
    //   break;
    //
    // case Moose::NeighborNeighbor:
    //   break;
    //
    // case Moose::NeighborElement:
    //   jac = _test_neighbor[_i][_qp] * -_D[_qp] * _grad_phi_neighbor[_j][_qp] * _normals[_qp];//_test_neighbor[_i][_qp] * -_D[_qp] * _du_dotdot_du[_qp];
    //   break;
    //
    // case Moose::ElementNeighbor:
    //   jac = 0; // _test[_i][_qp] * _D_neighbor[_qp] * _phi_neighbor[_j][_qp];
    //   break;

  }

  return jac;
}
