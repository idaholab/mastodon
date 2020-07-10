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

// This code was implemented in collaboration with Christopher J. Wong
// (chris.wong@utah.edu) from the University of Utah.

#pragma once

// LIBMESH includes
#include "DenseMatrix.h"
#include "libmesh/dense_vector.h"

/**
 * This namespace contains the functions used for the calculations corresponding
 * to the time history adjustment procedure in BaselineCorrection
 **/
namespace BaselineCorrectionUtils
{
/// Evaluates an integral over a single time step with Newmark-beta method
/// Also is used as simple trapezoidal rule when gamma = 0.5.
Real newmarkGammaIntegrate(const Real & u_ddot_old,
                           const Real & u_ddot,
                           const Real & u_dot_old,
                           const Real & gamma,
                           const Real & dt);

/// Evaluates a double integral over a single time step with Newmark-beta method
Real newmarkBetaIntegrate(const Real & u_ddot_old,
                          const Real & u_ddot,
                          const Real & u_dot_old,
                          const Real & u_old,
                          const Real & beta,
                          const Real & dt);

/// Solves linear normal equation for minimum acceleration square error
DenseVector<Real> getAccelerationFitCoeffs(unsigned int order,
                                           const std::vector<Real> & accel,
                                           const std::vector<Real> & t,
                                           const unsigned int & num_steps,
                                           const Real & gamma);

/// Solves linear normal equation for minimum velocity square error
DenseVector<Real> getVelocityFitCoeffs(unsigned int order,
                                       const std::vector<Real> & accel,
                                       const std::vector<Real> & vel,
                                       const std::vector<Real> & t,
                                       const unsigned int & num_steps,
                                       const Real & beta);

/// Solves linear normal equation for minimum displacement square error
DenseVector<Real> getDisplacementFitCoeffs(unsigned int order,
                                           const std::vector<Real> & disp,
                                           const std::vector<Real> & t,
                                           const unsigned int & num_steps);

/// Evaluates the least squares polynomials over at a single time step
std::vector<Real>
computePolynomials(unsigned int order, const DenseVector<Real> & coeffs, const Real & t);

} // namespace BaselineCorrectionUtils
