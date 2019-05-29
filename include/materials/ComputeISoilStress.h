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

// This code was implemented in colloboration with Ozgun Numanoglu
// (numanog2@illinois.edu) and Omar Baltaji (baltaji2@illinois.edu) from UIUC.

/**
 * This material class calculates the total stress for the  nonlinear hysteretic
 * soil model I-Soil.
 **/
#ifndef COMPUTEISOILSTRESS_H
#define COMPUTEISOILSTRESS_H

#include "ComputeFiniteStrainElasticStress.h"

class ComputeISoilStress;

template <>
InputParameters validParams<ComputeISoilStress>();

class ComputeISoilStress : public ComputeFiniteStrainElasticStress
{
public:
  ComputeISoilStress(const InputParameters & parameters);

protected:
  virtual void computeQpStress() override;

  /// Calculates the total stress as a sum of the stresses from the individual
  /// elastic-perfectly plastic stress strain curves.
  virtual void computeStress();

  virtual void initQpStatefulProperties() override;

  /// Strain_increment for the current time step.
  const MaterialProperty<RankTwoTensor> & _strain_increment;

  /// Base names given to each elastic perfectly plastic curve.
  std::vector<std::string> _base_models;

  /// Material property which stores the current stress for each elastic
  /// perfectly plastic curve.
  std::vector<MaterialProperty<RankTwoTensor> *> _stress_model;

  /// Material property which stores the old stress for each elastic perfectly
  /// plastic curve.
  std::vector<const MaterialProperty<RankTwoTensor> *> _stress_model_old;

  /// Yield stress for each elastic perfectly plastic curve for each soil layer.
  std::vector<std::vector<Real>> _yield_stress;

  /// Young's modulus for each elastic perfectly plastic curve for each soil
  /// layer.
  std::vector<std::vector<Real>> _youngs;

  /// The coupled variable field providing the soil layer id.
  const VariableValue & _soil_layer_variable;

  /// The vector of soil layer ids.
  std::vector<unsigned int> _layer_ids;

  /// Flag to turn on/off P and S wave speed calculation
  bool _wave_speed_calculation;

  /// Poisson's ratio for each soil layer.
  std::vector<Real> _poissons_ratio;

  /// Density stored as a material property.
  const MaterialProperty<Real> * _density;

  /// Exponential parameter used in pressure dependent stiffness calcualtion.
  Real _b_exp;

  /// Reference pressure at which the soil properties are defined for each soil
  /// layer.
  std::vector<Real> _p_ref;

  /// The first coefficient used in pressure dependent yield strength
  /// calculation.
  Real _a0;

  /// The second coefficient used in pressure dependent yield strength
  /// calculation.
  Real _a1;

  /// The third coefficient used in pressure dependent yield strength
  /// calculation.
  Real _a2;

  /// Tension pressure cut off for the soil below which the soil loses stiffness
  /// in tension.
  Real _p0;

  /// Flag to turn on pressure dependent stiffness and yield strength
  /// calculation.
  bool _pressure_dependency;

  /// The multiplicative factor for pressure dependent yield strength
  /// correction.
  Real _strength_pressure_correction;

  /// The multiplicative factor for pressure dependent stiffness correction.
  Real _stiffness_pressure_correction;

  /// Computed shear wave speed.
  MaterialProperty<Real> * _shear_wave_speed;

  /// Computed P wave speed.
  MaterialProperty<Real> * _P_wave_speed;

  /// Tangent modulus for each soil layer.
  Real _tangent_modulus;

  /// The position of the current layer id in the vector layer_ids.
  int _pos;

  /// Whether initial stress was provided
  const bool _initial_soil_stress_provided;

  /// stress_new used in stress calcualtion.
  RankTwoTensor _stress_new;

  /// The stress increment for one elastic perfectly plastic curve.
  RankTwoTensor _individual_stress_increment;

  /// The deviatoric part of the trial stress for one elastic perfectly plastic
  /// curve.
  RankTwoTensor _deviatoric_trial_stress;

  /// The id to be used for looking up the shear modulus and density, it is
  /// declared once here and re-used rather
  /// than creating a temporary variable in computeQpProperties method, simply
  /// for effeciency.
  unsigned int _current_id;

  /// initial stress components
  std::vector<const Function *> _initial_soil_stress;
};

#endif // COMPUTEISOILSTRESS_H
