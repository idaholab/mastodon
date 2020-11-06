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

// MOOSE includes
#include "Function.h"
#include "LinearInterpolation.h"

// Forward Declarations
class BaselineCorrection;

/**
 * Applies a baseline correction to an accceleration time history using least squares polynomial
 * fits and outputs the adjusted acceleration with linear interpolation.
 */
class BaselineCorrection : public Function
{
public:
  static InputParameters validParams();

  BaselineCorrection(const InputParameters & parameters);

  virtual Real value(Real t, const Point & /*P*/) const override;

protected:
  /// Newmark integration parameters
  const Real & _gamma;
  const Real & _beta;

  /// order used for the least squares polynomial fit
  unsigned int _order;

  /// acceleration time history variables from input
  std::vector<Real> _time;
  std::vector<Real> _accel;

  /// adjusted (corrected) acceleration ordinates
  std::vector<Real> _adj_accel;

  /// linear interpolation object is applied over adjusted acceleration, i.e., AFTER correction
  std::unique_ptr<LinearInterpolation> _linear_interp;

  /// function value scale factor - final output is scale_factor * _linear_interp(_time, _adj_accel)
  const Real & _scale_factor;

private:
  /// Applies baseline correction to raw acceleration time history
  void applyCorrection();

  /// Reads and builds data from supplied CSV file
  void buildFromFile();

  /// Builds data from pairs of `time_values` and `acceleration_values'
  void buildFromXandY();
};
