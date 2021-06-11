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

#include "MaterialRealCMMAux.h"

registerMooseObject("MastodonApp", MaterialRealCMMAux);

defineLegacyParams(MaterialRealCMMAux);

InputParameters
MaterialRealCMMAux::validParams()
{
  InputParameters params = MaterialAuxBase<>::validParams();
  params.addClassDescription(
      "Populate an auxiliary variable with an entry from a ColumnMajorMatrix material property.");
  params.addParam<unsigned int>("row", 0, "The row component to consider for this kernel");
  params.addParam<unsigned int>("column", 0, "The column component to consider for this kernel");
  return params;
}

MaterialRealCMMAux::MaterialRealCMMAux(const InputParameters & parameters)
  : MaterialAuxBase<ColumnMajorMatrix>(parameters),
    _row(getParam<unsigned int>("row")),
    _col(getParam<unsigned int>("column"))
{
}

Real
MaterialRealCMMAux::getRealValue()
{
  if (_row >= _prop[_qp].n() || _col >= _prop[_qp].m())
    mooseError("In ", name(), " the row or column parameter is out of bounds.");
  return _prop[_qp](_row, _col);
}
