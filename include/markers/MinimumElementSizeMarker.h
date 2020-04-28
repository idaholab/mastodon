#ifndef MINIMUMELEMENTSIZEMARKER_H
#define MINIMUMELEMENTSIZEMARKER_H

// MOOSE includes
#include "IndicatorMarker.h"

/**
 * Marks for refinement base on shear wave speed material property.
 */
class MinimumElementSizeMarker : public IndicatorMarker
{
public:
  static InputParameters validParams();
  MinimumElementSizeMarker(const InputParameters & params);

protected:
  /**
   * Computes the marker value base on the current element size and minimum size
   * indicator.
   */
  virtual MarkerValue computeElementMarker() override;

  /// Term multiplied to the minimum element size indicator.
  const Real & _scale;

  /// Term added to the minimum element size indicator.
  const Real & _factor;

  /// Place to compute the element size (for effeciency)
  Real _minimum_element_size;
};

#endif // MINIMUMELEMENTSIZEMARKER_H
