// MOOSE includes
#include "ResponseHistoryMean.h"
#include "PostprocessorInterface.h"
#include "VectorPostprocessorInterface.h"
#include "MastodonUtils.h"
#include "ResponseHistoryBuilder.h"

registerMooseObject("MastodonApp", ResponseHistoryMean);

template <>
InputParameters
validParams<ResponseHistoryMean>()
{
  InputParameters params = validParams<GeneralVectorPostprocessor>();
  params.addRequiredParam<VectorPostprocessorName>(
      "response_history",
      "Name of the ResponseHistoryBuilder vectorpostprocessor, for which "
      "response spectra are calculated.");

  // Make sure that csv files are created only at the final timestep
  params.set<bool>("contains_complete_history") = true;
  params.suppressParameter<bool>("contains_complete_history");

  params.set<ExecFlagEnum>("execute_on") = {EXEC_FINAL};
  params.suppressParameter<ExecFlagEnum>("execute_on");

  params.addClassDescription(
      "Calculate the mean acceleration time series given a response history.");
  return params;
}

ResponseHistoryMean::ResponseHistoryMean(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _builder_time(getVectorPostprocessorValue("response_history", "time")),
    _history_time(declareVector("time")),
    _history_mean(declareVector("mean")),
    _builder(getUserObjectByName<ResponseHistoryBuilder>(
        getParam<VectorPostprocessorName>("response_history")))
{
}

void
ResponseHistoryMean::initialize()
{
}

void
ResponseHistoryMean::execute()
{
  // Returning the times when response is recorded as an output.
  _history_time = _builder_time;

  // Calling the "mean" (overloaded) function to compute the mean of response  histories.
  _history_mean = MastodonUtils::mean(_builder.getHistories());
}
