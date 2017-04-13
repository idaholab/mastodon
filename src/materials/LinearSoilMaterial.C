// Mastodon includes
#include "LinearSoilMaterial.h"

template <> InputParameters validParams<LinearSoilMaterial>() {
  InputParameters params = validParams<Material>();
  params += validParams<LayeredMaterialInterface<>>();
  params.setDocString("layer_ids", "Vector of layer ids that map one-to-one "
                                   "with the 'shear_modulus' and 'density' "
                                   "input parameters.");
  params.addRequiredParam<std::vector<Real>>(
      "shear_modulus", "Vector of shear modules values that map one-to-one "
                       "with the number 'layer_ids' parameter.");
  params.addRequiredParam<std::vector<Real>>(
      "density", "Vector of density values that map one-to-one with the number "
                 "'layer_ids' parameter.");
  params.addClassDescription("Material for computing the shear wave speed "
                             "($v_s$) and minimum element size as a function "
                             "of shear modules ($G$) and density ($\rho$): "
                             "$v_s = \\sqrt{\frac{G}{\rho}}.$");
  return params;
}

LinearSoilMaterial::LinearSoilMaterial(const InputParameters &parameters)
    : LayeredMaterialInterface<Material>(parameters),
      _shear_wave_speed(declareProperty<Real>("shear_wave_speed")),
      _layer_shear_modulus(getLayerParam<Real>("shear_modulus")),
      _layer_density(getLayerParam<Real>("density")) {}

void LinearSoilMaterial::computeQpProperties() {
  _shear_wave_speed[_qp] =
      std::sqrt(_layer_shear_modulus[_qp] / _layer_density[_qp]);
}
