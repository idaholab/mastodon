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

#ifndef STRESSDIVERGENCEDAMPER_H
#define STRESSDIVERGENCEDAMPER_H

#include "Kernel.h"
#include "ColumnMajorMatrix.h"

// Forward Declarations
class StressDivergenceDamper;

template <>
InputParameters validParams<StressDivergenceDamper>();

class StressDivergenceDamper : public Kernel
{
public:
  StressDivergenceDamper(const InputParameters & parameters);
  virtual void computeResidual() override;
  virtual void computeJacobian() override;
  virtual void computeOffDiagJacobian(MooseVariableFEBase & jvar) override;
  using Kernel::computeOffDiagJacobian;

protected:

  virtual Real computeQpResidual() override { return 0.0; }

  /// direction along which force is calculated
  const unsigned int _component;

  /// number of coupled displacement variables
  unsigned int _ndisp;

  /// variable numbers corresponding to displacement variables
  std::vector<unsigned int> _disp_var;

  /// damper forces in global coordinate system
  const MaterialProperty<ColumnMajorMatrix> & _Fg;

  /// damper stiffness matrix in global coordinate system
  const MaterialProperty<ColumnMajorMatrix> & _Kg;
};

#endif // STRESSDIVERGENCEDAMPER_H
