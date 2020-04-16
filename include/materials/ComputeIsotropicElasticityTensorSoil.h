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
template <bool is_ad>
class ComputeIsotropicElasticityTensorSoilTempl
  : public LayeredMaterialInterface<ComputeElasticityTensorBaseTempl<is_ad>>
{
public:
  ComputeIsotropicElasticityTensorSoilTempl(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void computeQpElasticityTensor() override;

  /// Computed P wave modules
  Real _P_wave_modulus = 0;

  std::vector<Real> _input_shear_modulus;

  /// Layer parameter for the "shear_modulus" input
  const MooseArray<Real> & _layer_shear_modulus;

  /// Layer parameter for the "density" input
  const MooseArray<Real> & _layer_density;

  /// Layer parameter for the "poissons_ratio" input
  const MooseArray<Real> & _layer_poissons_ratio;

  /// Flag to turn on/off P and S wave speed calculation.
  bool _wave_speed_calculation;

  /// Computed shear wave speed.
  GenericMaterialProperty<Real, is_ad> * _shear_wave_speed;

  /// Computed P wave speed.
  GenericMaterialProperty<Real, is_ad> * _P_wave_speed;

  /// Density stored as a material property.
  GenericMaterialProperty<Real, is_ad> & _density;

  /// Density scale factor
  const Real & _scale_density;

  /// individual elasticity tensor
  RankFourTensor _Cijkl;

  /// Effective stiffness of the element: function of material properties
  Real _effective_stiffness_local;

  using LayeredMaterialInterface<ComputeElasticityTensorBaseTempl<is_ad>>::issueGuarantee;
  using LayeredMaterialInterface<ComputeElasticityTensorBaseTempl<is_ad>>::_elasticity_tensor_name;
  using LayeredMaterialInterface<ComputeElasticityTensorBaseTempl<is_ad>>::_elasticity_tensor;
  using LayeredMaterialInterface<ComputeElasticityTensorBaseTempl<is_ad>>::_qp;
  using LayeredMaterialInterface<ComputeElasticityTensorBaseTempl<is_ad>>::name;
  using LayeredMaterialInterface<ComputeElasticityTensorBaseTempl<is_ad>>::_effective_stiffness;
};

typedef ComputeIsotropicElasticityTensorSoilTempl<false> ComputeIsotropicElasticityTensorSoil;
typedef ComputeIsotropicElasticityTensorSoilTempl<true> ADComputeIsotropicElasticityTensorSoil;

#endif // COMPUTEISOTROPICELASTICITYTENSORSOIL_H
