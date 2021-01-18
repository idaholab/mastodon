// Mastodon includes
#include "MinimumElementSizeMarker.h"

// libMesh includes
#include "libmesh/error_vector.h"

registerMooseObject("MastodonApp", MinimumElementSizeMarker);

InputParameters
MinimumElementSizeMarker::validParams()
{
  InputParameters params = Marker::validParams();
  params.addParam<IndicatorName>("indicator", "The name of the Indicator that this Marker uses.");
  params.addParam<Real>("element_size", "Minimum element size to be enforced in the mesh.");
  params.addParam<Real>("scale",
                        1.0,
                        "Additional scale term to multiply with "
                        "the computed minimum element size.");
  params.addParam<Real>(
      "factor", 0.0, "Additional factor to add to the computed minimum element size.");
  params.addClassDescription("Marks element for refinement based on the element size.");
  return params;
}

MinimumElementSizeMarker::MinimumElementSizeMarker(const InputParameters & parameters)
  : Marker(parameters),
    _element_size(isParamValid("element_size") ? &getParam<Real>("element_size") : nullptr),
    _scale(getParam<Real>("scale")),
    _factor(getParam<Real>("factor")),
    _error_vector(isParamValid("indicator")
                      ? &getErrorVector(parameters.get<IndicatorName>("indicator"))
                      : nullptr)
{
  if (_element_size && isParamValid("indicator"))
    mooseError(
        "In block ",
        name(),
        ", both the element_size and the indicator should not be provided at the same time.");
  if (!_element_size && !isParamValid("indicator"))
    mooseError(
        "In block ", name(), ", either the element_size or the indicator should be provided.");
}

Marker::MarkerValue
MinimumElementSizeMarker::computeElementMarker()
{
  if (_error_vector)
    _minimum_element_size = _scale * (*_error_vector)[_current_elem->id()] + _factor;
  else
    _minimum_element_size = (*_element_size);

  if (_minimum_element_size < _current_elem->hmin())
    return REFINE;
  return DO_NOTHING;
}
