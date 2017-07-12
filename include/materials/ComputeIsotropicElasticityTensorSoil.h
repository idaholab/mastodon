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
#ifndef COMPUTEISOTROPICELASTICITYTENSORSOIL_H
#define COMPUTEISOTROPICELASTICITYTENSORSOIL_H

// MOOSE includes
#include "ComputeElasticityTensorBase.h"

// Mastodon includes
#include "LayeredMaterialInterface.h"

/**
 * This material defines elasticity tensor for isotropic linear elastic
 * layer soil material and also calculates the material properties shear and P
 * wave velocities.
 */
class ComputeIsotropicElasticityTensorSoil
    : public LayeredMaterialInterface<ComputeElasticityTensorBase>
{
public:
  ComputeIsotropicElasticityTensorSoil(const InputParameters & parameters);

protected:
  virtual void computeQpElasticityTensor() override;

  /// Computed P wave modules
  Real _P_wave_modulus = 0;

  /// Layer parameter for the "shear_modulus" input
  const MooseArray<Real> & _layer_shear_modulus;

  /// Layer parameter for the "density" input
  const MooseArray<Real> & _layer_density;

  /// Layer parameter for the "poissons_ratio" input
  const MooseArray<Real> & _layer_poissons_ratio;

  /// Flag to turn on/off P and S wave speed calculation.
  bool _wave_speed_calculation;

  /// Computed shear wave speed.
  MaterialProperty<Real> * _shear_wave_speed;

  /// Computed P wave speed.
  MaterialProperty<Real> * _P_wave_speed;

  /// Density stored as a material property.
  MaterialProperty<Real> & _density;

  /// individual elasticity tensor
  RankFourTensor _Cijkl;
};

#endif // COMPUTEISOTROPICELASTICITYTENSORSOIL_H
