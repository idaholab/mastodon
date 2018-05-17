/*************************************************/
/*           DO NOT MODIFY THIS HEADER           */
/*                                               */
/*                     MASTODON                  */
/*                                               */
/*    (c) 2015 Battelle Energy Alliance, LLC     */
/*            ALL RIGHTS RESERVED                */
/*                                               */
/*   Prepared by Battelle Energy Alliance, LLC   */
/*     With the U. S. Department of Energy       */
/*                                               */
/*     See COPYRIGHT for full restrictions       */
/*************************************************/

#ifndef TESTLAYEREDMATERIALINTERFACE_H
#define TESTLAYEREDMATERIALINTERFACE_H

// MOOSE includes
#include "Kernel.h"
#include "Material.h"

// Mastodon includes
#include "LayeredMaterialInterface.h"

/**
 * Test object for checking setDocString error of LayeredMaterialInterface
 */
class TestLayeredMaterialInterfaceDocString : public LayeredMaterialInterface<Material>
{
public:
  TestLayeredMaterialInterfaceDocString(const InputParameters & parameters)
    : LayeredMaterialInterface(parameters)
  {
  }

protected:
  virtual void computeQpProperties() override {}
};

template <>
InputParameters validParams<TestLayeredMaterialInterfaceDocString>();

/**
 * Test object needed to test Material type of LayeredMaterialInterface
 */
class TestLayeredMaterialInterfaceKernel : public Kernel
{
public:
  TestLayeredMaterialInterfaceKernel(const InputParameters & parameters) : Kernel(parameters) {}

protected:
  virtual void computeProperties() {}
};

template <>
InputParameters validParams<TestLayeredMaterialInterfaceKernel>();

/**
 * Test object for checking Material type check error of LayeredMaterialInterface
 */
class TestLayeredMaterialInterfaceTypeError
  : public LayeredMaterialInterface<TestLayeredMaterialInterfaceKernel>
{
public:
  TestLayeredMaterialInterfaceTypeError(const InputParameters & parameters)
    : LayeredMaterialInterface(parameters)
  {
  }
  virtual Real computeQpResidual() override { return 0.0; }
};

template <>
InputParameters validParams<TestLayeredMaterialInterfaceTypeError>();

#endif
