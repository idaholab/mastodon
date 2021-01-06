#ifndef MINIMUMELEMENTSIZEMARKER_H
#define MINIMUMELEMENTSIZEMARKER_H

// MOOSE includes
// #include "IndicatorMarker.h"
// #include "UniformMarker.h"
#include "Marker.h"

/**
 * Marks for refinement base on shear wave speed material property.
 */
class MinimumElementSizeMarker : public Marker // , public UniformMarker
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

  /// Required element size specified by the user.
  // const std::string & _indicator_name;
  // const IndicatorName & _indicator_name;

  /// Required element size specified by the user.
  const Real & _element_size;

  /// Term multiplied to the minimum element size indicator.
  const Real & _scale;

  /// Term added to the minimum element size indicator.
  const Real & _factor;

  /// Place to compute the element size (for effeciency)
  Real _minimum_element_size;

  ErrorVector & _error_vector;
};

#endif // MINIMUMELEMENTSIZEMARKER_H
