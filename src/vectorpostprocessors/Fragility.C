// MOOSE includes
#include "Fragility.h"
#include "PostprocessorInterface.h"
#include "VectorPostprocessorInterface.h"
#include "MastodonUtils.h"
#include "LinearInterpolation.h"
#include "DelimitedFileReader.h"
#include <string>

registerMooseObject("MastodonApp", Fragility);

template <>
InputParameters
validParams<Fragility>()
{
  InputParameters params = validParams<GeneralVectorPostprocessor>();
  params.addRequiredParam<std::string>("master_file", "Name of the master file without extension.");
  params.addRequiredParam<std::string>(
      "hazard_multiapp", "Name of the multiapp corresponding to ground motion sampling.");
  params.addRequiredParam<std::string>(
      "probabilistic_multiapp",
      "Name of the multiapp corresponding to the probabilistic simulations.");
  params.addRequiredParam<unsigned int>("num_gms",
                                        "Number of ground motions used in each intensity bin.");
  params.addRequiredParam<std::string>(
      "demand_variable",
      "Demand variable for the SSC that is also column name in the output csv "
      "file. Acceleration variable only."); // TODO: generalize class for all
                                            // demand variables by processing
                                            // VectorPostprocessor data and
                                            // getting rid of response spectra
                                            // calculation in this class.
  params.addRequiredParam<Real>(
      "frequency", "Frequency at which, the spectral demand of the SSC is calculated.");
  params.addParam<Real>(
      "damping_ratio",
      0.05,
      "Damping ratio at which the spectral demand of the SSC is calculated. Default is 0.05");
  params.addRequiredParam<Real>("dt", "dt for response spectra calculation.");
  params.addRequiredParam<Real>(
      "median_capacity",
      "Median capacity of the SSC in terms of local demand at the SSC location.");
  params.addRequiredParam<Real>("beta_capacity", "Uncertainty in the capacity of the SSC.");
  params.addRequiredParam<unsigned int>(
      "num_bins", "Number of bins in the hazard curve where the risk calculation is performed.");
  params.addRequiredParam<unsigned int>("num_samples",
                                        "Number of probabilistic simulations for each bin.");
  params.addRequiredParam<std::vector<Real>>(
      "im_values", "IM values used in the bins."); // TODO: remove this later by transferring IM
                                                   // values from hazard curve UO
  params.addRequiredParam<std::vector<Real>>("median_fragility_limits",
                                             "Limits for median fragility of the component.");
  params.addRequiredParam<std::vector<Real>>(
      "beta_fragility_limits",
      "Limits for the lognormal standard deviation of the component fragility.");
  params.addRequiredParam<std::string>("optimization_method",
                                       "Name of the optimization "
                                       "method for fragility fitting. The following "
                                       "methods are available: Crude (input 'BRUTE FORCE') "
                                       "or Stochastic Gradient Descent (input 'SGD').");
  params.addParam<Real>("sgd_tolerance",
                        1e-05,
                        "Tolerance for declaring convergence of the Stochastic Gradient Descent "
                        "algorithm. Default is 1e-05.");
  params.addParam<Real>("sgd_gamma",
                        0.0001,
                        "Parameter controlling the step size of the Stochastic Gradient Descent "
                        "algorithm. Default is 0.0001.");
  params.addParam<Real>("sgd_numrnd",
                        100,
                        "Number of random initializations in the Stochastic Gradient Descent "
                        "algorithm. Default is 100.");
  params.addParam<Real>("sgd_seed",
                        1028,
                        "Seed for random number generator in the Stochastic Gradient Descent "
                        "algorithm. Default is 1028.");
  params.addClassDescription("Calculate the seismic fragility of an SSC by postprocessing the "
                             "results of a probabilistic or stochastic simulation.");
  return params;
}

