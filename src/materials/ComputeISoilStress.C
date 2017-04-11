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

// This code was implemented in colloboration with Ozgun Numanoglu (numanog2@illinois.edu) and Omar Baltaji (baltaji2@illinois.edu) from UIUC.

#include "ComputeISoilStress.h"

#include "MooseMesh.h"

template<>
InputParameters validParams<ComputeISoilStress>()
{
  InputParameters params = validParams<ComputeFiniteStrainElasticStress>();
  params.addClassDescription("Compute total stress for the nonlinear material model I-Soil using a set of elastic perfectly plastic stress-strain curves");
  params.addRequiredParam<std::vector<std::vector<Real>>>("yield_stress", "Yield stress for the individual stress-strain curves for each soil layer.");
  params.addParam<Real>("b_exp", "The exponent for pressure dependent stiffness calculation.");
  params.addParam<Real>("a0", "The first coefficient for pressure dependent yield strength calculation.");
  params.addParam<Real>("a1", "The second coefficient for pressure dependent yield strength calculation.");
  params.addParam<Real>("a2", "The third coefficient for pressure dependent yield strength calculation.");
  params.addParam<Real>("tension_pressure_cut_off", "The stiffness of the soil is reduced to zero if the hydrostatic pressure goes below the tension_pressure_cut_off. A negative value for the pressure indicates tensile pressure.");
  params.addParam<std::vector<Real>>("p_ref", "Reference Pressure");
  params.addParam<bool>("pressure_dependency", false, "Set to true to turn on pressure dependent stiffness and yield strength calculation.");
  params.addRequiredParam<std::vector<std::vector<Real>>>("youngs_modulus", "The youngs modulus for the individual stress-strain curves for each soil layer.");
  params.addRequiredParam<std::vector<Real>>("poissons_ratio", "The Poissons's ratio for each soil layer.");
  params.addRequiredParam<std::vector<std::string>>("base_models", "Base name for each elastic perfectly plastic model. All soil layers have the same number of elastic perfectly plastic models so this is not defined for each soil layer.");
  params.addRequiredParam<std::vector<unsigned int>>("layer_ids", "Vector of layer ids that map one-to-one to the rest of the soil layer parameters provided as input.");
  params.addRequiredCoupledVar("layer_variable", "The variable providing the soil layer identification.");
  params.addParam<bool>("wave_speed_calculation", true, "Set to False to turn off P and S wave speed calcualtion.");
  return params;
}

ComputeISoilStress::ComputeISoilStress(const InputParameters & parameters) :
    ComputeFiniteStrainElasticStress(parameters),
    _strain_increment(getMaterialProperty<RankTwoTensor>(_base_name + "strain_increment")),
    _base_models(getParam<std::vector<std::string> >("base_models")),
    _stress_model(_base_models.size()),
    _stress_model_old(_base_models.size()),
    _yield_stress(getParam<std::vector<std::vector<Real>>>("yield_stress")),
    _youngs(getParam<std::vector<std::vector<Real>>>("youngs_modulus")),
    _poissons_ratio(getParam<std::vector<Real>>("poissons_ratio")),
    _b_exp(getParam<Real>("b_exp")),
    _a0(getParam<Real>("a0")),
    _a1(getParam<Real>("a1")),
    _a2(getParam<Real>("a2")),
    _p0(getParam<Real>("tension_pressure_cut_off")),
    _p_ref(getParam<std::vector<Real>>("p_ref")),
    _pressure_dependency(getParam<bool>("pressure_dependency")),
    _layer_ids(getParam<std::vector<unsigned int>>("layer_ids")),
    _strength_pressure_correction(1.0),
    _stiffness_pressure_correction(1.0),
    _soil_layer_variable(coupledValue("layer_variable")),
    _wave_speed_calculation(getParam<bool>("wave_speed_calculation")),
    _shear_wave_speed(_wave_speed_calculation ? &declareProperty<Real>("shear_wave_speed") : NULL),
    _P_wave_speed(_wave_speed_calculation ? &declareProperty<Real>("P_wave_speed") : NULL),
    _density(_wave_speed_calculation ? &getMaterialProperty<Real>("density") : NULL),
    _tangent_modulus(0.0),
    _pos(0)
{
   if ((_youngs.size() != _layer_ids.size()) || (_yield_stress.size() != _layer_ids.size()) || (_poissons_ratio.size() != _layer_ids.size()))
     mooseError("Error in" + name() + ". Young's modulus, yield stress and Poisson's ratio should be of the same size as layer_ids.");

   for (unsigned int i = 0; i < _layer_ids.size(); i++)
   {
     if ((_youngs[i].size() != _yield_stress[i].size()) || (_youngs[i].size() != _base_models.size()))
       mooseError("Error in" + name() + ". Youngs modulus, yield stress and base_models for each soil layer should be of the same size");
   }

   for (unsigned int i = 0; i < _base_models.size(); i++)
   {
     _stress_model[i] = &declareProperty<RankTwoTensor>(_base_models[i] + "_stress_model");
     _stress_model_old[i] = &getMaterialPropertyOld<RankTwoTensor>(_base_models[i] + "_stress_model");
   }

   _stress_new.zero();
   _individual_stress_increment.zero();
   _deviatoric_trial_stress.zero();
}

