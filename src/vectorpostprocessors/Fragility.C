// MOOSE includes
#include "Fragility.h"
#include "PostprocessorInterface.h"
#include "VectorPostprocessorInterface.h"
#include "MastodonUtils.h"
#include "LinearInterpolation.h"
#include "DelimitedFileReader.h"
#include <string>

registerMooseObject("MastodonApp", Fragility);

InputParameters
Fragility::validParams()
{
  InputParameters params = GeneralVectorPostprocessor::validParams();
  params.addParam<std::string>("master_file", "Name of the master file without extension.");
  params.addParam<std::string>("hazard_multiapp",
                               "Name of the multiapp corresponding to ground motion sampling.");
  params.addParam<std::string>(
      "probabilistic_multiapp",
      "Name of the multiapp corresponding to the probabilistic simulations.");
  params.addRequiredParam<unsigned int>("num_gms",
                                        "Number of ground motions used in each intensity bin.");
  params.addParam<std::string>(
      "demand_variable",
      "Demand variable for the SSC that is also column name in the output csv "
      "file. Acceleration variable only."); // TODO: generalize class for all
                                            // demand variables by processing
                                            // VectorPostprocessor data and
                                            // getting rid of response spectra
                                            // calculation in this class.
  params.addParam<Real>("ssc_frequency",
                        "Frequency at which the spectral demand of the SSC is calculated.");
  params.addParam<Real>("ssc_damping_ratio",
                        "Damping ratio at which the spectral demand of the SSC is calculated.");
  params.addParam<Real>("dtsim", "dt for response spectra calculation.");
  params.addParam<std::string>("demand_filename",
                               "File name that contains stochastic demand matrix. Has m x n values "
                               "where m is number of GMs in each bin and n is num bins.");
  params.addRequiredParam<Real>(
      "median_capacity",
      "Median capacity of the SSC in terms of local demand at the SSC location.");
  params.addRequiredParam<Real>("beta_capacity", "Uncertainty in the capacity of the SSC.");
  params.addRequiredParam<unsigned int>(
      "num_bins", "Number of bins in the hazard curve where the risk calculation is performed.");
  params.addRequiredParam<unsigned int>("num_samples",
                                        "Number of probabilistic simulations for each bin.");
  params.addParam<unsigned int>(
      "num_collapses",
      500,
      "Number of collapses required to calculate likelihood when using Baker's MLE.");
  params.addRequiredParam<std::vector<Real>>(
      "im_values", "IM values used in the bins."); // TODO: remove this later by transferring IM
                                                   // values from hazard curve UO
  params.addRequiredParam<std::vector<Real>>("median_fragility_limits",
                                             "Limits for median fragility of the component.");
  params.addRequiredParam<std::vector<Real>>(
      "beta_fragility_limits",
      "Limits for the lognormal standard deviation of the component fragility.");
  params.addParam<bool>("brute_force",
                        false,
                        "Optimization "
                        "method for fragility fitting. The following "
                        "methods are available: brute force "
                        "or Randomized Gradient Descent.");
  params.addParam<Real>("rgd_tolerance",
                        1e-03,
                        "Tolerance for declaring convergence of the Randomized Gradient Descent "
                        "algorithm.");
  params.addParam<Real>("rgd_gamma",
                        0.001,
                        "Parameter controlling the step size of the Randomized Gradient Descent "
                        "algorithm.");
  params.addParam<Real>("rgd_numrnd",
                        1000,
                        "Number of random initializations in the Randomized Gradient Descent "
                        "algorithm.");
  params.addParam<Real>("rgd_seed",
                        1028,
                        "Seed for random number generator in the Randomized Gradient Descent "
                        "algorithm.");
  params.addClassDescription("Calculate the seismic fragility of an SSC by postprocessing the "
                             "results of a probabilistic or stochastic simulation.");
  return params;
}

