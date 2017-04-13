#ifndef LAYERINTERFACEINDICATOR_H
#define LAYERINTERFACEINDICATOR_H

// MOOSE includes
#include "InternalSideIndicator.h"

// Forward Declarations
class LayerInterfaceIndicator;

template <> InputParameters validParams<LayerInterfaceIndicator>();

/**
 * Computes the "error" as defined by the difference between the layers. Since,
 * layer ids are arbitrary the
 * magnitude of the error is not relevant, just that the error is above zero.
 * Therefore, when a difference is computed
 * the "error" is set to one. Thus, this should be used with the
 * ErrorToleranceMarker.
 *
 * This inherits from InternalSideIndicator to gain access to neighbor
 * information.
 */
class LayerInterfaceIndicator : public InternalSideIndicator {
public:
  LayerInterfaceIndicator(const InputParameters &params);

  /**
   * This elminates the aggregation performed by the
   * InternalSideIndicator::finalize,
   * which is not needed here.
   */
  virtual void finalize() override {}

protected:
  /// Computes the difference in the layer ids
  virtual void computeIndicator() override;

  /**
   * This method is not used.
   *
   * see computeIndicator
   */
  virtual Real computeQpIntegral() final { return 0.0; }

  /// Tolerence for considering a layer ids different
  const Real &_tolerance;
};

#endif // LAYERINTERFACEINDICATOR_H
