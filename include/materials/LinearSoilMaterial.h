#ifndef LINEARSOILMATERIAL_H
#define LINEARSOILMATERIAL_H

// MOOSE includes
#include "Material.h"

// Mastodon includes
#include "LayeredMaterialInterface.h"

/**
 * A material for computing soil properties
 * based on linear relationship using shear modules and density.
 */
class LinearSoilMaterial : public LayeredMaterialInterface<Material>
{
public:
  static InputParameters validParams();
  LinearSoilMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  /// Computed shear wave speed.
  MaterialProperty<Real> & _shear_wave_speed;

  /// Layer parameter for the "shear_modulus" input
  const MooseArray<Real> & _layer_shear_modulus;

  /// Layer parameter for the "density" input
  const MooseArray<Real> & _layer_density;
};

#endif // LINEARSOILMATERIAL_H