void
ComputeISoilStress::initQpStatefulProperties()
{
  ComputeStressBase::initQpStatefulProperties();
  for (unsigned int i = 0; i < _base_models.size(); i++)
    (*_stress_model[i])[_qp].zero();

  // Determine the current id for the soil. The variable which is a Real must be converted to a unsigned int for lookup, so first
  // it is rounded to avoid Real values that are just below the desired value.
  _current_id = static_cast<unsigned int>(std::round(_soil_layer_variable[_qp]));
  // Get the position of the current id in the layer_ids array
  _pos = find(_layer_ids.begin(), _layer_ids.end(), _current_id) - _layer_ids.begin();

  if (_wave_speed_calculation)
  {
    Real initial_youngs = 0.0;
    for (unsigned int i = 0; i < _base_models.size(); i++)
      initial_youngs += _youngs[_pos][i];

    // shear wave speed is sqrt(shear_modulus/density)
    (*_shear_wave_speed)[_qp] = std::sqrt(initial_youngs / (2.0 * (1.0 + _poissons_ratio[_pos])) / (*_density)[_qp]);

    // P wave speed is sqrt(P wave modulus/density)
    (*_P_wave_speed)[_qp] = std::sqrt(initial_youngs * (1.0 - _poissons_ratio[_pos]) / (1.0 + _poissons_ratio[_pos]) / (1.0 - 2.0 * _poissons_ratio[_pos]) / (*_density)[_qp]);
  }

  // determine the lateral and vertical stresses
   Real residual_vertical =  _stress[_qp](2,2);
   Real residual_xx = _stress[_qp](0,0);
   Real residual_yy = _stress[_qp](1,1);
   Real mean_stress = _stress[_qp].trace()/(-3.0);

   if (_pressure_dependency)
   {
     _stiffness_pressure_correction = pow((mean_stress - _p0) / _p_ref[_pos], _b_exp);
     _strength_pressure_correction = std::sqrt(_a0+_a1*(mean_stress - _p0)+ _a2*(mean_stress - _p0)*(mean_stress - _p0))/std::sqrt(_a0+_a1*(_p_ref[_pos])+ _a2*(_p_ref[_pos])*(_p_ref[_pos]));
   }

   // Calculate the K0 consistent stress distribution
   RankTwoTensor dev_model;
   for (unsigned int i = 0; i < _base_models.size(); i++)
   {
     Real mean_pressure = 0.0;
     if (residual_vertical != 0.0)
     {
       Real sum_youngs = 0.0;

       for (unsigned int j = i; j < _base_models.size(); j++)
         sum_youngs += _youngs[_pos][j];

      (*_stress_model[i])[_qp](2,2) = residual_vertical * _youngs[_pos][i]/sum_youngs;
      (*_stress_model[i])[_qp](0,0) = residual_xx * _youngs[_pos][i]/sum_youngs;
      (*_stress_model[i])[_qp](1,1) = residual_yy * _youngs[_pos][i]/sum_youngs;
      dev_model =  ((*_stress_model[i])[_qp]).deviatoric() / _youngs[_pos][i];
      mean_pressure = (*_stress_model[i])[_qp].trace() / 3.0;
      Real J2_model = dev_model.doubleContraction(dev_model);
      Real dev_stress_model = std::sqrt(3.0 / 2.0 * J2_model);
      if (dev_stress_model > _yield_stress[_pos][i] * _strength_pressure_correction)
        dev_model *= (_yield_stress[_pos][i] * _strength_pressure_correction) / dev_stress_model;

      (*_stress_model[i])[_qp] = dev_model * _youngs[_pos][i]; // stress_model contains only the deviatoric part of the stress
     }
     residual_vertical = residual_vertical - (*_stress_model[i])[_qp](2,2) - mean_pressure;
     residual_xx = residual_xx - (*_stress_model[i])[_qp](0,0) - mean_pressure;
     residual_yy = residual_yy - (*_stress_model[i])[_qp](1,1) - mean_pressure;
  }
}

