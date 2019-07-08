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
      "vectorpostprocessor",
      "Name of the ResponseHistoryBuilder vectorpostprocessor, for which "
      "response spectra are calculated.");
  /* params.addRequiredParam<std::vector<Real>>(
          "mean_acc_history",
          "Mean of acceleration time histories of multiple nodes."); */
  // Make sure that csv files are created only at the final timestep
  params.set<bool>("contains_complete_history") = true;
  params.suppressParameter<bool>("contains_complete_history");

  params.set<ExecFlagEnum>("execute_on") = {EXEC_FINAL};
  params.suppressParameter<ExecFlagEnum>("execute_on");

  params.addClassDescription("Calculate the response spectrum at the requested nodes or points.");
  return params;
}

ResponseHistoryMean::ResponseHistoryMean(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
  // _mean_acc(declareVector("mean_acc_history")),
    //_frequency(declareVector("frequency")),
    // Time vector from the response history builder vector postprocessor
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

  // Declaring three spectrum vectors: displacement, velocity and acceleration
  // for each vector in history VPP.
  // for (const std::string & name : _varnames)
  for (std::size_t i = 0; i < history_names.size(); i++)
  {
    _history_acc[i] = history_vpp.getHistories()[i];
  }

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

  /*std::vector<std::vector<Real>> vec_mean_acc = MastodonUtils::responseSpectrum(
      _freq_start, _freq_end, _freq_num, reg_vector[1], _xi, _reg_dt);*/

  std::vector<std::vector<Real>> vec_mean_acc =
  MastodonUtils::VectorMean(_history_acc, _history_time);

  *_mean_acc[0] = vec_mean_acc[0];
  *_mean_acc[1] = vec_mean_acc[1];


/*  for (std::size_t i = 0; i < _history_acc.size(); ++i)
  {

    // Calculation of the response spectrum. All three spectra: displacmeent,
    // velocity and acceleration, are calculated and output into a csv file.
    std::vector<std::vector<Real>> var_spectrum = MastodonUtils::responseSpectrum(
        _freq_start, _freq_end, _freq_num, reg_vector[1], _xi, _reg_dt);
    _frequency = var_spectrum[0];
    *_spectrum[3 * i] = var_spectrum[1];
    *_spectrum[3 * i + 1] = var_spectrum[2];
    *_spectrum[3 * i + 2] = var_spectrum[3];
  }*/
}
