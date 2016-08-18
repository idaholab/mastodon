// Mastodon includes
#include "LinearSoilMaterial.h"

template<>
InputParameters validParams<LinearSoilMaterial>()
{
  InputParameters params = validParams<Material>();
  params.addRequiredParam<std::vector<unsigned int>>("layer_ids", "Vector of layer ids that map one-to-one with the 'shear_modulus' and 'density' input parameters.");
  params.addRequiredParam<std::vector<Real>>("shear_modulus", "Vector of shear modules values that map one-to-one with the number 'layer_ids' parameter.");
  params.addRequiredParam<std::vector<Real>>("density", "Vector of density values that map one-to-one with the number 'layer_ids' parameter.");
  params.addRequiredParam<Real>("cutoff_frequency", "The cutoff frequency in Hertz.");
  params.addRangeCheckedParam<Real>("element_size_reduction_factor", 0.1, "element_size_reduction_factor<=1.0", "Additional factor to apply to the computed minimum element size, this factor much be less than or equal to one.");
  params.addRequiredCoupledVar("layer_variable", "The variable providing the soil layer identification.");
  params.addParamNamesToGroup("element_size_reduction_factor", "Advanced");
  params.addClassDescription("Material for computing the shear wave speed ($v_s$) and minimum element size as a function of shear modules ($G$) and density ($\rho$): $v_s = \\sqrt(\frac{G}{\rho}).$");
  return params;
}

LinearSoilMaterial::LinearSoilMaterial(const InputParameters & parameters) :
    Material(parameters),
    _cutoff_frequency(getParam<Real>("cutoff_frequency")),
    _minimum_element_size_reduction_factor(getParam<Real>("element_size_reduction_factor")),
    _soil_layer_variable(coupledValue("layer_variable")),
    _shear_wave_speed(declareProperty<Real>("shear_wave_speed")),
    _minimum_wave_length(declareProperty<Real>("minimum_wave_length")),
    _minimum_element_size(declareProperty<Real>("minimum_element_size"))
{
}

void
LinearSoilMaterial::initialSetup()
{
  // Get the vectors for poulating the id to the modules/density map
  const std::vector<unsigned int> & layers = getParam<std::vector<unsigned int>>("layer_ids");
  const std::vector<Real> & modulus = getParam<std::vector<Real>>("shear_modulus");
  const std::vector<Real> & density = getParam<std::vector<Real>>("density");

  // Check that the vectors are all the same size
  if (layers.size() != modulus.size() || layers.size() != density.size() || modulus.size() != density.size())
    mooseError("The 'layer_ids', 'shear_modulus', and 'density' vectors must be the same size.");

  // Build the map
  for (unsigned int i = 0; i < layers.size(); ++i)
    _soil_id_to_shear_modules_and_density[layers[i]] = std::pair<Real, Real>(modulus[i], density[i]);
}

void
LinearSoilMaterial::computeQpProperties()
{
  // Determine the current id. The variable which is a Real must be converted to a unsigned int for lookup, so first
  // it is rounded to avoid Real values that are just below the desired value.
  _current_id = static_cast<unsigned int>(std::round(_soil_layer_variable[_qp]));

  // Locate the shear modulus and density values for the current soil id and error if it is not found
  std::map<unsigned int, std::pair<Real, Real>>::iterator it = _soil_id_to_shear_modules_and_density.find(_current_id);
  if (it == _soil_id_to_shear_modules_and_density.end())
    mooseError("The soil id variable value (" << _current_id << ") was not provided in the 'layer_ids' parameter.");

  // Shear wave speed: sqrt(G/rho)
  _shear_wave_speed[_qp] = std::sqrt(it->second.first/it->second.second);

  /// Computed minimum wavelength
  _minimum_wave_length[_qp] = 1.0/_cutoff_frequency * _shear_wave_speed[_qp];

  /// Computed minimum element size
  _minimum_element_size[_qp] = _minimum_element_size_reduction_factor * _minimum_wave_length[_qp];
}
