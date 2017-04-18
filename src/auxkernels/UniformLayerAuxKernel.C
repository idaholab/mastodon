// Mastodon includes
#include "UniformLayerAuxKernel.h"

template <>
InputParameters
validParams<UniformLayerAuxKernel>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addRequiredParam<std::vector<Real>>(
      "interfaces",
      "A list of layer interface locations to apply across the "
      "domain in the specified direction.");
  params.addParam<std::vector<unsigned int>>(
      "layer_ids",
      "A list of layer identifiers to assign to each interface. "
      "If not provided integer values starting from 0 are "
      "utilized, if provided the length of this vector must be "
      "identical to the 'intefaces' vector.");
  params.addParam<RealVectorValue>(
      "direction", RealVectorValue(1, 0, 0), "The direction to apply layering.");
  params.addClassDescription("Computes an AuxVariable for representing a "
                             "layered structure in an arbitrary direction.");
  return params;
}

UniformLayerAuxKernel::UniformLayerAuxKernel(const InputParameters & parameters)
  : AuxKernel(parameters),
    _interfaces(getParam<std::vector<Real>>("interfaces")),
    _layer_ids(getParam<std::vector<unsigned int>>("layer_ids"))
{
  // Normalize the direction
  _direction = getParam<RealVectorValue>("direction");
  if (_direction.norm() == 0)
    mooseError("The supplied direction vector is not valid, it has a zero norm.");
  _direction /= _direction.norm();

  // If not provided populate the ids vector starting a zero
  if (_layer_ids.empty())
  {
    _layer_ids.resize(_interfaces.size());
    for (unsigned int id = 0; id < _interfaces.size(); ++id)
      _layer_ids[id] = id;
  }

  if (_layer_ids.size() != _interfaces.size())
    mooseError("The number of 'interfaces' must match the number of 'layer_ids'.");
}

Real
UniformLayerAuxKernel::computeValue()
{
  // Compute projected distance (dot product of direction vector and the current
  // point)
  if (_nodal)
    _distance = _direction * (*_current_node);
  else
    _distance = _direction * _current_elem->centroid();

  // Locate the layer
  std::vector<Real>::const_iterator iter =
      std::upper_bound(_interfaces.begin(), _interfaces.end(), _distance);
  if (iter == _interfaces.end())
    mooseError("Failed to locate an interface within the domain.");

  // Return the "layer_id" at the same location as the interface that was
  // located.
  return _layer_ids[std::distance(_interfaces.begin(), iter)];
}
