// Mastodon includes
#include "LinearSoilMaterial.h"

registerMooseObject("MastodonApp", LinearSoilMaterial);

InputParameters
LinearSoilMaterial::validParams()
{
  InputParameters params = Material::validParams();
  params += LayeredMaterialInterface<>::validParams();
  params.setDocString("layer_ids",
                      "Vector of layer ids that map one-to-one "
                      "with the 'shear_modulus' and 'density' "
                      "input parameters.");
  params.addRequiredParam<std::vector<Real>>("shear_modulus",
                                             "Vector of shear modules values that map one-to-one "
                                             "with the number 'layer_ids' parameter.");
  params.addRequiredParam<std::vector<Real>>(
      "density",
      "Vector of density values that map one-to-one with the number "
      "'layer_ids' parameter.");
  params.addClassDescription("Material for computing the shear wave speed "
                             "and minimum element size as a function "
                             "of shear modulus and density.");
  return params;
}

LinearSoilMaterial::LinearSoilMaterial(const InputParameters & parameters)
  : LayeredMaterialInterface<Material>(parameters),
    _shear_wave_speed(declareProperty<Real>("shear_wave_speed")),
    _layer_shear_modulus(getLayerParam<Real>("shear_modulus")),
    _layer_density(getLayerParam<Real>("density"))
{
}

void
LinearSoilMaterial::computeQpProperties()
{
  _shear_wave_speed[_qp] = std::sqrt(_layer_shear_modulus[_qp] / _layer_density[_qp]);
}
