#ifndef LINEARSOILMATERIAL_H
#define LINEARSOILMATERIAL_H

// MOOSE includes
#include "Material.h"

// Forward Declarations
class LinearSoilMaterial;

template<>
InputParameters validParams<LinearSoilMaterial>();

/**
 * A material for computing soil properties
 * based on linear relationship using shear modules and density.
 */
class LinearSoilMaterial : public Material
{
public:
  LinearSoilMaterial(const InputParameters & parameters);

protected:

  /**
   * Populates the map of soild layer id to shear modules and density.
   */
  virtual void initialSetup() override;

  virtual void computeQpProperties() override;

  /// The coupled variable field providing the layer id
  const VariableValue & _soil_layer_variable;

  /// Shear Modules and Density mapping to soil id.
  // This is being done in a single map to avoid looking up the id multipe times.
  std::map<unsigned int, std::pair<Real, Real>>  _soil_id_to_shear_modules_and_density;

  /// Computed shear wave speed.
  MaterialProperty<Real> & _shear_wave_speed;

  /// The id to be used for looking up the shear modulus and density, it is declared once here and re-used rather
  /// than creating a temporary variable in computeQpProperties method, simply for effeciency.
  unsigned int _current_id;
};

#endif // LINEARSOILMATERIAL_H
