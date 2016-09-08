#ifndef LAYERINTERFACEINDICATOR_H
#define LAYERINTERFACEINDICATOR_H

// MOOSE includes
#include "InternalSideIndicator.h"

// Forward Declarations
class LayerInterfaceIndicator;

template<>
InputParameters validParams<LayerInterfaceIndicator>();

/**
 * Computes the "error" as defined by the difference between the layers. Since, layer ids are arbitrary the
 * magnitude of the error is not relevant, just that the error is above zero. Threfore, when a differnce is computed
 * the "error" is set to one. Thus, this should be used with the ErrorToleranceMarker.
 */
class LayerInterfaceIndicator : public InternalSideIndicator
{
public:
  LayerInterfaceIndicator(const InputParameters & params);

protected:

  /// Computes the difference in the layer ids
  virtual void computeIndicator() override;

  /**
   * This method is not used.
   *
   * see computeIndicator
   */
  virtual Real computeQpIntegral() final { return 0.0; }
};

#endif // LAYERINTERFACEINDICATOR_H
