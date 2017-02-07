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
#ifndef ComputeIsotropicElasticityTensorSoil_H
#define ComputeIsotropicElasticityTensorSoil_H

// MOOSE includes
#include "ComputeElasticityTensorBase.h"

/**
 * This material defines elasticity tensor for isotropic linear elastic
 * layer soil material and also calculates the material properties shear and P
 * wave velocities.
 */
class ComputeIsotropicElasticityTensorSoil : public ComputeElasticityTensorBase
{
public:
  ComputeIsotropicElasticityTensorSoil(const InputParameters & parameters);

protected:

  /**
   * Populates the map of solid layer id to shear modules and density.
   */
  virtual void initialSetup() override;

  virtual void computeQpElasticityTensor() override;

  /// The coupled variable field providing the layer id
  const VariableValue & _soil_layer_variable;

  /// Flag to turn on/off P and S wave speed calculation.
  bool _wave_speed_calculation;

  /// Shear Modules and Density mapping to soil id.
  // This is being done in a single map to avoid looking up the id multipe times.
  std::map<unsigned int, std::pair<Real, Real>> _soil_id_to_shear_modulus_and_density;

  /// Young's Modules and Density mapping to soil id.
  // This is being done in a single map to avoid looking up the id multipe times.
  std::map<unsigned int, std::pair<Real, Real>> _soil_id_to_youngs_modulus_and_density;

  /// Computed shear wave speed.
  MaterialProperty<Real> * _shear_wave_speed;

  /// Computed P wave speed.
  MaterialProperty<Real> * _P_wave_speed;

  /// Density stored as a material property.
  MaterialProperty<Real> & _density;

  /// The id to be used for looking up the shear modulus and density, it is declared once here and re-used rather
  /// than creating a temporary variable in computeQpProperties method, simply for efficiency.
  unsigned int _current_id;

  /// individual elasticity tensor
  RankFourTensor _Cijkl;
};

#endif // COMPUTEISOTROPICELASTICITYTENSORSOIL_H
