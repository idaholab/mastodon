// MOOSE includes
#include "ResponseHistoryMean.h"
#include "PostprocessorInterface.h"
#include "VectorPostprocessorInterface.h"
#include "MastodonUtils.h"
#include "ResponseHistoryBuilder.h"
#include <iostream>
#include <fstream>
using namespace std;

registerMooseObject("MastodonApp", ResponseHistoryMean);

template <>
InputParameters
validParams<ResponseHistoryMean>()
{
  InputParameters params = validParams<GeneralVectorPostprocessor>();
  params.addRequiredParam<VectorPostprocessorName>(
      "vectorpostprocessor",
      "Name of the ResponseHistoryBuilder vectorpostprocessor, for which "
      "response spectra are calculated.");

  // Make sure that csv files are created only at the final timestep
  params.set<bool>("contains_complete_history") = true;
  params.suppressParameter<bool>("contains_complete_history");

  params.set<ExecFlagEnum>("execute_on") = {EXEC_FINAL};
  params.suppressParameter<ExecFlagEnum>("execute_on");

  params.addClassDescription("Calculate the mean acceleration time series given a set of nodes.");
  return params;
}

ResponseHistoryMean::ResponseHistoryMean(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _history_time(getVectorPostprocessorValue("vectorpostprocessor", "time"))

{
}

void
ResponseHistoryMean::initialSetup()
{
  const ResponseHistoryBuilder & history_vpp = getUserObjectByName<ResponseHistoryBuilder>(
      getParam<VectorPostprocessorName>("vectorpostprocessor"));
  std::vector<std::string> history_names =
      history_vpp.getHistoryNames(); // names of the vectors in responsehistorybuilder
  _history_acc.resize(history_names.size());


  for (std::size_t i = 0; i < history_names.size(); i++)
  {
    _history_acc[i] = history_vpp.getHistories()[i];
  }

// Declaring a vector of pointers for storing the addresses of the time vector
// and the mean response history vector.

  _mean_acc.push_back(&declareVector("_time"));
  _mean_acc.push_back(&declareVector("_mean"));
}

void
ResponseHistoryMean::initialize()
{
  for (VectorPostprocessorValue * ptr : _mean_acc)
    ptr->clear();
}

void
ResponseHistoryMean::execute()
{

// Calling the "mean" (overloaded) function to compute the mean of response
// histories.

  std::vector<std::vector<Real>> vec_mean_acc =
  MastodonUtils::mean(_history_acc, _history_time);

  *_mean_acc[0] = vec_mean_acc[0];
  *_mean_acc[1] = vec_mean_acc[1];

}
