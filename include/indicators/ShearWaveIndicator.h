#ifndef SHEARWAVEINDICATOR_H
#define SHEARWAVEINDICATOR_H

// MOOSE includes
#include "Indicator.h"

// libMesh includes
#include "libmesh/quadrature.h"

// Forward Declarations
class ShearWaveIndicator;

template <>
InputParameters validParams<ShearWaveIndicator>();

/**
 * Computes the minimum element size.
 */
class ShearWaveIndicator : public Indicator
{
public:
  ShearWaveIndicator(const InputParameters & params);

protected:
  /// Computes the minimum element size based on the shear wave speed
  virtual void computeIndicator() override;

  /// Shear wave speed
  const MaterialProperty<Real> & _shear_wave_speed;

  /// Maximum frequency
  const Real & _cutoff_frequency;

  /// Storage for the compute minimum value on a element.
  std::vector<Real> _minimum_element_size;

  /// The current quadrature rule
  const QBase * const & _qrule;

  /// The quadrature point index for looping
  unsigned int _qp;

  /// The variable for storing indicator value
  MooseVariable & _indicator_var;
};

#endif // SHEARWAVEINDICATOR_H
