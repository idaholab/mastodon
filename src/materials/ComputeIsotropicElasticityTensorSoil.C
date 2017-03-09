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

template<>
InputParameters validParams<ComputeIsotropicElasticityTensorSoil>()
{
  InputParameters params = validParams<ComputeElasticityTensorBase>();
  params += validParams<LayeredMaterialInterface<>>();
  params.addClassDescription("Compute an isotropic elasticity tensor for a layered soil material when shear modulus, poisson's ratio and density are provided as input for each layer.");
  params.setDocString("layer_ids", "Vector of layer ids that map one-to-one with the 'shear_modulus', 'poissons_ratio' and 'density' input parameters.");
  params.addRequiredParam<std::vector<Real>>("poissons_ratio", "Vector of Poisson's ratio values that map one-to-one with the number 'layer_ids' parameter.");
  params.addRequiredParam<std::vector<Real>>("shear_modulus", "Vector of  shear modulus values that map one-to-one with the number 'layer_ids' parameter.");
  params.addRequiredParam<std::vector<Real>>("density", "Vector of density values that map one-to-one with the number 'layer_ids' parameter.");
  params.addParam<bool>("wave_speed_calculation", true, "Set to False to turn off P and S wave speed calcualtion.");
  return params;
}

ComputeIsotropicElasticityTensorSoil::ComputeIsotropicElasticityTensorSoil(const InputParameters & parameters) :
    LayeredMaterialInterface(parameters),
    _layer_shear_modulus(getLayerParam<Real>("shear_modulus")),
    _layer_density(getLayerParam<Real>("density")),
    _layer_poissons_ratio(getLayerParam<Real>("poissons_ratio")),
    _wave_speed_calculation(getParam<bool>("wave_speed_calculation")),
    _shear_wave_speed(_wave_speed_calculation ? &declareProperty<Real>("shear_wave_speed") : NULL),
    _P_wave_speed(_wave_speed_calculation ? &declareProperty<Real>("P_wave_speed") : NULL),
    _density(declareProperty<Real>("density"))
{
  std::vector<Real> iso_const(2);
  //Fill elasticity tensor
  _Cijkl.fillFromInputVector(iso_const, RankFourTensor::symmetric_isotropic);
}

void
ComputeIsotropicElasticityTensorSoil::computeQpElasticityTensor()
{

  _P_wave_modulus = _layer_shear_modulus[_qp] * 2.0* (1.0 - _layer_poissons_ratio[_qp]) / (1.0 - 2.0 * _layer_poissons_ratio[_qp]);

  _density[_qp] = _layer_density[_qp];

  if (_wave_speed_calculation)
  {
    // Shear wave speed: sqrt(G/rho)
    (*_shear_wave_speed)[_qp] = std::sqrt(_layer_shear_modulus[_qp]/ _density[_qp]);

    // P wave speed: sqrt(M/rho)
    (*_P_wave_speed)[_qp] = std::sqrt(_P_wave_modulus / _density[_qp]);
  }

  std::vector<Real> iso_const(2);
  iso_const[0] = _P_wave_modulus - 2.0 * _layer_shear_modulus[_qp]; // lambda = M - 2G
  iso_const[1] = _layer_shear_modulus[_qp]; // shear modulus

  //Fill elasticity tensor
  _Cijkl.fillFromInputVector(iso_const, RankFourTensor::symmetric_isotropic);

  //Assign elasticity tensor at a given quad point
  _elasticity_tensor[_qp] = _Cijkl;
}
