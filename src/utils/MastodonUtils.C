// System includes
#include <glob.h>

// STL includes
#include <cmath>
#include <numeric>

// MOOSE contrib
#include "pcrecpp.h"
#include "tinydir.h"
#include "MooseUtils.h"
#include "MooseRandom.h"

// MASTODON includes
#include "MastodonUtils.h"

std::vector<std::vector<Real>>
MastodonUtils::responseSpectrum(const Real & freq_start,
                                const Real & freq_end,
                                const unsigned int & freq_num,
                                const std::vector<Real> & history_acc,
                                const Real & xi,
                                const Real & reg_dt)
{
  std::vector<Real> freq_vec, aspec_vec, vspec_vec, dspec_vec;
  Real logdf, om_n, om_d, dt2, dis1, vel1, acc1, dis2, vel2, acc2, pdmax, kd;
  for (std::size_t n = 0; n < freq_num; ++n)
  {
    // Building the frequency vector. Frequencies are distributed
    // uniformly in the log scale.
    logdf = (std::log10(freq_end) - std::log10(freq_start)) / (freq_num - 1);
    freq_vec.push_back(pow(10.0, std::log10(freq_start) + n * logdf));
    om_n = 2.0 * 3.141593 * freq_vec[n]; // om_n = 2*pi*f
    om_d = om_n * xi;
    dis1 = 0.0;
    vel1 = 0.0;
    dt2 = reg_dt * reg_dt;
    pdmax = 0.0;
    acc1 = -1.0 * history_acc[0] - 2.0 * om_d * vel1 - om_n * om_n * dis1;
    kd = 1.0 + om_d * reg_dt + dt2 * om_n * om_n / 4.0;
    for (std::size_t j = 0; j < history_acc.size(); ++j)
    {
      dis2 = ((1.0 + om_d * reg_dt) * dis1 + (reg_dt + 1.0 / 2.0 * om_d * dt2) * vel1 +
              dt2 / 4.0 * acc1 - dt2 / 4.0 * history_acc[j]) /
             kd;
      acc2 = 4.0 / dt2 * (dis2 - dis1) - 4.0 / reg_dt * vel1 - acc1;
      vel2 = vel1 + reg_dt / 2.0 * (acc1 + acc2);
      if (std::abs(dis2) > pdmax)
        pdmax = std::abs(dis2);
      dis1 = dis2;
      vel1 = vel2;
      acc1 = acc2;
    }
    dspec_vec.push_back(pdmax);
    vspec_vec.push_back(pdmax * om_n);
    aspec_vec.push_back(pdmax * om_n * om_n);
  }
  return {freq_vec, dspec_vec, vspec_vec, aspec_vec};
}

std::vector<std::vector<Real>>
MastodonUtils::regularize(const std::vector<Real> & history_acc,
                          const std::vector<Real> & history_time,
                          const Real & reg_dt)
{
  std::vector<Real> reg_acc;
  std::vector<Real> reg_tme;
  Real cur_tme = history_time[0];
  Real cur_acc;
  for (std::size_t i = 0; i < history_time.size() - 1; ++i)
  {
    while (cur_tme >= history_time[i] && cur_tme <= history_time[i + 1])
    {
      cur_acc = history_acc[i] +
                (cur_tme - history_time[i]) / (history_time[i + 1] - history_time[i]) *
                    (history_acc[i + 1] - history_acc[i]);
      reg_acc.push_back(cur_acc);
      reg_tme.push_back(cur_tme);
      cur_tme += reg_dt;
    }
  }
  return {reg_tme, reg_acc};
}

bool
MastodonUtils::checkEqualSize(const std::vector<std::vector<Real>> & vectors)
{
  for (const auto & v : vectors)
  {
    if (v.size() != vectors[0].size())
      return false;
  }
  return true;
}

bool
MastodonUtils::checkEqualSize(const std::vector<std::vector<Real> *> & vectors)
{
  for (std::size_t i = 0; i < vectors.size(); i++)
  {
    if ((*vectors[i]).size() != (*vectors[0]).size())
      return false;
  }
  return true;
}

