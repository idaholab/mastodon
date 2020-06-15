//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "FluidStructureInterface_3D.h"
#include <iostream>

registerMooseObject("MastodonApp", FluidStructureInterface_3D);

InputParameters
FluidStructureInterface_3D::validParams()
{
  InputParameters params = InterfaceKernel::validParams();
  params.addParam<MaterialPropertyName>("D", "D", "The density.");
  params.addParam<MaterialPropertyName>("D_neighbor", "D_neighbor", "The density.");
  params.addRequiredParam<Real>("component", "The desired component of displacement.");
  return params;
}

FluidStructureInterface_3D::FluidStructureInterface_3D(const InputParameters & parameters)
  : InterfaceKernel(parameters),
    _D(getMaterialProperty<Real>("D")),
    _D_neighbor(getNeighborMaterialProperty<Real>("D_neighbor")),
    _u_dotdot(dotDot()),
    _du_dotdot_du(dotDotDu()),
    _component(getParam<Real>("component"))
{
}

Real
FluidStructureInterface_3D::computeQpResidual(Moose::DGResidualType type)
{
  Real r = 0;
  // std::cout << _normals[_qp] << std::endl;

  if (_component == 0){

    switch (type)
    {

      case Moose::Element:
        r = _test[_i][_qp] * _D_neighbor[_qp] * _neighbor_value[_qp] * _normals[_qp](0); // Element is str
        break;

      case Moose::Neighbor:
        r = _test_neighbor[_i][_qp] * -_D[_qp] * _u_dotdot[_qp] * _normals[_qp](0); // Neighbor is fluid
        break;

    }
  }

  if (_component == 1){
    switch (type)
    {

      case Moose::Element:
        r = _test[_i][_qp] * _D_neighbor[_qp] * _neighbor_value[_qp] * _normals[_qp](1);
        break;

      case Moose::Neighbor:
        r = _test_neighbor[_i][_qp] * -_D[_qp] * _u_dotdot[_qp] * _normals[_qp](1);
        break;

    }
  }

  if (_component == 2){
    switch (type)
    {

      case Moose::Element:
        r = _test[_i][_qp] * _D_neighbor[_qp] * _neighbor_value[_qp] * _normals[_qp](2);
        break;

      case Moose::Neighbor:
        r = _test_neighbor[_i][_qp] * -_D[_qp] * _u_dotdot[_qp] * _normals[_qp](2);
        break;

    }
  }

  return r;
}

Real
FluidStructureInterface_3D::computeQpJacobian(Moose::DGJacobianType type)
{

  Real jac = 0;

  if (_component == 0){
    switch (type)
    {
      case Moose::ElementElement:
        break;

      case Moose::NeighborNeighbor:
        break;

      case Moose::ElementNeighbor:
        jac = _test[_i][_qp] * _D_neighbor[_qp] * _phi_neighbor[_j][_qp] * _normals[_qp](0);
        break;

      case Moose::NeighborElement:
        jac = _test_neighbor[_i][_qp] * -_D[_qp] * _phi[_j][_qp] * _du_dotdot_du[_qp] * _normals[_qp](0);
        break;

    }
  }

  if (_component == 1){
    switch (type)
    {
      case Moose::ElementElement:
        break;

      case Moose::NeighborNeighbor:
        break;

      case Moose::ElementNeighbor:
        jac = _test[_i][_qp] * _D_neighbor[_qp] * _phi_neighbor[_j][_qp] * _normals[_qp](1);
        break;

      case Moose::NeighborElement:
        jac = _test_neighbor[_i][_qp] * -_D[_qp] * _phi[_j][_qp] * _du_dotdot_du[_qp] * _normals[_qp](1);
        break;

    }
  }

  if (_component == 2){
    switch (type)
    {
      case Moose::ElementElement:
        break;

      case Moose::NeighborNeighbor:
        break;

      case Moose::ElementNeighbor:
        jac = _test[_i][_qp] * _D_neighbor[_qp] * _phi_neighbor[_j][_qp] * _normals[_qp](2);
        break;

      case Moose::NeighborElement:
        jac = _test_neighbor[_i][_qp] * -_D[_qp] * _phi[_j][_qp] * _du_dotdot_du[_qp] * _normals[_qp](2);
        break;

    }
  }

  return jac;
}

