// MOOSE includes
#include "HousnerSpectrumIntensity.h"
#include "PostprocessorInterface.h"
#include "VectorPostprocessorInterface.h"
#include "MastodonUtils.h"
#include "LinearInterpolation.h"
#include "ResponseHistoryBuilder.h"

registerMooseObject("MastodonApp", HousnerSpectrumIntensity);

InputParameters
HousnerSpectrumIntensity::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();
  params.addRequiredParam<VectorPostprocessorName>(
      "vectorpostprocessor",
      "Name of the ResponseHistoryBuilder vectorpostprocessor, for which "
      "HSIs are calculated.");
  params.addParam<Real>("damping_ratio", 0.05, "Damping ratio for HSI calculation.");
  params.addParam<Real>("start_period", 0.25, "Start period for the HSI calculation.");
  params.addParam<Real>("end_period", 2.5, "End period for the HSI calculation.");
  params.addParam<unsigned int>(
      "num_periods", 140, "Number of frequencies for the HSI calculation.");
  params.addRequiredRangeCheckedParam<Real>("regularize_dt",
                                            "regularize_dt>0.0",
                                            "dt for HSI calculation. The acceleration "
                                            "response will be regularized to this dt prior to "
                                            "the HSI calculation.");
  // Make sure that csv files are created only at the final timestep
  params.set<bool>("contains_complete_history") = true;
  params.suppressParameter<bool>("contains_complete_history");

  params.set<ExecFlagEnum>("execute_on") = {EXEC_FINAL};
  params.suppressParameter<ExecFlagEnum>("execute_on");
  params.addClassDescription("Calculate the HSI for the requested acceleration variables.");
  return params;
}

HousnerSpectrumIntensity::HousnerSpectrumIntensity(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _xi(getParam<Real>("damping_ratio")),
    _per_start(getParam<Real>("start_period")),
    _per_end(getParam<Real>("end_period")),
    _per_num(getParam<unsigned int>("num_periods")),
    _reg_dt(getParam<Real>("regularize_dt")),
    // Time vector from the ResponseHistoryBuilder vectorpostprocessor.
    _history_time(getVectorPostprocessorValue("vectorpostprocessor", "time"))

{
  // Check for starting and ending period
  if (_per_start >= _per_end)
    mooseError("Error in " + name() + ". Starting period must be less than the ending period.");

  // Check for damping
  if (_xi <= 0)
    mooseError("Error in " + name() + ". Damping ratio must be positive.");
}

void
HousnerSpectrumIntensity::initialSetup()
{
  const ResponseHistoryBuilder & history_vpp = getUserObjectByName<ResponseHistoryBuilder>(
      getParam<VectorPostprocessorName>("vectorpostprocessor"));
  std::vector<std::string> history_names =
      history_vpp.getHistoryNames(); // names of the vectors in responsehistorybuilder
  _history_acc.resize(history_names.size());
  for (unsigned int i = 0; i < history_names.size(); ++i)
  {
    // Acceleration vectors corresponding to the variables from the
    // ResponseHistoryBuilder vectorpostprocessor.
    _history_acc[i] = history_vpp.getHistories()[i];
    _hsi_vec.push_back(&declareVector(history_names[i] + "_hsi"));
  }
}

void
HousnerSpectrumIntensity::initialize()
{
  for (std::size_t i = 0; i < _hsi_vec.size(); ++i)
    _hsi_vec[i]->clear();
}

void
HousnerSpectrumIntensity::execute()
{
  std::vector<Real> period;
  std::vector<Real> vel_spectrum;
  Real freq_start = 1 / _per_end;
  Real freq_end = 1 / _per_start;
  for (unsigned int i = 0; i < _hsi_vec.size(); ++i)
  {
    // The acceleration responses may or may not have a constant time step.
    // Therefore, they are regularized by default to a constant time step by the
    // regularize function before performing the HSI calculations.
    std::vector<std::vector<Real>> reg_vector =
        MastodonUtils::regularize(*_history_acc[i], _history_time, _reg_dt);
    // Calculation of the response spectrum.
    std::vector<std::vector<Real>> var_spectrum = MastodonUtils::responseSpectrum(
        freq_start, freq_end, _per_num, reg_vector[1], _xi, _reg_dt);
    period = var_spectrum[1];
    vel_spectrum = var_spectrum[3];
    std::reverse(period.begin(), period.end());
    std::reverse(vel_spectrum.begin(), vel_spectrum.end());
    LinearInterpolation hsi_calc(period, vel_spectrum);
    // HSI is calculated as the area below the vel_spectrum curve. HSI is output
    // into the csv file in the same order as the variables in the .i file.
    _hsi_vec[i]->push_back(hsi_calc.integrate());
  }
}