Fragility::Fragility(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _master_file(isParamValid("master_file") ? &getParam<std::string>("master_file") : NULL),
    _hazard_multiapp(isParamValid("hazard_multiapp") ? &getParam<std::string>("hazard_multiapp")
                                                     : NULL),
    _probabilistic_multiapp(isParamValid("probabilistic_multiapp")
                                ? &getParam<std::string>("probabilistic_multiapp")
                                : NULL),
    _demand_variable(isParamValid("demand_variable") ? &getParam<std::string>("demand_variable")
                                                     : NULL),
    _ssc_freq(isParamValid("ssc_frequency") ? &getParam<Real>("ssc_frequency") : NULL),
    _ssc_xi(isParamValid("ssc_damping_ratio") ? &getParam<Real>("ssc_damping_ratio") : NULL),
    _dtsim(isParamValid("dtsim") ? &getParam<Real>("dtsim") : NULL),
    _rh_file_exist(_master_file && _hazard_multiapp && _probabilistic_multiapp &&
                   _demand_variable && _ssc_freq && _ssc_xi && _dtsim),
    _demand_filename(isParamValid("demand_filename") ? &getParam<std::string>("demand_filename")
                                                     : NULL),
    _sd_file_exist(!!_demand_filename),
    _num_gms(getParam<unsigned int>("num_gms")),
    _median_cap(getParam<Real>("median_capacity")),
    _beta_ssc_cap(getParam<Real>("beta_capacity")),
    _num_bins(getParam<unsigned int>("num_bins")),
    _num_samples(getParam<unsigned int>("num_samples")),
    _num_collapses(getParam<unsigned int>("num_collapses")),
    _im_values(getParam<std::vector<Real>>("im_values")),
    _median_fragility_limits(getParam<std::vector<Real>>("median_fragility_limits")),
    _beta_fragility_limits(getParam<std::vector<Real>>("beta_fragility_limits")),
    _im(declareVector("intensity")),
    _median_demand(declareVector("demand_median")),
    _beta_demand(declareVector("demand_beta")),
    _conditional_pf(declareVector("conditional_pf")),
    _median_fragility(declareVector("fragility_median")),
    _beta_fragility(declareVector("fragility_beta")),
    _loglikelihood(declareVector("loglikelihood")),
    _brute_force(getParam<bool>("brute_force")),
    _rgd_tolerance(getParam<Real>("rgd_tolerance")),
    _rgd_gamma(getParam<Real>("rgd_gamma")),
    _rgd_numrnd(getParam<Real>("rgd_numrnd")),
    _rgd_seed(getParam<Real>("rgd_seed"))
{
  if (_rh_file_exist && _sd_file_exist)
    mooseError("Error in block '" + name() +
               "'. Both response history file options and stochastic demand file options "
               "are provided. Provide exactly one set of them.");
  if (!_rh_file_exist && !_sd_file_exist)
    mooseError(
        "Error in block '" + name() +
        "'. Either one or more of the response history file options are missing or "
        "none of the response history file options or stochastic demand file "
        "options are provided. Provide exactly one of them. \n\n **If response history files "
        "are to be used, please provide the input parameters, master_file, hazard_multiapp, "
        "probabilistic_multiapp, demand_variable, ssc_freq, ssc_xi, and dt.");

  if (!_rh_file_exist && (_master_file || _hazard_multiapp || _probabilistic_multiapp ||
                          _demand_variable || _ssc_freq || _ssc_xi || _dtsim))
    mooseError(
        "Error in block '" + name() +
        "'. If response history files "
        "are to be used, please provide ALL of the input parameters, master_file, hazard_multiapp, "
        "probabilistic_multiapp, demand_variable, ssc_freq, ssc_xi, and dt.");

  if (_rh_file_exist)
  {
    // Check for non-positive SSC frequency
    if (*_ssc_freq <= 0)
      mooseError("Error in block '" + name() + "'. SSC frequency must be positive.");
    // Check for non-positive SSC damping
    if (*_ssc_xi <= 0)
      mooseError("Error in block '" + name() + "'. SSC damping ratio must be positive.");
  }

  // // Check for non-positive median and beta of capacity
  if (_median_cap <= 0 || _beta_ssc_cap <= 0)
    mooseError(
        "Error in block '" + name() +
        "'. Median and lognormal standard deviation of the capacity of the SSC must be positive.");
  // Check if median and lognormal standard deviation limits for fraglity are of size 2
  if (_median_fragility_limits.size() != 2 || _beta_fragility_limits.size() != 2)
    mooseError("Error in block '" + name() +
               "'. Please provide two limits (minimum and maximum) "
               "for median and lognormal standard deviation of the "
               "component.");
  if (_median_fragility_limits[0] >= _median_fragility_limits[1])
    mooseError("Error in block '" + name() +
               "'. Minimum median fragility should be less than the maximum median fragility.");
  if (_beta_fragility_limits[0] >= _beta_fragility_limits[1])
    mooseError(
        "Error in block '" + name() +
        "'. Minimum should be less than the maximum for fragility lognormal standard deviation.");
  if (_median_fragility_limits[0] <= 0 || _beta_fragility_limits[0] <= 0)
    mooseError("Error in block '" + name() +
               "'. Limits of median and beta of the component fragility must be positive.");
  // Check if _im_values is of the same size as the number of bins
  if (_im_values.size() != _num_bins)
    mooseError("Error in block '" + name() +
               "'. Number of IM values should be the same as the number of bins.");
}

void
Fragility::initialize()
{
  _median_demand.clear();
  _beta_demand.clear();
  _im.clear();
  _conditional_pf.clear();
  _median_fragility.clear();
  _beta_fragility.clear();
  _loglikelihood.clear();
}

void
Fragility::execute()
{
  _im.resize(_num_bins);
  _median_demand.resize(_num_bins);
  _beta_demand.resize(_num_bins);
  _conditional_pf.resize(_num_bins);
  _median_fragility.resize(1);
  _beta_fragility.resize(1);
  _loglikelihood.resize(1);
  for (std::size_t bin = 0; bin < _num_bins; bin++)
  {
    _im[bin] = _im_values[bin];
    std::vector<Real> stoc_demands;
    if (_sd_file_exist)
      // reading demands from stochastic demand csv files if it is a restart PRA
      stoc_demands = Fragility::readDemandsFromSDFiles(bin);
    else
      // calculating demands from RH csv files if it is a restart PRA
      stoc_demands = Fragility::calcDemandsFromRHFiles(bin);
    _median_demand[bin] = MastodonUtils::median(stoc_demands);
    _beta_demand[bin] = MastodonUtils::lognormalStandardDeviation(stoc_demands);

    Real demand_median = _median_demand[bin];
    Real demand_scale = _beta_demand[bin];
    Real capacity_median = _median_cap;
    Real capacity_scale = _beta_ssc_cap;

    _conditional_pf[bin] = MastodonUtils::greaterProbability(
        demand_median, demand_scale, capacity_median, capacity_scale);
    _console << "**********\nFinished calculations for bin: " << bin
             << " \n Median demand: " << _median_demand[bin]
             << " \n Lognormal standard deviation: " << _beta_demand[bin]
             << " \n Conditional probability of failure: " << _conditional_pf[bin]
             << "\n**********\n";
  }
  std::vector<Real> fitted_vals = MastodonUtils::maximizeLogLikelihood(_im,
                                                                       _conditional_pf,
                                                                       _median_fragility_limits,
                                                                       _beta_fragility_limits,
                                                                       _num_collapses,
                                                                       _brute_force,
                                                                       _rgd_tolerance,
                                                                       _rgd_gamma,
                                                                       _rgd_numrnd,
                                                                       _rgd_seed);
  _median_fragility[0] = fitted_vals[0];
  _beta_fragility[0] = fitted_vals[1];
  _loglikelihood[0] = MastodonUtils::calcLogLikelihood(
      _im, _conditional_pf, _median_fragility[0], _beta_fragility[0], _num_collapses);
}

std::vector<Real>
Fragility::readDemandsFromSDFiles(unsigned int bin)
{
  _console << "Stochastic demands file exists. Reading from " << *_demand_filename << "\n";
  // Reading stochastic demand vector from the stochastic demands file of
  // the component. The file has m x n demand values, where m is the
  // number of GMs per bin and n is the number of bins.
  std::vector<Real> stoc_demands;
  stoc_demands.resize(_num_samples * _num_gms);
  MooseUtils::DelimitedFileReader stoc_demands_file(*_demand_filename);
  stoc_demands_file.read();
  if ((stoc_demands_file.getNames()).size() != _num_bins)
    mooseError("Error in block '" + name() +
               "'. Number of columns in stochastic demands file is not the "
               "same as the number of bins.");
  stoc_demands = stoc_demands_file.getData("bin_" + std::to_string(bin + 1));
  if (stoc_demands.size() != _num_gms * _num_samples)
    mooseError("Error in block '" + name() +
               "'. Number of rows in stochastic demands file is not the "
               "same as the product of the number of GMs and number of samples.");
  return stoc_demands;
}

std::vector<Real>
Fragility::calcDemandsFromRHFiles(unsigned int bin)
{
  std::vector<Real> demand_sample;
  std::vector<std::vector<Real>> demand_sample_spectrum;
  std::vector<Real> stoc_demands;
  stoc_demands.resize(_num_samples * _num_gms);
  std::vector<Real> demand_time;
  std::string demand_sample_filename;
  Real k = 0;
  for (unsigned int i = 0; i < _num_gms; ++i)
  {
    for (unsigned int j = 0; j < _num_samples; ++j)
    {
      demand_sample_filename = *_master_file + "_out_" + *_hazard_multiapp +
                               MastodonUtils::zeropad(bin * _num_gms + i, _num_bins * _num_gms) +
                               "_" + *_probabilistic_multiapp + std::to_string(j) + ".csv";
      _console << "In block '" + name() + "'. Reading file: " << demand_sample_filename
               << std::endl;
      MooseUtils::DelimitedFileReader demand_sample_file(demand_sample_filename);
      demand_sample_file.read();
      demand_sample = demand_sample_file.getData(*_demand_variable);
      demand_time = demand_sample_file.getData("time");
      demand_sample = MastodonUtils::regularize(
          demand_sample, demand_time, *_dtsim)[1]; // regularize the demand sample
      demand_sample_spectrum =
          MastodonUtils::responseSpectrum(0.01, 100, 401, demand_sample, *_ssc_xi, *_dtsim);
      LinearInterpolation spectraldemand(demand_sample_spectrum[0], demand_sample_spectrum[4]);
      stoc_demands[k] = spectraldemand.sample(*_ssc_freq);
      k++;
    }
  }
  return stoc_demands;
}

// TODO: Currently all stochastic simulations have the same termination time.
//       Add capability to terminate at various times.
// TODO: Add capability to calculate demands from transferring VectorPostprocessor
//       data instead of reading csv outputs.