void
ComputeISoilStress::computeQpStress()
{
  // Nothing to update during the first time step, return immediately
  if (_t_step == 0)
    return;

  _stress_new.zero();
  computeStress();
  _stress[_qp] = _rotation_increment[_qp] * _stress_new * _rotation_increment[_qp].transpose();

  //Compute dstress_dstrain
  if (_tangent_modulus == 0.0)
    _tangent_modulus = _youngs[_pos][_youngs.size() - 1];

  _Jacobian_mult[_qp] = _elasticity_tensor[_qp] * _tangent_modulus; //This is NOT the exact jacobian
}

void
ComputeISoilStress::computeStress()
{
  if (_t_step == 0)
    return;

  // Determine the current id for the soil. The variable which is a Real must be converted to a unsigned int for lookup, so first
  // it is rounded to avoid Real values that are just below the desired value.
  _current_id = static_cast<unsigned int>(std::round(_soil_layer_variable[_qp]));
  // Get the position of the current id in the layer_ids array
  _pos = find(_layer_ids.begin(), _layer_ids.end(), _current_id) - _layer_ids.begin();

  _individual_stress_increment.zero();
  _deviatoric_trial_stress.zero();
  _tangent_modulus = 0.0;

  // current pressure calculation
  Real mean_stress = _stress_old[_qp].trace() / (-3.0);
  if (mean_stress < _p0)
    mean_stress = _p0;

  if (_pressure_dependency)
  {
    _stiffness_pressure_correction = pow((mean_stress - _p0) / _p_ref[_pos], _b_exp);
    _strength_pressure_correction = std::sqrt(_a0 + _a1 * (mean_stress - _p0) + _a2 * (mean_stress - _p0) * (mean_stress - _p0)) / std::sqrt(_a0 + _a1 * (_p_ref[_pos]) + _a2 * (_p_ref[_pos]) * (_p_ref[_pos]));
  }

  Real mean_pressure = 0.0;
  for (unsigned int i = 0; i < _base_models.size(); i++)
  {
    // compute trial stress increment - note that _elasticity_tensor here assumes youngs modulus = 1
    _individual_stress_increment = _elasticity_tensor[_qp] * (_strain_increment[_qp]);

    // calculate pressure for each element
    mean_pressure += _individual_stress_increment.trace() / 3.0 * _youngs[_pos][i] * _stiffness_pressure_correction;

    // compute the deviatoric trial stress normalized by non pressure dependent youngs modulus.
    _deviatoric_trial_stress = _individual_stress_increment.deviatoric() * _stiffness_pressure_correction + (*_stress_model_old[i])[_qp] / (_youngs[_pos][i]);

    // compute the effective trial stress
    Real dev_trial_stress_squared = _deviatoric_trial_stress.doubleContraction(_deviatoric_trial_stress);
    Real effective_trial_stress = std::sqrt(3.0 / 2.0 * dev_trial_stress_squared);

    // check yield condition and calculate plastic strain
    Real yield_condition = effective_trial_stress - _yield_stress[_pos][i] * _strength_pressure_correction;

    if (yield_condition > 0.0)
      _deviatoric_trial_stress *= _yield_stress[_pos][i] * _strength_pressure_correction / effective_trial_stress;
    else
      _tangent_modulus += _youngs[_pos][i];

    (*_stress_model[i])[_qp] = _youngs[_pos][i] * (_deviatoric_trial_stress);

    _stress_new += (*_stress_model[i])[_qp];
  }
  _stress_new(0,0) += mean_pressure - mean_stress;
  _stress_new(1,1) += mean_pressure - mean_stress;
  _stress_new(2,2) += mean_pressure - mean_stress;

  _tangent_modulus *= _stiffness_pressure_correction;
}
