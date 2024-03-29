// MOOSE includes
#include "FEProblemBase.h"
#include "MultiApp.h"
#include "PiecewiseBase.h"

// MASTODON includes
#include "PiecewiseFunctionTransfer.h"

registerMooseObject("MastodonApp", PiecewiseFunctionTransfer);

InputParameters
PiecewiseFunctionTransfer::validParams()
{
  InputParameters params = MultiAppTransfer::validParams();
  params.addClassDescription("Transfers from one Piecewise function to another.");

  params.addRequiredParam<std::string>("to_function",
                                       "The name of the function being transferred into.");

  params.addRequiredParam<std::string>("from_function",
                                       "The name of the function being transferred out of.");

  return params;
}

PiecewiseFunctionTransfer::PiecewiseFunctionTransfer(const InputParameters & parameters)
  : MultiAppTransfer(parameters)
{
}

void
PiecewiseFunctionTransfer::execute()
{
  const std::string & from_name = getParam<std::string>("from_function");
  const std::string & to_name = getParam<std::string>("to_function");

  if (_direction == TO_MULTIAPP)
  {
    FEProblemBase & from_problem = getToMultiApp()->problemBase();
    PiecewiseBase & from_function =
        dynamic_cast<PiecewiseBase &>(from_problem.getFunction(from_name));
    for (unsigned int i = 0; i < getToMultiApp()->numGlobalApps(); i++)
      if (getToMultiApp()->hasLocalApp(i))
      {
        FEProblemBase & to_problem = getMultiApp()->appProblemBase(i);
        PiecewiseBase & to_function =
            dynamic_cast<PiecewiseBase &>(to_problem.getFunction(to_name));
        transfer(from_function, to_function);
      }
  }

  else if (_direction == FROM_MULTIAPP)
  {
    FEProblemBase & to_problem = getFromMultiApp()->problemBase();
    PiecewiseBase & to_function = dynamic_cast<PiecewiseBase &>(to_problem.getFunction(to_name));
    for (unsigned int i = 0; i < getFromMultiApp()->numGlobalApps(); i++)
      if (getFromMultiApp()->hasLocalApp(i))
      {
        FEProblemBase & from_problem = getMultiApp()->appProblemBase(i);
        PiecewiseBase & from_function =
            dynamic_cast<PiecewiseBase &>(from_problem.getFunction(from_name));
        transfer(from_function, to_function);
      }
  }
}

void
PiecewiseFunctionTransfer::transfer(PiecewiseBase & from_function, PiecewiseBase & to_function)
{
  std::size_t n = from_function.functionSize();
  std::vector<Real> x(n), y(n);
  for (std::size_t i = 0; i < n; ++i)
  {
    x[i] = from_function.domain(i);
    y[i] = from_function.range(i);
  }
  to_function.setData(x, y);
}
