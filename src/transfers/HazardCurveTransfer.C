// MOOSE includes
#include "FEProblemBase.h"
#include "MultiApp.h"
#include "Piecewise.h"
#include "Conversion.h"

// MASTODON includes
#include "HazardCurveMultiApp.h"
#include "HazardCurveTransfer.h"

registerMooseObject("MastodonApp", HazardCurveTransfer);

template <>
InputParameters
validParams<HazardCurveTransfer>()
{
  InputParameters params = validParams<MultiAppTransfer>();
  params.addClassDescription("Transfers scaled ground motion data from a HazardCurve object to "
                             "a Piecewise function on the sub application.");
  params.set<MooseEnum>("direction") = "to_multiapp";
  params.suppressParameter<MooseEnum>("direction");
  params.addRequiredParam<std::string>(
      "function",
      "The name of the function on the sub application to which data will be transferred");
  params.addParam<MooseEnum>(
      "component",
      MooseEnum("x=1 y=2 z=3", "x"), // These enums must correspond to GroundMotionReader::component
      "The (x, y, z) component of the ground motion that will be "
      "transfered to the sub application.");
  return params;
}

HazardCurveTransfer::HazardCurveTransfer(const InputParameters & parameters)
  : MultiAppTransfer(parameters), UserObjectInterface(this)
{
}

void
HazardCurveTransfer::execute()
{
  // Require HazardCurveMultiApp
  std::shared_ptr<HazardCurveMultiApp> ptr =
      std::dynamic_pointer_cast<HazardCurveMultiApp>(getMultiApp());
  if (!ptr)
    mooseError(
        "The HazardCurveTransfer '", name(), "' will only work with a HazardCurveMultiApp object.");

  // Get input required for transfer
  const HazardCurve & hazard_curve = ptr->getUserObject<HazardCurve>("hazard");
  const std::string & function_name = getParam<std::string>("function");
  GroundMotionReader::Component comp =
      getParam<MooseEnum>("component").getEnum<GroundMotionReader::Component>();

  // Double check sizes, this really shouldn't be possible to trip
  mooseAssert(getMultiApp()->numGlobalApps() == hazard_curve.count(),
              "The number of apps does not match the number of hazard curves.");

  // Loop over
  unsigned int global_index = 0;
  for (std::size_t bin = 0; bin < hazard_curve.bins(); ++bin)
    for (std::size_t index = 0; index < hazard_curve.count(bin); ++index)
    {
      if (getMultiApp()->hasLocalApp(global_index))
      {
        const std::vector<Real> & t =
            hazard_curve.getData(bin, index, GroundMotionReader::Component::TIME);
        const std::vector<Real> & a = hazard_curve.getData(bin, index, comp);

        FEProblemBase & problem = getMultiApp()->appProblemBase(global_index);
        Piecewise & function = dynamic_cast<Piecewise &>(problem.getFunction(function_name));
        function.setData(t, a);
      }
      global_index++;
    }
}
