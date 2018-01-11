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
}

#endif