// Real
// FluidStructureInterface_3D::computeQpResidual(Moose::DGResidualType type)
// {
//   Real r = 0;
//   std::cout << _normals[_qp] << std::endl;
//
//   if (_component == 0){
//
//     switch (type)
//     {
//
//       case Moose::Element:
//         r = _test[_i][_qp] * _D_neighbor[_qp] * _neighbor_value[_qp] * _normals[_qp](0); // Element is str
//         break;
//
//       case Moose::Neighbor:
//         r = _test_neighbor[_i][_qp] * -_D[_qp] * _u_dotdot[_qp] * _normals[_qp](0); // Neighbor is fluid
//         break;
//
//     }
//   }
//
//   if (_component == 1){
//     switch (type)
//     {
//
//       case Moose::Element:
//         r = _test[_i][_qp] * _D_neighbor[_qp] * _neighbor_value[_qp] * _normals[_qp](1);
//         break;
//
//       case Moose::Neighbor:
//         r = _test_neighbor[_i][_qp] * -_D[_qp] * _u_dotdot[_qp] * _normals[_qp](1);
//         break;
//
//     }
//   }
//
//   if (_component == 2){
//     switch (type)
//     {
//
//       case Moose::Element:
//         r = _test[_i][_qp] * _D_neighbor[_qp] * _neighbor_value[_qp] * _normals[_qp](2);
//         break;
//
//       case Moose::Neighbor:
//         r = _test_neighbor[_i][_qp] * -_D[_qp] * _u_dotdot[_qp] * _normals[_qp](2);
//         break;
//
//     }
//   }
//
//   return r;
// }
//
// Real
// FluidStructureInterface_3D::computeQpJacobian(Moose::DGJacobianType type)
// {
//
//   Real jac = 0;
//
//   if (_component == 0){
//     switch (type)
//     {
//       case Moose::ElementElement:
//         break;
//
//       case Moose::NeighborNeighbor:
//         break;
//
//       case Moose::ElementNeighbor:
//         jac = _test[_i][_qp] * _D_neighbor[_qp] * _phi_neighbor[_j][_qp] * _normals[_qp](0);
//         break;
//
//       case Moose::NeighborElement:
//         jac = _test_neighbor[_i][_qp] * -_D[_qp] * _phi[_j][_qp] * _du_dotdot_du[_qp] * _normals[_qp](0);
//         break;
//
//     }
//   }
//
//   if (_component == 1){
//     switch (type)
//     {
//       case Moose::ElementElement:
//         break;
//
//       case Moose::NeighborNeighbor:
//         break;
//
//       case Moose::ElementNeighbor:
//         jac = _test[_i][_qp] * _D_neighbor[_qp] * _phi_neighbor[_j][_qp] * _normals[_qp](1);
//         break;
//
//       case Moose::NeighborElement:
//         jac = _test_neighbor[_i][_qp] * -_D[_qp] * _phi[_j][_qp] * _du_dotdot_du[_qp] * _normals[_qp](1);
//         break;
//
//     }
//   }
//
//   if (_component == 2){
//     switch (type)
//     {
//       case Moose::ElementElement:
//         break;
//
//       case Moose::NeighborNeighbor:
//         break;
//
//       case Moose::ElementNeighbor:
//         jac = _test[_i][_qp] * _D_neighbor[_qp] * _phi_neighbor[_j][_qp] * _normals[_qp](2);
//         break;
//
//       case Moose::NeighborElement:
//         jac = _test_neighbor[_i][_qp] * -_D[_qp] * _phi[_j][_qp] * _du_dotdot_du[_qp] * _normals[_qp](2);
//         break;
//
//     }
//   }
//
//   return jac;
// }