bool
MastodonUtils::checkEqual(const std::vector<Real> & vector1,
                          const std::vector<Real> & vector2,
                          const Real percent_error)
{
  if (vector1.size() != vector2.size())
    return false;
  for (std::size_t i = 0; i < vector1.size(); ++i)
  {
    if (!MooseUtils::absoluteFuzzyEqual(
            vector1[i], vector2[i], std::abs(vector1[i] * percent_error / 100)))
      return false;
  }
  return true;
}

bool
MastodonUtils::isNegativeOrZero(const std::vector<Real> & vector)
{
  for (const auto & element : vector)
    if (element <= 0)
      return true;
  return false;
}

Real
MastodonUtils::mean(const std::vector<Real> & vector)
{
  Real sum = std::accumulate(vector.begin(), vector.end(), 0.0);
  return sum / vector.size();
}

std::vector<Real>
MastodonUtils::mean(const std::vector<std::vector<Real> *> & history_acc)
{
  // bool check = ;
  if (MastodonUtils::checkEqualSize(history_acc) < 1)
    mooseError("Input vectors are all not of equal size.");
  else
  {
    std::vector<Real> mean_acc;
    mean_acc.resize((*history_acc[0]).size());
    Real tmp_var = 0;
    for (std::size_t i = 0; i < (*history_acc[0]).size(); i++)
    {
      for (std::size_t j = 0; j < history_acc.size(); j++)
      {
        tmp_var = tmp_var + (*history_acc[j])[i];
      }
      mean_acc[i] = tmp_var / history_acc.size();
      tmp_var = 0;
    }
    return mean_acc;
  }
}

Real
MastodonUtils::median(const std::vector<Real> & vector, const std::string & interpolation)
{
  std::vector<Real> sorted_vector = vector;
  std::sort(sorted_vector.begin(), sorted_vector.end());
  if (sorted_vector.size() % 2 != 0)
    return sorted_vector[(sorted_vector.size() - 1) / 2];
  else if (interpolation == "linear")
    return (sorted_vector[sorted_vector.size() / 2] + sorted_vector[sorted_vector.size() / 2 - 1]) /
           2.0;
  else if (interpolation == "lower")
    return sorted_vector[sorted_vector.size() / 2 - 1];
  else if (interpolation == "higher")
    return sorted_vector[sorted_vector.size() / 2];
  else
    mooseError("Invalid interpolation type in median calculation.");
}

Real
MastodonUtils::percentile(const std::vector<Real> & vector,
                          const Real & percent,
                          const std::string & interpolation)
{
  std::vector<Real> sorted_vector = vector;
  std::sort(sorted_vector.begin(), sorted_vector.end());
  if (percent < 0.0 || percent > 100.0)
    mooseError("Percent should be between 0 and 100.\n");
  std::size_t low_index;
  if (floor(percent / 100 * sorted_vector.size()) == 0)
    low_index = 0;
  else
    low_index = floor(percent / 100 * sorted_vector.size()) - 1;
  if (interpolation == "lower")
    return sorted_vector[low_index];
  else if (interpolation == "higher")
    return sorted_vector[low_index + 1];
  else if (interpolation == "linear")
  {
    Real index_remainder = fmod(percent / 100.0 * sorted_vector.size(), 1.0);
    Real percentile_value =
        sorted_vector[low_index] +
        index_remainder * (sorted_vector[low_index + 1] - sorted_vector[low_index]);
    return percentile_value;
  }
  else
    mooseError("Invalid interpolation type in percentile calculation.");
}

Real
MastodonUtils::standardDeviation(const std::vector<Real> & vector)
{
  Real sum = 0;
  for (std::size_t i = 0; i < vector.size(); i++)
  {
    sum += (vector[i] - MastodonUtils::mean(vector)) * (vector[i] - MastodonUtils::mean(vector));
  }
  Real sigma = sqrt(sum / (vector.size() - 1));
  return sigma;
}

Real
MastodonUtils::lognormalStandardDeviation(const std::vector<Real> & vector)
{
  if (MastodonUtils::isNegativeOrZero(vector))
    mooseError("One or more elements in the sample for calculating beta are non positive.\n");
  std::vector<Real> logvector;
  for (const auto & element : vector)
    logvector.push_back(log(element));
  return MastodonUtils::standardDeviation(logvector);
}

