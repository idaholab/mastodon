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

InputParameters
TestLayeredMaterialInterfaceDocString::validParams()
{
  InputParameters params = Material::validParams();
  params += LayeredMaterialInterface<>::validParams();
  return params;
}

InputParameters
TestLayeredMaterialInterfaceKernel::validParams()
{
  return Kernel::validParams();
}

InputParameters
TestLayeredMaterialInterfaceTypeError::validParams()
{
  InputParameters params = TestLayeredMaterialInterfaceKernel::validParams();
  params += LayeredMaterialInterface<>::validParams();
  return params;
}
