// MOOSE includes
#include "ResponseSpectraCalculator.h"
#include "PostprocessorInterface.h"
#include "VectorPostprocessorInterface.h"
#include "MastodonUtils.h"

template <>
InputParameters
validParams<ResponseSpectraCalculator>()
{
  InputParameters params = validParams<GeneralVectorPostprocessor>();
  params.addRequiredParam<VectorPostprocessorName>(
      "vectorpostprocessor",
      "Name of the ResponseHistoryBuilder vectorpostprocessor, for which "
      "response spectra are calculated.");
  params.addRequiredParam<std::vector<VariableName>>(
      "variables", "Variables for which response spectra are requested (accelerations only).");
  params.addRequiredParam<Real>("damping_ratio", "Damping ratio for response spectra calculation.");
  params.addRequiredParam<Real>("calculation_time",
                                "Time, when the response spectrum calculation "
                                "is made. Usually the final time of the "
                                "simulation.");
  params.addParam<Real>(
      "start_frequency", 0.01, "Start frequency for the response spectra calculation.");
  params.addParam<Real>(
      "end_frequency", 100.0, "End frequency for the response spectra calculation.");
  params.addParam<Real>(
      "num_frequencies", 401, "Number of frequencies for the response spectra calculation.");
  params.addRequiredParam<Real>("dt_output",
                                "dt for response spectra calculation. The "
                                "acceleration response will be regularized to this dt "
                                "prior to the response spectrum calculation.");
  params.addClassDescription("Calculate the response spectrum at the requested nodes or points.");
  return params;
}

ResponseSpectraCalculator::ResponseSpectraCalculator(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _varnames(getParam<std::vector<VariableName>>("variables")),
    _xi(getParam<Real>("damping_ratio")),
    _calc_time(getParam<Real>("calculation_time")),
    _freq_start(getParam<Real>("start_frequency")),
    _freq_end(getParam<Real>("end_frequency")),
    _freq_num(getParam<Real>("num_frequencies")),
    _reg_dt(getParam<Real>("dt_output")),
    _frequency(declareVector("_frequency")),
    // Time vector from the response history builder vector postprocessor
    _history_time(getVectorPostprocessorValue("vectorpostprocessor", "_time"))

{
  // Check for starting and ending frequency
  if (_freq_start >= _freq_end)
    mooseError("Error in " + name() +
               ". Starting frequency must be less than the ending frequency.");

  // Check for damping
  if (_xi <= 0)
    mooseError("Error in " + name() + ". Damping ratio must be positive.");

  // Check for dt
  if (_reg_dt <= 0)
    mooseError("Error in " + name() + ". dt must be positive.");

  // Declaring three spectrum vectors: displacement, velocity and acceleration
  // for each variable name input by the user.
  for (unsigned int i = 0; i < _varnames.size(); ++i)
  {
    _history_acc.push_back(&getVectorPostprocessorValue("vectorpostprocessor", _varnames[i]));
    _spectrum.push_back(&declareVector(_varnames[i] + "_sd"));
    _spectrum.push_back(&declareVector(_varnames[i] + "_sv"));
    _spectrum.push_back(&declareVector(_varnames[i] + "_sa"));
  }
}

void
ResponseSpectraCalculator::initialize()
{
  _frequency.clear();
  for (int i = 0; i < _spectrum.size(); ++i)
    _spectrum[i]->clear();
}

void
ResponseSpectraCalculator::execute()
{
  // Only performing the calculation if current time is equal to calculation
  // time. Sometimes _t is not exactly equal to the _calc_time. Therefore, the
  // calculation is performed when the distance between _t and _calc_time is
  // smaller than the _dt at that time step. The makes sure that the
  // calculation is performed only once.
  for (int i = 0; i < _varnames.size() && abs(_t - _calc_time) < _dt; ++i)
  {
    // The acceleration responses may or may not have a constant time step.
    // Therefore, they are regularized by default to a constant time step by the
    // regularize function before performing the response spectrum calculations.
    std::vector<std::vector<Real>> reg_vector =
        MastodonUtils::regularize(*_history_acc[i], _history_time, _reg_dt);
    // Calculation of the response spectrum. All three spectra: displacmeent,
    // velocity and acceleration, are calculated and output into a csv file.
    std::vector<std::vector<Real>> var_spectrum = MastodonUtils::responseSpectrum(
        _freq_start, _freq_end, _freq_num, reg_vector[1], _xi, _reg_dt);
    _frequency = var_spectrum[0];
    *_spectrum[3 * i] = var_spectrum[1];
    *_spectrum[3 * i + 1] = var_spectrum[2];
    *_spectrum[3 * i + 2] = var_spectrum[3];
  }
}
