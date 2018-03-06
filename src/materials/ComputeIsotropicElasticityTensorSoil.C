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

template <>
InputParameters
validParams<ComputeIsotropicElasticityTensorSoil>()
{
  InputParameters params = validParams<ComputeElasticityTensorBase>();
  params += validParams<LayeredMaterialInterface<>>();
  params.addClassDescription("Compute an isotropic elasticity tensor for a "
                             "layered soil material when shear modulus or elastic modulus, "
                             "poisson's ratio and density are provided as "
                             "input for each layer.");
  params.setDocString("layer_ids",
                      "Vector of layer ids that map one-to-one "
                      "with the 'shear_modulus' or 'elastic_modulus', 'poissons_ratio' "
                      "and 'density' input parameters.");
  params.addRequiredParam<std::vector<Real>>("poissons_ratio",
                                             "Vector of Poisson's ratio values that map one-to-one "
                                             "with the number 'layer_ids' parameter.");
  params.addParam<std::vector<Real>>("shear_modulus",
                                     "Vector of shear modulus values that map one-to-one "
                                     "with the number 'layer_ids' parameter.");
  params.addParam<std::vector<Real>>("elastic_modulus",
                                     "Vector of elastic modulus values that map one-to-one "
                                     "with the number 'layer_ids' parameter.");
  params.addRequiredParam<std::vector<Real>>(
      "density",
      "Vector of density values that map one-to-one with the number "
      "'layer_ids' parameter.");
  params.addParam<bool>(
      "wave_speed_calculation", true, "Set to False to turn off P and S wave speed calculation.");
  // Controlled scale parameters (temporary; TODO: remove when #107 is resolved.)
  params.addParam<Real>("scale_factor_density", 1.0, "Scale factor for density.");
  params.declareControllable("scale_factor_density");
  return params;
}

const MooseArray<Real> &
getShearModulus(ComputeIsotropicElasticityTensorSoil * object,
                std::vector<Real> & shear_modulus,
                std::string name)
{
  if (object->isParamValid("shear_modulus") && object->isParamValid("elastic_modulus"))
    mooseError("In block " + name + ". Please provide ONE of the parameters, 'shear_modulus' and "
                                    "'elastic_modulus', but not both.");
  if (!object->isParamValid("shear_modulus") && !object->isParamValid("elastic_modulus"))
    mooseError("In block " + name +
               ". Please provide ONE of the parameters, 'shear_modulus' or 'elastic_modulus'.");
  if (object->isParamValid("shear_modulus"))
    return object->getLayerParam<Real>("shear_modulus");
  else
  {
    const std::vector<Real> & elastic_modulus =
        object->getParam<std::vector<Real>>("elastic_modulus");
    const std::vector<Real> & poissons_ratio =
        object->getParam<std::vector<Real>>("poissons_ratio");
    shear_modulus.resize(elastic_modulus.size());
    for (std::size_t i = 0; i < shear_modulus.size(); ++i)
      shear_modulus[i] = elastic_modulus[i] / (2 * (1 + poissons_ratio[i]));
    return object->addLayerVector(shear_modulus);
  }
}

ComputeIsotropicElasticityTensorSoil::ComputeIsotropicElasticityTensorSoil(
    const InputParameters & parameters)
  : LayeredMaterialInterface(parameters),
    _input_shear_modulus(),
    _layer_shear_modulus(getShearModulus(this, _input_shear_modulus, name())),
    _layer_density(getLayerParam<Real>("density")),
    _layer_poissons_ratio(getLayerParam<Real>("poissons_ratio")),
    _wave_speed_calculation(getParam<bool>("wave_speed_calculation")),
    _shear_wave_speed(_wave_speed_calculation ? &declareProperty<Real>("shear_wave_speed") : NULL),
    _P_wave_speed(_wave_speed_calculation ? &declareProperty<Real>("P_wave_speed") : NULL),
    _density(declareProperty<Real>("density")),
    _scale_density(getParam<Real>("scale_factor_density"))
{
  // all tensors created by this class are always isotropic
  issueGuarantee(_elasticity_tensor_name, Guarantee::ISOTROPIC);

  // all tensors created by this class are always constant in time
  issueGuarantee(_elasticity_tensor_name, Guarantee::CONSTANT_IN_TIME);

  std::vector<Real> iso_const(2);
  // Fill elasticity tensor
  _Cijkl.fillFromInputVector(iso_const, RankFourTensor::symmetric_isotropic);
}

void
ComputeIsotropicElasticityTensorSoil::computeQpElasticityTensor()
{
  _P_wave_modulus = _layer_shear_modulus[_qp] * 2.0 * (1.0 - _layer_poissons_ratio[_qp]) /
                    (1.0 - 2.0 * _layer_poissons_ratio[_qp]);

  _density[_qp] = _layer_density[_qp] * _scale_density;

  if (_wave_speed_calculation)
  {
    // Shear wave speed: sqrt(G/rho)
    (*_shear_wave_speed)[_qp] = std::sqrt(_layer_shear_modulus[_qp] / _density[_qp]);

    // P wave speed: sqrt(M/rho)
    (*_P_wave_speed)[_qp] = std::sqrt(_P_wave_modulus / _density[_qp]);
  }

  std::vector<Real> iso_const(2);
  iso_const[0] = _P_wave_modulus - 2.0 * _layer_shear_modulus[_qp]; // lambda = M - 2G
  iso_const[1] = _layer_shear_modulus[_qp];                         // shear modulus

  // Fill elasticity tensor
  _Cijkl.fillFromInputVector(iso_const, RankFourTensor::symmetric_isotropic);

  // Assign elasticity tensor at a given quad point
  _elasticity_tensor[_qp] = _Cijkl;
}
