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

#ifndef LAYERPARAMETER_H
#define LAYERPARAMETER_H

// STL includes
#include <vector>

// MOOSE includes
#include "MooseArray.h"

/*
 * Base for defining an interface to update MooseArray<T> referenced from
 * getLayerParam method of LayeredMaterialInterface. This allows the same
 * storage structure to store a variety of parameter types.
 *
 * It also defines a container that cannot be copied, which is the case with
 * MooseArray objects
 * that are stored in the parent version (LayerParameter).
 */
class LayerParameterBase {
public:
  LayerParameterBase() = default;
  virtual ~LayerParameterBase() = default;

  LayerParameterBase(const LayerParameterBase &) = delete;
  LayerParameterBase &operator=(const LayerParameterBase &) = delete;

  LayerParameterBase(LayerParameterBase &&) = default;
  LayerParameterBase &operator=(LayerParameterBase &&) = default;

  virtual void resize(const unsigned int & /*n*/){}; // = 0;
  virtual void reinit(const unsigned int & /*qp*/,
                      const unsigned int & /*idx*/){}; // = 0;
};

/**
 * Class for storing a MooseArray to which the data from getLayerParam
 * references. This
 * data is updated with the input parameter data via LayeredMaterialInterface.
 */
template <class P> class LayerParameter : public LayerParameterBase {
public:
  LayerParameter(const std::vector<P> &data) : _param_data(data) {}

  /**
   * Resizes the MooseArray that will be referenced by calls to getLayerParam.
   */
  virtual void resize(const unsigned int &n) override { _array_data.resize(n); }

  /**
   * Updates the MooseArray data at a quadrature point given the input parameter
   * vector index.
   * see LayeredMaterialInterface::computeProperties
   */
  virtual void reinit(const unsigned int &qp,
                      const unsigned int &idx) override {
    _array_data[qp] = _param_data[idx];
  }

  /**
   * Return a reference to the MooseArray object (see getLayerParam).
   */
  const MooseArray<P> &array() { return _array_data; }

private:
  /// Reference to the input parameter data for the problem.
  const std::vector<P> &_param_data;

  /// Data that will be referenced in calls to getLayerParam
  MooseArray<P> _array_data;
};

#endif
