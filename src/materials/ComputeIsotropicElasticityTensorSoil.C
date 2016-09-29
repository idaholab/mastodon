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
#include "ComputeIsotropicElasticityTensorSoil.h"

/**
 * This class defines the elasticity tensor for a layered linear isotropic soil material.
 * Additionally this class calculates and stores the shear wave speed and P wave speed as material properties.
 **/
template<>
InputParameters validParams<ComputeIsotropicElasticityTensorSoil>()
{
  InputParameters params = validParams<ComputeElasticityTensorBase>();
  params.addClassDescription("Compute an isotropic elasticity tensor for a layered soil material when shear modulus, poisson's ratio and density are provided as input for each layer.");
  params.addRequiredParam<std::vector<unsigned int>>("layer_ids", "Vector of layer ids that map one-to-one with the 'shear_modulus', 'poissons_ratio' and 'density' input parameters.");
  params.addRequiredParam<std::vector<Real>>("poissons_ratio", "Vector of Poisson's ratio values that map one-to-one with the number 'layer_ids' parameter.");
  params.addRequiredParam<std::vector<Real>>("shear_modulus", "Vector of  shear modulus values that map one-to-one with the number 'layer_ids' parameter.");
  params.addRequiredParam<std::vector<Real>>("density", "Vector of density values that map one-to-one with the number 'layer_ids' parameter.");
  params.addRequiredCoupledVar("layer_variable", "The variable providing the soil layer identification.");
  return params;
}

ComputeIsotropicElasticityTensorSoil::ComputeIsotropicElasticityTensorSoil(const InputParameters & parameters) :
    ComputeElasticityTensorBase(parameters),
    _soil_layer_variable(coupledValue("layer_variable")),
    _shear_wave_speed(declareProperty<Real>("shear_wave_speed")),
    _P_wave_speed(declareProperty<Real>("P_wave_speed")),
    _density(declareProperty<Real>("density"))
{
  std::vector<Real> iso_const(2);
  //Fill elasticity tensor
  _Cijkl.fillFromInputVector(iso_const, RankFourTensor::symmetric_isotropic);
}

void
ComputeIsotropicElasticityTensorSoil::initialSetup()
{
  // Get the vectors for poulating the id to the modules/density map
  const std::vector<unsigned int> & layers = getParam<std::vector<unsigned int>>("layer_ids");
  const std::vector<Real> & modulus = getParam<std::vector<Real>>("shear_modulus");
  const std::vector<Real> & density = getParam<std::vector<Real>>("density");
  const std::vector<Real> & poissons_ratio = getParam<std::vector<Real>>("poissons_ratio");

  // Check that the vectors are all the same size
  if (layers.size() != modulus.size() || layers.size() != density.size() || modulus.size() != density.size() || modulus.size() != poissons_ratio.size())
    mooseError("The 'layer_ids', 'shear_modulus', 'poissons_ratio' and 'density' vectors must be the same size.");

  // Build the map
  for (unsigned int i = 0; i < layers.size(); ++i)
  {
    _soil_id_to_shear_modulus_and_density[layers[i]] = std::pair<Real, Real>(modulus[i], density[i]);
    _soil_id_to_youngs_modulus_and_density[layers[i]] = std::pair<Real, Real>(modulus[i] * 2.0* (1.0 + poissons_ratio[i]), density[i]);
  }
}

void
ComputeIsotropicElasticityTensorSoil::computeQpElasticityTensor()
{
  // Determine the current id. The variable which is a Real must be converted to a unsigned int for lookup, so first
  // it is rounded to avoid Real values that are just below the desired value.
  _current_id = static_cast<unsigned int>(std::round(_soil_layer_variable[_qp]));

  // Locate the shear modulus and density values for the current soil id and error if it is not found
  std::map<unsigned int, std::pair<Real, Real>>::iterator it_s = _soil_id_to_shear_modulus_and_density.find(_current_id);
  if (it_s == _soil_id_to_shear_modulus_and_density.end())
    mooseError("The soil id variable value (" << _current_id << ") was not provided in the 'layer_ids' parameter.");

  // Locate the Young's modulus and density values for the current soil id and error if it is not found
  std::map<unsigned int, std::pair<Real, Real>>::iterator it_p = _soil_id_to_youngs_modulus_and_density.find(_current_id);
  if (it_p == _soil_id_to_youngs_modulus_and_density.end())
    mooseError("The soil id variable value (" << _current_id << ") was not provided in the 'layer_ids' parameter.");

  // Shear wave speed: sqrt(G/rho)
  _shear_wave_speed[_qp] = std::sqrt(it_s->second.first / it_s->second.second);

  // P wave speed: sqrt(E/rho)
  _P_wave_speed[_qp] = std::sqrt(it_p->second.first / it_p->second.second);

  _density[_qp] = it_p->second.second;

  std::vector<Real> iso_const(2);
  iso_const[0] = it_s->second.first * (it_p->second.first - 2.0 * it_s->second.first) / (3.0 * it_s->second.first - it_p->second.first); // lambda = G*(E-2G)/(3G-E)
  iso_const[1] = it_s->second.first; // shear modulus
  //Fill elasticity tensor
  _Cijkl.fillFromInputVector(iso_const, RankFourTensor::symmetric_isotropic);

  //Assign elasticity tensor at a given quad point
  _elasticity_tensor[_qp] = _Cijkl;
}

