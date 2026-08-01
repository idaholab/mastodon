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

// This code was implemented in collaboration with Christopher J. Wong (chris.wong@utah.edu) from
// the University of Utah as part of NEUP Project: 17-12939.

#pragma once

#include "PiecewiseBase.h"
#include "FunctionInterface.h"

class BaselineCorrection;

/**
 * Applies a baseline correction to an acceleration time history using the type-oriented algorithm
 * provided by Wong and Ibarra (2022) and evaluates the corrected ordinates of a kinematic variable.
 */
class BaselineCorrection : public PiecewiseBase, protected FunctionInterface
{
public:
  static InputParameters validParams();

  BaselineCorrection(const InputParameters & parameters);

  virtual Real value(Real t, const Point & /*p*/) const override;
  virtual void setData(const std::vector<Real> & t, const std::vector<Real> & u) override;

protected:
  /**
   * Helper function that computes the least squares polynomial of a kinematic time series and fills
   * the container 'c' with its coefficients. The basis depends on the 'nmin' argument, which serves
   * to guarantee that the minimum differentiability of a velocity or displacement fit is C1 or C2,
   * respectively. See the 'applyAdjustment()' definition for the exact form of the polynomials.
   */
  void fitTimeSeries(std::vector<Real> * c,
                     const int & nmin,
                     const std::vector<Real> & tau,
                     const std::vector<Real> & u,
                     const Real & jmap,
                     const Real & rtol = 1e-04);

  /**
   * Helper function for adjusting the acceleration, velocity, and displacement time histories by
   * polynomials of kinematically consistent bases multiplied by the provided coefficients 'c'.
   */
  void applyAdjustment(const std::vector<Real> & c,
                       const std::vector<Real> & tau,
                       std::vector<Real> & accel,
                       std::vector<Real> & vel,
                       std::vector<Real> & disp,
                       const Real & jmap);

  /// Object to perform a piecewise linear interpolation of the corrected time series data
  LinearInterpolation _corrected_series;
};
