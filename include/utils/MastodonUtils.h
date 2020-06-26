/*************************************************/
/*           DO NOT MODIFY THIS HEADER           */
/*                                               */
/*                     MASTODON                  */
/*                                               */
/*    (c) 2015 Battelle Energy Alliance, LLC     */
/*            ALL RIGHTS RESERVED                */
/*                                               */
/*   Prepared by Battelle Energy Alliance, LLC   */
/*     With the U. S. Department of Energy       */
/*                                               */
/*     See COPYRIGHT for full restrictions       */
/*************************************************/

#ifndef MASTODONUTILS_H
#define MASTODONUTILS_H

// MOOSE includes
#include "GeneralVectorPostprocessor.h"

// Forward Declarations
namespace MastodonUtils
{
/**
 *  The responseSpectrum function calculates the response spectrum for a
 *  given acceleration history.
 */
std::vector<std::vector<Real>> responseSpectrum(const Real & freq_start,
                                                const Real & freq_end,
                                                const unsigned int & freq_num,
                                                const std::vector<Real> & history_acc,
                                                const Real & xi,
                                                const Real & reg_dt);

/**
 *  The regularize function inputs a vector or a vector of vectors (with the
 *  first vector being time) and outputs a vector with a constant dt. The
 *  input time vector has to be monotonically increasing.
 */
std::vector<std::vector<Real>> regularize(const std::vector<Real> & history_acc,
                                          const std::vector<Real> & history_time,
                                          const Real & reg_dt);

/**
 * Function to check if all the vectors in a vector are of equal size
 */
bool checkEqualSize(const std::vector<std::vector<Real>> & vectors);

bool checkEqualSize(const std::vector<std::vector<Real> *> & vectors);

/**
 * Function to check if two vectors are equal
 */
bool checkEqual(const std::vector<Real> & vector1,
                const std::vector<Real> & vector2,
                const Real percent_error = 0.0);

/**
 * Function to check if the vector contains any non-positive elements
 */
bool isNegativeOrZero(const std::vector<Real> & vector);

/**
 * Function to calculate the mean of the elements in a vector
 */
Real mean(const std::vector<Real> & vector);

/**
 * Function to calculate the mean vector of the elements in a vector of pointers
 */

std::vector<Real> mean(const std::vector<std::vector<Real> *> & history_acc);

/**
 * Function to calculate the median of the elements in a vector
 */
Real median(const std::vector<Real> & vector, const std::string & interpolation = "linear");

/**
 * Function to calculate the percentile value of the elements in a vector
 */
Real percentile(const std::vector<Real> & vector,
                const Real & percent,
                const std::string & interpolation = "linear");

/**
 * Function to calculate the standard deviation of the elements in a vector
 */
Real standardDeviation(const std::vector<Real> & vector);

/**
 * Function to calculate the lognormal standard deviation of the elements in a vector
 */
Real lognormalStandardDeviation(const std::vector<Real> & vector);

/**
 * Function to calculate the probability that one random variable is greater
 * than another random variable
 */
Real greaterProbability(Real demand_median,
                        Real demand_scale,
                        Real capacity_median,
                        Real capacity_scale);

/**
 * Function to calculate the loglikelihood (for MLE fiting) given the data
 * (im and pf) and the location and scale parameters of a lognormal distribution.
 */
Real calcLogLikelihood(const std::vector<Real> & im,
                       const std::vector<Real> & pf,
                       const Real & loc,
                       const Real & sca,
                       const unsigned int & n);

/**
 * Function to calculate location and scale parameters of a lognormal
 * distribution that maximizes the loglikelihood of fitting the data im and pf
 */

std::vector<Real> maximizeLogLikelihood(const std::vector<Real> & im,
                                        const std::vector<Real> & pf,
                                        const std::vector<Real> & loc_space,
                                        const std::vector<Real> & sca_space,
                                        const unsigned int & n,
                                        const bool & brute_force,
                                        const Real tolerance,
                                        const Real gamma,
                                        const int num_rnd,
                                        const int seed);

/**
 * This function zeropads the number n with zeros in the beginning and makes n
 * the same number of digits as n_tot. The function returns the zeropadded string.
 */
std::string zeropad(const unsigned int n, const unsigned int n_tot);

/**
 * Return a list of numbered files from a base string.
 */
std::vector<std::string> glob(const std::string & pattern);

/**
 * Shift and scale std::vector components.
 */
std::vector<Real> adjust(const std::vector<Real> & input, const Real & scale, const Real & offset);

/**
 * Compute log (base 10) of the supplied vector entries.
 */
std::vector<Real> log10(const std::vector<Real> & input);

} // namespace MastodonUtils
#endif
