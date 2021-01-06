// Mastodon includes
#include "MinimumElementSizeMarker.h"

// libMesh includes
#include "libmesh/error_vector.h"

registerMooseObject("MastodonApp", MinimumElementSizeMarker);

InputParameters
MinimumElementSizeMarker::validParams()
{
  InputParameters params = Marker::validParams();
  params.addParam<IndicatorName>("indicator_name",
                                         "The name of the Indicator that this Marker uses.");
  params.addParam<Real>("element_size", 0.01,
                        "Minimum element size to be enforced in the mesh.");
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
  : Marker(parameters), _element_size(getParam<Real>("element_size")), _scale(getParam<Real>("scale")), _factor(getParam<Real>("factor")),
  _error_vector(isParamValid("indicator_name") ? getErrorVector(parameters.get<IndicatorName>("indicator_name")) : getErrorVector(NULL)) // libMesh::ErrorVector(0.0)
{ // std::vector<ErrorVectorReal>
  // _error_vector = getErrorVector(getParam<IndicatorName>("indicator_name"));
  // _error_vector = getErrorVector(_indicator_name);
  // ErrorVector _error_vector(getErrorVector(parameters.get<IndicatorName>("indicator")));
  // if (isParamValid("indicator") && isParamValid("element_size"))
  //   mooseError()
  // if(isParamValid("indicator"))
    // _error_vector(getErrorVector(parameters.get<IndicatorName>("indicator")));
}

Marker::MarkerValue
MinimumElementSizeMarker::computeElementMarker()
{
  if (isParamValid("indicator_name"))
  {
    _minimum_element_size = _scale * _error_vector[_current_elem->id()] + _factor;
    // std::cout << "Here" << std::endl;
  }
  else
  {
    std::cout << "Here" << std::endl;
    // ErrorVector error_vector = getErrorVector(_indicator_name);
    _minimum_element_size = _element_size;
    // _minimum_element_size = _scale * getErrorVector(parameters.get<IndicatorName>("indicator_name"))[_current_elem->id()] + _factor;
  }

  // if (isParamValid("element_size"))
  // {
  //   _minimum_element_size = _element_size;
  //   // std::cout << "Here" << std::endl;
  // }
  // else
  // {
  //   // ErrorVector error_vector = getErrorVector(_indicator_name);
  //   _minimum_element_size = _scale * _error_vector[_current_elem->id()] + _factor;
  //   // _minimum_element_size = _scale * getErrorVector(parameters.get<IndicatorName>("indicator_name"))[_current_elem->id()] + _factor;
  // }

  // if (isParamValid("indicator_name"))
  // {
  //   // ErrorVector error_vector = getErrorVector(_indicator_name);
  //   // std::cout << error_vector[0] << std::endl;
  //   _minimum_element_size = _scale * _error_vector[_current_elem->id()] + _factor;
  // }
  // else
  // {
  //   _minimum_element_size = _element_size;
  // }
  if (_minimum_element_size < _current_elem->hmin())
    return REFINE;
  return DO_NOTHING;
}
