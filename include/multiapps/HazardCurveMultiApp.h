#ifndef HAZARDCURVEMULTIAPP_H
#define HAZARDCURVEMULTIAPP_H

// MOOSE includes
#include "TransientMultiApp.h"
#include "UserObjectInterface.h"

// MASTODON includes
#include "HazardCurve.h"



class HazardCurveMultiApp : public TransientMultiApp, public UserObjectInterface
{
public:
  static InputParameters validParams();
  HazardCurveMultiApp(const InputParameters & parameters);

protected:
  /// HazardCurve to utilize for creating MultiApps
  const HazardCurve & _hazard_curve;
};

#endif
