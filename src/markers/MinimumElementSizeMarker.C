// Mastodon includes
#include "MinimumElementSizeMarker.h"

// libMesh includes
#include "libmesh/error_vector.h"

template<>
InputParameters validParams<MinimumElementSizeMarker>()
{
  InputParameters params = validParams<IndicatorMarker>();
  params.addParam<Real>("scale", 1.0, "Additional scale term to multiply with the computed minimum element size.");
  params.addParam<Real>("factor", 0.0, "Additional factor to add to the computed minimum element size.");
  params.addClassDescription("Marks element for refienment based on the element size.");
  return params;
}

MinimumElementSizeMarker::MinimumElementSizeMarker(const InputParameters & parameters) :
    IndicatorMarker(parameters),
    _scale(getParam<Real>("scale")),
    _factor(getParam<Real>("factor"))
{
}

Marker::MarkerValue
MinimumElementSizeMarker::computeElementMarker()
{
  _minimum_element_size = _scale * _error_vector[_current_elem->id()] + _factor;
  if (_minimum_element_size < _current_elem->hmin())
    return REFINE;
  return DO_NOTHING;
}
