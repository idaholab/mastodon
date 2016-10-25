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
#ifndef NONREFLECTINGBC_H
#define NONREFLECTINGBC_H

#include "IntegratedBC.h"

class NonReflectingBC;

template<>
InputParameters validParams<NonReflectingBC>();

/**
 * NonReflecting BC  applies a Lysmer damper  on a given boundary in the normal and tangential directions
 */

class NonReflectingBC : public IntegratedBC
{
public:

  NonReflectingBC(const InputParameters & parameters);

protected:

  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Direction in which the Lysmer damper is applied
  unsigned int _component;

  /// Number of displacement variables
  unsigned int _ndisp;

  /// Vector of displacement variables
  std::vector<const VariableValue *> _disp;

  /// Unsigned integers representing the displacement variables
  std::vector<unsigned int> _disp_var;

  /// Vector of old displacement variables
  std::vector<const VariableValue *> _disp_old;

  /// Vector of old velocity variables
  std::vector<const VariableValue *> _vel_old;

  /// Vector of old acceleration variables
  std::vector<const VariableValue *> _accel_old;

  /// _beta Parameter for Newmark time integration scheme
  const Real _beta;

  /// _gamma Parameter for Newmark time integration scheme
  const Real _gamma;

  /// _alpha Parameter for HHT time integration scheme
  const Real _alpha;

  /// Density of the soil
  const MaterialProperty<Real> & _density;

  /// Shear wave speed of the soil
  const MaterialProperty<Real> & _shear_wave_speed;

  /// P wave speed of the soil
  const MaterialProperty<Real> & _P_wave_speed;
};

#endif //NONREFLECTINGBC_H
