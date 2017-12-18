#include "HazardCurveMultiApp.h"

template <>
InputParameters
validParams<HazardCurveMultiApp>()
{
  InputParameters params = validParams<TransientMultiApp>();
  params.addClassDescription(
      "Creates a sub-application for each row of each HazardCurve bin and ground motion.");
  params.addRequiredParam<UserObjectName>(
      "hazard", "The HazardCurve object to utilize for creating MultiApps.");
  params.suppressParameter<std::vector<Point>>("positions");
  params.suppressParameter<bool>("output_in_position");
  params.suppressParameter<std::vector<FileName>>("positions_file");
  params.suppressParameter<Real>("move_time");
  params.suppressParameter<std::vector<Point>>("move_positions");
  params.suppressParameter<std::vector<unsigned int>>("move_apps");
  params.set<bool>("use_positions") = false;
  return params;
}

HazardCurveMultiApp::HazardCurveMultiApp(const InputParameters & parameters)
  : TransientMultiApp(parameters),
    UserObjectInterface(this),
    _hazard_curve(getUserObject<HazardCurve>("hazard"))
{
  init(_hazard_curve.count());
}
