#ifndef HAZARDCURVETRANSFER_H
#define HAZARDCURVETRANSFER_H

// MOOSE includes
#include "MultiAppTransfer.h"

// MASTODON includes
#include "HazardCurve.h"

class HazardCurveTransfer;

template<>
InputParameters validParams<HazardCurveTransfer>();

/**
 * Transfers scale hazard curve to a Piecewise function.
 */
class HazardCurveTransfer : public MultiAppTransfer, public UserObjectInterface
{
public:
  HazardCurveTransfer(const InputParameters & parameters);
  virtual void execute() override;

protected:

  //std::shared_ptr<HazardCurveMultiApp> _hazard_curve_multiapp;

//  const HazardCurve & _hazard_curve;
};

#endif
