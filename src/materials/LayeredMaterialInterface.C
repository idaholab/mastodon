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

// Mastodon includes
#include "LayeredMaterialInterface.h"

template<>
InputParameters validParams<LayeredMaterialInterface<>>()
{
  InputParameters params = emptyInputParameters();
  params.addRequiredParam<std::vector<Real>>("layer_ids", "Vector of layer ids that maps one-to-one with the layered input parameters. [This should be modified in the parent classes validParam function using setDocString to include information on the parameters which the layer ids correspond.]");
  params.addRequiredCoupledVar("layer_variable", "The variable providing the soil layer identification.");
  return params;
}
