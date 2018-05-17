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

#include "TestLayeredMaterialInterface.h"

registerMooseObject("MastodonTestApp", TestLayeredMaterialInterfaceDocString);
registerMooseObject("MastodonTestApp", TestLayeredMaterialInterfaceTypeError);

template <>
InputParameters
validParams<TestLayeredMaterialInterfaceDocString>()
{
  InputParameters params = validParams<Material>();
  params += validParams<LayeredMaterialInterface<>>();
  return params;
}

template <>
InputParameters
validParams<TestLayeredMaterialInterfaceKernel>()
{
  return validParams<Kernel>();
}

template <>
InputParameters
validParams<TestLayeredMaterialInterfaceTypeError>()
{
  InputParameters params = validParams<TestLayeredMaterialInterfaceKernel>();
  params += validParams<LayeredMaterialInterface<>>();
  return params;
}
