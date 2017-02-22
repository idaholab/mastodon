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
#include "Material.h"
#include "MastodonTypes.h"

// Mastodon includes
#include "LayerParameter.h"

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

  /// The variable containing the layer ids
  const VariableValue & _layer_variable;

  /**
   * Get a reference to the current value of a input parameter for the current layer id.
   * @param param_name Name of the parameter to lookup according to layer id, it must be the same length as "layer_ids"
   *                   and of type std::vector<Real>.
   */
  template<typename P>
  const MooseArray<P> & getLayerParam(const std::string & param_name);

  /*
   * Add a vector of data that corresponds with the "layer_ids" parameter.
   * @param data A reference to a vector of data to correspond with layer id data.
   *
   * This method exists to allow data to be associated with layer ids can be computed and the accessed with
   * similar functionality as from getLayerParam.
   */
  template<typename P>
  const MooseArray<P> & addLayerVector(const std::vector<P> & data);


// The following are functions that shouldn't be modified in parent classes, they are used to implement the internal
// connection between the "layer_id" and other layer parameters. These also must be defined prior to the _layer_id,
// which is intended for use in parent classes, because the getLayerParam function that sets the reference for the
// _layer_id uses these members.
private:

  /// The "input_ids" as entered in the input file, this is used for size checking of other layer parameters and
  /// indexing into the other parameters vectors (see _layer_id_to_param_index)
  const std::vector<unsigned int> & _input_layer_ids;

  /// Provides the index in the input parameter data for a given layer id.
  /// A vector is used here that is sized to the maximum available layer_id value, this is inefficient with respect to
  /// to storage because the vector might be quite large depending with only a few non-zero entries. However, this
  /// vector must be accessed at each quadrature point, so doing some sort of look-up will impact performance, so we
  /// are sacrificing memory for performance.
  std::vector<unsigned int> _layer_id_to_param_index;

  /// The vector of data to reference and parameter data from the input parameters object.
  /// This uses a shared pointer rather than a unique pointer so it can be cast to the LayerParameter<P> type for
  /// returning the MooseArray reference (see addLayerVector).
  std::vector<std::shared_ptr<LayerParameterBase>> _layer_data;

protected:

  /// The current "layer id" to be used for looking up the parameters.
  const MooseArray<unsigned int> & _layer_id;

};

template<class T>
LayeredMaterialInterface<T>::LayeredMaterialInterface(const InputParameters & parameters) :
    T(parameters),
    _layer_variable(T::coupledValue("layer_variable")),
    _input_layer_ids(T::template getParam<std::vector<unsigned int>>("layer_ids")),
    _layer_id(getLayerParam<unsigned int>("layer_ids"))
{
  if (!dynamic_cast<Material*>(this))
    mooseError("The LayeredMaterialInterface requires that the template class be a Material object.");

  const std::string & doc = parameters.getDocString("layer_ids");
  if (doc.find("[This should be modified in the parent classes") != std::string::npos)
    mooseError("The documentation for the 'layer_ids' parameter must be modified using InputParameters::setDocString in the validParams function to include information on the parameters which the layer ids correspond.");
}

template<class T>
void
LayeredMaterialInterface<T>::computeProperties()
{
  // Number of quadrature points
  auto n = _layer_variable.size();

  for (auto & data : _layer_data)
  {
    // Resize the MooseArray
    data->resize(n);

    for (unsigned int qp = 0; qp < n; ++qp)
    {
      // The current "layer id"
      unsigned int current_layer_id = static_cast<unsigned int>(std::round(_layer_variable[qp]));

      // The location of the data in parameter data (i.e., data.second)
      const unsigned int & idx = _layer_id_to_param_index[current_layer_id];
      if (idx == Mastodon::INVALID_LAYER_ID)
        mooseError("The current layer id variable value (", current_layer_id, ") was not provided in the 'layer_ids' parameter of the \"", T::name(), "\" block.");

      // Update the reference data for the current quadrature point and layer id
      //    qp = The current quadrature point
      //    idx: The location in the input vectors given the current layer id
      data->reinit(qp, idx);
    }
  }

  // Call the base method
  T::computeProperties();
}

template<class T>
template<typename P>
const MooseArray<P> &
LayeredMaterialInterface<T>::getLayerParam(const std::string & param_name)
{
  // Get the parameter data and check that it is the same size as "layer_ids"
  const std::vector<P> & data = T::template getParam<std::vector<P>>(param_name);
  if (data.size() != _input_layer_ids.size())
    mooseError("The parameter \"", param_name, "\" in the \"", T::name(), "\" block must be the same length as the \"layer_ids\" parameter.");

  return addLayerVector<P>(data);
}

template<class T>
template<class P>
const MooseArray<P> &
LayeredMaterialInterface<T>::addLayerVector(const std::vector<P> & data)
{
  // Initialize the layer id to index map, this is done here rather than the constructor so that
  // the "layer_id" can utilize the getLayerParam function.
  if (_layer_id_to_param_index.empty())
  {
    unsigned int n = *std::max_element(_input_layer_ids.begin(), _input_layer_ids.end());
    _layer_id_to_param_index.resize(n + 1, Mastodon::INVALID_LAYER_ID);
    for (unsigned int i = 0; i < _input_layer_ids.size(); ++i)
      _layer_id_to_param_index[_input_layer_ids[i]] = i;
  }

  // Check that the input data being retrieved is the same size as "layer_ids"
  if (data.size() != _input_layer_ids.size())
    mooseError("The data in the \"", T::name(), "\" block supplied with the addLayerVector method must be the same length as the \"layer_ids\" parameter.");

  // Add the data to the list and return a reference to the MooseArray that will be populated in computeProperties
  _layer_data.emplace_back(std::make_shared<LayerParameter<P>>(data));
  std::shared_ptr<LayerParameter<P>> ptr = std::static_pointer_cast<LayerParameter<P>>(_layer_data.back());
  return ptr->array();
}

#endif
