/*************************************************/
/*           DO NOT MODIFY THIS HEADER           */
/*                                               */
/*                     MASTODON                  */
/*                                               */
/*    (c) 2015 Battelle Energy Alliance, LLC     */
/*            ALL RIGHTS RESERVED                */
/*                                               */
/*   Prepared by Battelle Energy Alliance, LLC   */
/*     With the U. S. Department of Energy       */
/*                                               */
/*     See COPYRIGHT for full restrictions       */
/*************************************************/

#ifndef LAYEREDMATERIALINTERFACE_H
#define LAYEREDMATERIALINTERFACE_H

// MOOSE includes
#include "MooseObject.h"
#include "Material.h"
#include "MooseUtils.h"

template<class T = Material>
class LayeredMaterialInterface;

template<>
InputParameters validParams<LayeredMaterialInterface<>>();

/**
 * An interface class to build materials based on a layer number from a variable.
 *
 * The main purpose is to add the getLayerParam function that allows access to
 * input parameters associated with a layer id in a simple manner.
 */
template<class T>
class LayeredMaterialInterface : public T
{
public:
  LayeredMaterialInterface(const InputParameters & parameters);

  /**
   * Adds updating of the layer parameter data structure.
   */
  virtual void computeProperties() override;

protected:

  /**
   * Get a reference to the current value of a input parameter for the current layer id.
   * @param param_name Name of the parameter to lookup according to layer id, it must be the same length as "layer_ids"
   *                   and of type std::vector<Real>.
   */
  const MooseArray<Real> & getLayerParam(const std::string & param_name);

  /// The variable containing the layer ids
  const VariableValue & _layer_variable;

// The following are functions that shouldn't be modified in parent classes, they are used to implement the internal
// connection between the "layer_id" and other layer parameters. These also must be defined prior to the _layer_id,
// which is intended for use in parent classes, because the getLayerParam function that sets the reference for the
// _layer_id uses these members.
private:

  /// The "input_ids" as entered in the input file, this is used for size checking of other layer parameters
  const std::vector<Real> & _input_layer_ids;

  /// Maps the value of the layer id to the location in the input vector, this is used to access the data in the
  /// layer parameters for the given layer id.
  std::map<unsigned int, unsigned int> _layer_id_to_index_map;

  /// Storage for input vectors of layer parameters, this is updated when getLayerParam is called
  std::map<std::string, const std::vector<Real>* > _name_to_param_data_map;

  /// Storage for the data for the current layer id and quadrature point, the data here is referenced
  /// in the return value of the getLayerParam method.
  std::map<std::string, MooseArray<Real>> _name_to_moose_array_map;

protected:
  /// The current "layer id" to be used for looking up the parameters.
  const MooseArray<Real> & _layer_id;

};

template<class T>
LayeredMaterialInterface<T>::LayeredMaterialInterface(const InputParameters & parameters) :
    T(parameters),
    _layer_variable(T::coupledValue("layer_variable")),
    _input_layer_ids(T::template getParam<std::vector<Real>>("layer_ids")),
    _layer_id(getLayerParam("layer_ids"))
{
  if (!dynamic_cast<Material*>(this))
    mooseError("The LayeredMaterialInterface requires that the template class be a Material object.");

  const std::string & doc = parameters.getDocString("layer_ids");
  if (doc.find("[This should be modified in the parent classes") != std::string::npos)
    mooseError("The documentation for the 'layer_ids' parameter must be modified using InputParameters::etDocString in the validParams function to include information on the parameters which the layer ids correspond.");
}

template<class T>
void
LayeredMaterialInterface<T>::computeProperties()
{
  // Number of quadrature points
  auto n = _layer_variable.size();

  // Loop over the layer parameters and update the data referenced by calls to getLayerParam method.
  for (const auto & map_item : _name_to_param_data_map)
  {
    _name_to_moose_array_map[map_item.first].resize(n);
    for (unsigned int qp = 0; qp < n; ++qp)
    {
      // The current "layer id"
      unsigned int current_layer_id = static_cast<unsigned int>(std::round(_layer_variable[qp]));

      // Locate the position in the layer parameter vectors to extract data
      std::map<unsigned int, unsigned int>::const_iterator idx_it = _layer_id_to_index_map.find(current_layer_id);
      if (idx_it == _layer_id_to_index_map.end())
        mooseError("The current layer id variable value (" << current_layer_id << ") was not provided in the 'layer_ids' parameter of the \"" << T::name() << "\" block.");

      // Update the reference data for the current quadrature point and layer id
      //    qp = The current quadrature point
      //    map_item.first = name of the layer property (i.e., the name of the input parameter used in getLayerParam)
      //    map_item.second = The MooseArray<Real> that is referenced by return value of getLayerParam
      //    idx_it->second: The location in the input vectors given the current layer id
      _name_to_moose_array_map[map_item.first][qp] = (*map_item.second)[idx_it->second];
    }
  }

  // Call the base method
  T::computeProperties();
}

template<class T>
const MooseArray<Real> &
LayeredMaterialInterface<T>::getLayerParam(const std::string & param_name)
{
  // Initialize the layer id to index map, this is done here rather than the constructor so that
  // the "layer_id" can utilize the getLayerParam function.
  if (_layer_id_to_index_map.empty())
    for (unsigned int i = 0; i < _input_layer_ids.size(); ++i)
      _layer_id_to_index_map[_input_layer_ids[i]] = i;

  // Check that the input parameter being retrieved is the same size as "layer_ids"
  const std::vector<Real> & data = T::template getParam<std::vector<Real>>(param_name);
  if (data.size() != _input_layer_ids.size())
    mooseError("The parameter \"" << param_name << "\" in the \"" << T::name() << "\" block must be the same length as the \"layer_ids\" parameter.");

  // Store the input parameter data vector
  _name_to_param_data_map[param_name] = &data;

  // This is intentional insertion of an empty MooseArray<Real>, the data is update in computeProperties
  return _name_to_moose_array_map[param_name];
}

#endif
