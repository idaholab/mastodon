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

#ifndef STRESSDIVERGENCEISOLATOR_H
#define STRESSDIVERGENCEISOLATOR_H

#include "Kernel.h"
#include "ColumnMajorMatrix.h"

class StressDivergenceIsolator : public Kernel
{
public:
  static InputParameters validParams();
  StressDivergenceIsolator(const InputParameters & parameters);
  virtual void computeResidual() override;
  virtual void computeJacobian() override;
  virtual void computeOffDiagJacobian(unsigned int jvar) override;

protected:
  virtual Real computeQpResidual() override { return 0.0; }

  /// Direction along which force/moment is calculated
  const unsigned int _component;

  /// Number of coupled displacement variables
  unsigned int _ndisp;

  /// Variable numbers corresponding to displacement variables
  std::vector<unsigned int> _disp_var;

  /// Number of coupled rotational variables
  unsigned int _nrot;

  /// Variable numbers corresponding to rotational variables
  std::vector<unsigned int> _rot_var;

  /// Global isolator forces
  const MaterialProperty<ColumnMajorMatrix> & _Fg;

  /// GLobal stiffness matrix
  const MaterialProperty<ColumnMajorMatrix> & _Kg;

  /// Stiffness proportional Rayleigh damping parameter
  const Real & _zeta;

  /// HHT time integration parameter
  const Real & _alpha;

  /// Boolean flag to turn on Rayleigh damping or numerical damping due to HHT time integration
  const bool _isDamped;

  /// Old global isolator forces
  const MaterialProperty<ColumnMajorMatrix> * _Fg_old;

  /// Older global isolator forces
  const MaterialProperty<ColumnMajorMatrix> * _Fg_older;

  /// Static initialization
  const bool _static_initialization;
};

#endif // STRESSDIVERGENCEISOLATOR_H
