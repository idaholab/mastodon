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
#ifndef SEISMICFORCE_H
#define SEISMICFORCE_H

#include "IntegratedBC.h"

class Function;
class SeismicForce;

template <>
InputParameters validParams<SeismicForce>();

/**
 * SeismicForce applies a pressure on a given boundary in the direction defined
 * by component
 */
class SeismicForce : public IntegratedBC
{
public:
  SeismicForce(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  /// The direction associated with the variable is given in _component.
  const unsigned int _component;

  /// The direction in which the ground velocity is applied.
  const unsigned int _vel_component;

  /// Scaling factor that needs to be applied to the force
  const Real _factor;

  /// alpha Parameter for HHT time integration scheme
  const Real _alpha;

  /// density of the underlying bedrock
  const Real & _density;

  /// P wave speed of the underlying bedrock
  const Real & _p_wave_speed;

  /// shear wave speed of the underlying bedrock
  const Real & _shear_wave_speed;
};

#endif // SEISMICFORCE_H
