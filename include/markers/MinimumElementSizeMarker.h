#ifndef MINIMUMELEMENTSIZEMARKER_H
#define MINIMUMELEMENTSIZEMARKER_H

#include "Marker.h"

/**
 * Marks for refinement based on an indicator or an input element size.
 */
class MinimumElementSizeMarker : public Marker
{
public:
  static InputParameters validParams();
  MinimumElementSizeMarker(const InputParameters & params);

protected:
  /**
   * Computes the marker value based on the current element size and minimum size
   * indicator or value.
   */
  virtual MarkerValue computeElementMarker() override;

  /// Pointer to the element size specified by the user.
  const Real * _element_size;

  /// Term multiplied to the minimum element size indicator.
  const Real & _scale;

  /// Term added to the minimum element size indicator.
  const Real & _factor;

  /// Place to compute the element size (for effeciency)
  Real _minimum_element_size;

  /// Pointer to store the libmesh error vector variable
  ErrorVector * _error_vector;
};

#endif // MINIMUMELEMENTSIZEMARKER_H
