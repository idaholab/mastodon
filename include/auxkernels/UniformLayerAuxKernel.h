#ifndef UNIFORMLAYERAUXKERNEL_H
#define UNIFORMLAYERAUXKERNEL_H

// MOOSE includes
#include "AuxKernel.h"

// Forward Declarations
class UniformLayerAuxKernel;

template <>
InputParameters validParams<UniformLayerAuxKernel>();

/**
 * Computes uniform layer ids given thicknesses and a direction.
 */
class UniformLayerAuxKernel : public AuxKernel
{
public:
  UniformLayerAuxKernel(const InputParameters & parameters);

protected:
  /**
   * Returns the layer id given the layer thicknesses and direction.
   */
  virtual Real computeValue() override;

  /// The layer interfaces along the direction vector
  const std::vector<Real> & _interfaces;

  /// The layer id values
  std::vector<unsigned int> _layer_ids;

  /// The direction vector
  RealVectorValue _direction;

  /// The projected distance of the node or element centroid
  Real _distance;
};

#endif // UNIFORMLAYERAUXKERNEL_H
