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
 * fits and outputs adjusted values for the specified kinematic variable.
 */
class BaselineCorrection : public Function
{
public:
  static InputParameters validParams();

  BaselineCorrection(const InputParameters & parameters);

  virtual Real value(Real t, const Point & /*P*/) const override;

protected:
  /// adjusted time series to evaluate - can be 'acceleration', 'velocity', or 'displacement'
  const MooseEnum _series;

  /// Newmark integration parameters
  const Real & _gamma;
  const Real & _beta;

  /// acceleration time history variables from input
  std::vector<Real> _time;
  std::vector<Real> _accel;

  /// vector storing adjusted (corrected) time series values
  std::vector<Real> _adj_series;

  /// linear interpolation object is applied over adjusted acceleration, i.e., AFTER correction
  std::unique_ptr<LinearInterpolation> _linear_interp;

  /// function multiplier - final output is 'scale_factor * _linear_interp(_time, _adj_series)'
  const Real & _scale_factor;

private:
  /// Applies baseline correction to raw acceleration and copies adjusted ordinates to '_adj_series'
  void applyCorrection();

  /// Reads and builds data from supplied file using MooseUtils::DelimitedFileReader()
  void buildFromFile();

  /// Builds data from pairs of `time_values` and `acceleration_values'
  void buildFromXandY();
};
