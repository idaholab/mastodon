#ifndef HAZARDCURVETRANSFER_H
#define HAZARDCURVETRANSFER_H

// MOOSE includes
#include "MultiAppTransfer.h"

// MASTODON includes
#include "HazardCurve.h"



/**
 * Transfers scale hazard curve to a Piecewise function.
 */
class HazardCurveTransfer : public MultiAppTransfer, public UserObjectInterface
{
public:
  static InputParameters validParams();
  HazardCurveTransfer(const InputParameters & parameters);
  virtual void execute() override;

protected:
  // std::shared_ptr<HazardCurveMultiApp> _hazard_curve_multiapp;

  //  const HazardCurve & _hazard_curve;
};

#endif
