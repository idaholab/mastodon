#ifndef ELEMENTSIZEINDICATOR_H
#define ELEMENTSIZEINDICATOR_H

// MOOSE includes
#include "Indicator.h"

// libMesh includes
#include "libmesh/quadrature.h"

/**
 * Computes the minimum element size.
 */
class ElementSizeIndicator : public Indicator
{
public:
  static InputParameters validParams();
  ElementSizeIndicator(const InputParameters & params);

protected:
  /// Computes the minimum element size based on the shear wave speed
  virtual void computeIndicator() override;

  /// Value of the material
  const MaterialProperty<Real> & _element_size;

  /// Storage for the compute minimum value on a element.
  std::vector<Real> _minimum_element_size;

  /// The current quadrature rule
  const QBase * const & _qrule;

  /// The quadrature point index for looping
  unsigned int _qp;

  /// The variable for storing indicator value
  MooseVariable & _indicator_var;
};

#endif // ELEMENTSIZEINDICATOR_H