std::string
MastodonUtils::zeropad(const unsigned int n, const unsigned int n_tot)
{
  std::size_t zeropadsize = (std::to_string(n_tot)).length() - (std::to_string(n)).length();
  std::string pad = "";
  for (std::size_t i = 0; i < zeropadsize; i++)
  {
    pad += "0";
  }
  return pad + std::to_string(n);
}

std::vector<std::string>
MastodonUtils::glob(const std::string & pattern)
{
  glob_t glob_result;
  glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);
  std::vector<string> output;
  for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
    output.push_back(string(glob_result.gl_pathv[i]));
  globfree(&glob_result);
  return output;
}

std::vector<Real>
MastodonUtils::adjust(const std::vector<Real> & input, const Real & scale, const Real & offset)
{
  std::vector<Real> output(input.size());
  for (std::size_t i = 0; i < input.size(); ++i)
    output[i] = scale * input[i] + offset;
  return output;
}

std::vector<Real>
MastodonUtils::log10(const std::vector<Real> & input)
{
  std::vector<Real> output(input.size());
  for (std::size_t i = 0; i < input.size(); ++i)
  {
    if (input[i] <= 0)
      ::mooseError("Cannot take the log of ", input[i], ".");
    output[i] = std::log10(input[i]);
  }
  return output;
}

Real
MastodonUtils::greaterProbability(Distribution & demand_distribution,
                                  Distribution & capacity_distribution)
{
  Real min_demand = demand_distribution.quantile(0.001); //~ -3 sigma for normal distributions
  Real max_demand = demand_distribution.quantile(0.999); //~ +3 sigma for normal distributions
  Real prob = 0.0;
  Real param = min_demand;
  Real p_1, p_2;
  Real delta = demand_distribution.median() / 1000;
  while (param < max_demand)
  {
    p_1 = demand_distribution.pdf(param) * capacity_distribution.cdf(param);
    p_2 = demand_distribution.pdf(param + delta) * capacity_distribution.cdf(param + delta);
    prob += delta * (p_1 + p_2) / 2;
    param += delta;
  }
  return prob;
}

// Utils that require external Boost
#ifdef LIBMESH_HAVE_EXTERNAL_BOOST
Real
MastodonUtils::calcLogLikelihood(const std::vector<Real> & im,
                                 const std::vector<Real> & pf,
                                 const Real & loc,
                                 const Real & sca,
                                 const unsigned int & n)
{
  // error check
  if (im.size() != pf.size())
    mooseError("While calculating loglikelihood, intensity measure and failure probability vectors "
               "should be of the same size.");
  if (MastodonUtils::isNegativeOrZero(im) || MastodonUtils::isNegativeOrZero(pf))
    mooseError("While calculating loglikelihood, intensity measure or failure probability has a "
               "non-positive value.");
  for (std::size_t i = 0; i < pf.size(); ++i)
  {
    if (pf[i] > 1.0)
      mooseError("While calculating loglikelihood, a value greater than 1 is found in the failure "
                 "probability vector.");
  }
  if (sca <= 0)
    mooseError("While calculating loglikelihood, scale parameter should be positive.");
  if (loc <= 0)
    mooseError("While calculating loglikelihood, location parameter should be positive.");
  // Calculating the likelihood at each IM value
  boost::math::lognormal_distribution<Real> fragility_fit(
      log(loc), sca); // Initial estimate of fragility fit from the seed values
  Real loglikelihood = 0;
  for (std::size_t i = 0; i < im.size(); ++i)
  {
    // Binomial pdf in the below calculation is made in the log scale due to
    // numerical errors (+inf) in calculating nCr for large trial sizes (n).
    // Calculating log10(nCr)
    unsigned int r = floor(n * pf[i]);
    Real log10_nCr = 0.0;
    for (std::size_t k = 1; k <= r; k++)
      log10_nCr += std::log10(n - r + k) - std::log10(k);
    Real p = boost::math::cdf(fragility_fit, im[i]);
    // calculating sum of loglikelihoods. Each loglikelihood is the log(binomial pdf),
    // which ends up to be the summation below.
    loglikelihood += log10_nCr + r * std::log10(p) + (n - r) * std::log10(1.0 - p);
  }
  return loglikelihood;
}

