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

#pragma once

// MOOSE includes
#include "MaterialAuxBase.h"

/**
 * AuxKernel for outputting a DenseMatrix<Real> material property component to an AuxVariable
 */
class MaterialRealCMMAux : public MaterialAuxBase<ColumnMajorMatrix>
{
public:
  static InputParameters validParams();

  /**
   * Class constructor
   * @param parameters The input parameters for this AuxKernel
   */
  MaterialRealCMMAux(const InputParameters & parameters);

protected:
  /// Returns the component of the tensor for output
  virtual Real getRealValue() override;

  /// The row index to output
  unsigned int _row;

  /// The column index to output
  unsigned int _col;
};