Fragility::Fragility(const InputParameters & parameters)
  : GeneralVectorPostprocessor(parameters),
    _master_file(getParam<std::string>("master_file")),
    _hazard_multiapp(getParam<std::string>("hazard_multiapp")),
    _probabilistic_multiapp(getParam<std::string>("probabilistic_multiapp")),
    _num_gms(getParam<unsigned int>("num_gms")),
    _demand_variable(getParam<std::string>("demand_variable")),
    _ssc_freq(getParam<Real>("frequency")),
    _ssc_xi(getParam<Real>("damping_ratio")),
    _dtsim(getParam<Real>("dt")),
    _median_cap(getParam<Real>("median_capacity")),
    _beta_ssc_cap(getParam<Real>("beta_capacity")),
    _num_bins(getParam<unsigned int>("num_bins")),
    _num_samples(getParam<unsigned int>("num_samples")),
    _im_values(getParam<std::vector<Real>>("im_values")),
    _median_fragility_limits(getParam<std::vector<Real>>("median_fragility_limits")),
    _beta_fragility_limits(getParam<std::vector<Real>>("beta_fragility_limits")),
    _im(declareVector("intensity")),
    _median_demand(declareVector("demand_median")),
    _beta_demand(declareVector("demand_beta")),
    _conditional_pf(declareVector("conditional_pf")),
    _median_fragility(declareVector("fragility_median")),
    _beta_fragility(declareVector("fragility_beta")),
    _method(getParam<std::string>("optimization_method")),
    _sgd_tolerance(getParam<Real>("sgd_tolerance")),
    _sgd_gamma(getParam<Real>("sgd_gamma")),
    _sgd_numrnd(getParam<Real>("sgd_numrnd")),
    _sgd_seed(getParam<Real>("sgd_seed"))
{
#ifndef LIBMESH_HAVE_EXTERNAL_BOOST
  mooseError("In Fragility block '",
             name(),
             "'. The Fragility block requires that libMesh be compiled with an external Boost "
             "library, this may be done "
             "using the --with-boost configure option.");
#endif // LIBMESH_HAVE_EXTERNAL_BOOST
  // Check for non-positive SSC frequency
  if (_ssc_freq <= 0)
    mooseError("Error in block '" + name() + "'. SSC frequency must be positive.");
  // Check for non-positive SSC damping
  if (_ssc_xi <= 0)
    mooseError("Error in block '" + name() + "'. SSC damping ratio must be positive.");
  // Check for non-positive median and beta of capacity
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
  if (_method.compare("BRUTE FORCE") != 0 && _method.compare("SGD") != 0)
    mooseError("Optimization method requested does not match any exisiting methods. Request either "
               "BRUTE FORCE or SGD.");
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
}

void
Fragility::execute()
{
#ifdef LIBMESH_HAVE_EXTERNAL_BOOST
  _im.resize(_num_bins);
  _median_demand.resize(_num_bins);
  _beta_demand.resize(_num_bins);
  _conditional_pf.resize(_num_bins);
  _median_fragility.resize(1);
  _beta_fragility.resize(1);
  for (std::size_t bin = 0; bin < _num_bins; bin++)
  {
    _im[bin] = _im_values[bin];
    std::vector<Real> stoc_demands = Fragility::calcDemandsFromFile(
        bin); // calculating demands from csv files if it is a restart PRA
    _median_demand[bin] = MastodonUtils::median(stoc_demands);
    _beta_demand[bin] = MastodonUtils::lognormalStandardDeviation(stoc_demands);
    boost::math::lognormal_distribution<Real> demand_distribution(log(_median_demand[bin]),
                                                                  _beta_demand[bin]);
    boost::math::lognormal_distribution<Real> capacity_distribution(log(_median_cap),
                                                                    _beta_ssc_cap);
    _conditional_pf[bin] = MastodonUtils::greaterProbability<boost::math::lognormal>(
        demand_distribution, capacity_distribution);
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
                                                                       500,
                                                                       _method,
                                                                       _sgd_tolerance,
                                                                       _sgd_gamma,
                                                                       _sgd_numrnd,
                                                                       _sgd_seed);
  _median_fragility[0] = fitted_vals[0];
  _beta_fragility[0] = fitted_vals[1];
#endif // LIBMESH_HAVE_EXTERNAL_BOOST
}

std::vector<Real>
Fragility::calcDemandsFromFile(unsigned int bin)
{
#ifdef LIBMESH_HAVE_EXTERNAL_BOOST
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
      demand_sample_filename = _master_file + "_out_" + _hazard_multiapp +
                               MastodonUtils::zeropad(bin * _num_gms + i, _num_bins * _num_gms) +
                               "_" + _probabilistic_multiapp +
                               MastodonUtils::zeropad(j, _num_samples) + ".csv";

      _console << "In block '" + name() + "'. Reading file: " << demand_sample_filename
               << std::endl;
      MooseUtils::DelimitedFileReader demand_sample_file(demand_sample_filename);
      demand_sample_file.read();
      demand_sample = demand_sample_file.getData(_demand_variable);
      demand_time = demand_sample_file.getData("time");
      demand_sample = MastodonUtils::regularize(
          demand_sample, demand_time, _dtsim)[1]; // regularize the demand sample
      demand_sample_spectrum =
          MastodonUtils::responseSpectrum(0.01, 100, 401, demand_sample, _ssc_xi, _dtsim);
      LinearInterpolation spectraldemand(demand_sample_spectrum[0], demand_sample_spectrum[3]);
      stoc_demands[k] = spectraldemand.sample(_ssc_freq);
      k++;
    }
  }
  return stoc_demands;
#else
  return {}; // return empty vector
#endif // LIBMESH_HAVE_EXTERNAL_BOOST
}

// TODO: Currently all stochastic simulations have the same termination time.
//       Add capability to terminate at various times.
// TODO: Add capability to calculate demands from transferring VectorPostprocessor
//       data instead of reading csv outputs.
