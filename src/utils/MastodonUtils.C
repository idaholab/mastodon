// System includes
#include <glob.h>

// STL includes
#include <cmath>

// MOOSE contrib
#include "pcrecpp.h"
#include "tinydir.h"

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
  for (unsigned int i = 0; i < history_time.size() - 1; ++i)
  {
    while (cur_tme >= history_time[i] && cur_tme <= history_time[i + 1])
    {
      cur_acc = history_acc[i] + (cur_tme - history_time[i]) /
                                     (history_time[i + 1] - history_time[i]) *
                                     (history_acc[i + 1] - history_acc[i]);
      reg_acc.push_back(cur_acc);
      reg_tme.push_back(cur_tme);
      cur_tme += reg_dt;
    }
  }
  return {reg_tme, reg_acc};
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