std::vector<Real>
MastodonUtils::maximizeLogLikelihood(const std::vector<Real> & im,
                                     const std::vector<Real> & pf,
                                     const std::vector<Real> & loc_space,
                                     const std::vector<Real> & sca_space,
                                     const unsigned int & n,
                                     const bool & brute_force,
                                     const Real tolerance,
                                     const Real gamma,
                                     const int num_rnd,
                                     const int seed)
{
  std::vector<Real> params_return = {0, 0};
  if (brute_force)
  {
    Real loglikelihoodmax = MastodonUtils::calcLogLikelihood(im, pf, loc_space[0], sca_space[0], n);
    for (Real loc = loc_space[0]; loc < loc_space[1]; loc += 0.01)
      for (Real sca = sca_space[0]; sca < sca_space[1]; sca += 0.01)
        if (MastodonUtils::calcLogLikelihood(im, pf, loc, sca, n) >= loglikelihoodmax)
        {
          loglikelihoodmax = MastodonUtils::calcLogLikelihood(im, pf, loc, sca, n);
          params_return = {loc, sca};
        }
  }
  else
  {
    Real loc_rand;
    Real sca_rand;
    std::vector<Real> params_now = {0, 0};     // Initializing new parameter vector here.
    std::vector<Real> params_before = {0, 0};  // Initializing old parameter vector here.
    std::vector<Real> gradient_now = {-1, -2}; // Initializing the gradient vector here.
    // This variable will get updated within each iteration of the Gradient Descent algorithm.
    Real dparam =
        0.01; // Initilizing an arbitrarily small deviation to the random seed parameter vector.
              // Note that Gradient Descent algorithm requires two likelihood values from two seeds.
    Real likelihood_now;           // Initializing a variable.
    Real likelihood_before;        // Initializing a variable.
    Real likelihood_base = -50001; // Initializing to an arbitrarily low value here.
    // This variable will get updated if a parameter vector has greater likelihood.
    MooseRandom::seed(seed); // Setting up the random number generator.
    for (int index = 0; index < num_rnd; index++)
    {
      loc_rand = loc_space[0] + (loc_space[1] - loc_space[0]) * MooseRandom::rand();
      sca_rand = sca_space[0] + (sca_space[1] - sca_space[0]) * MooseRandom::rand();
      likelihood_now = -MastodonUtils::calcLogLikelihood(im, pf, loc_rand, sca_rand, n);
      likelihood_before =
          -MastodonUtils::calcLogLikelihood(im, pf, loc_rand + dparam, sca_rand + dparam, n);
      params_now = {loc_rand, sca_rand};
      params_before = {loc_rand + dparam, sca_rand + dparam};
      if (likelihood_now > likelihood_before)
      {
        likelihood_now =
            -MastodonUtils::calcLogLikelihood(im, pf, loc_rand + dparam, sca_rand + dparam, n);
        likelihood_before = -MastodonUtils::calcLogLikelihood(im, pf, loc_rand, sca_rand, n);
        params_now = {loc_rand + dparam, sca_rand + dparam};
        params_before = {loc_rand, sca_rand};
      }
      while (std::abs(likelihood_now - likelihood_before) > tolerance)
      {
        gradient_now[0] = (likelihood_now - likelihood_before) / (params_now[0] - params_before[0]);
        gradient_now[1] = (likelihood_now - likelihood_before) / (params_now[1] - params_before[1]);
        likelihood_before = likelihood_now;
        params_now[0] = (params_now[0] - gamma * gradient_now[0]);
        params_now[1] = (params_now[1] - gamma * gradient_now[1]);
        if ((params_now[0] < loc_space[0]) || (params_now[0] > loc_space[1]) ||
            (params_now[1] < sca_space[0]) || (params_now[1] > sca_space[1]))
        {
          index = index - 1;
          break;
        }
        else
        {
          likelihood_now =
              -MastodonUtils::calcLogLikelihood(im, pf, params_now[0], params_now[1], n);
          if ((-likelihood_now) > (likelihood_base))
          {
            likelihood_base = -likelihood_now;
            params_return = params_now;
          }
        }
      }
    }
  }
  return params_return;
}

#endif // LIBMESH_HAVE_EXTERNAL_BOOST
