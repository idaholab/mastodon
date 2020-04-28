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

#ifndef HOUSNERSPECTRUMINTENSITY_H
#define HOUSNERSPECTRUMINTENSITY_H

// MOOSE includes
#include "GeneralVectorPostprocessor.h"

/**
 *  HousnerSpectrumIntensity is a type of GeneralVectorPostprocessor that computes the
 *  Housner Spectrum Intensity (HSI) for the given acceleration variables from a
 *  ResponseHistoryBuilder vectorpostprocessor. The HSIs for the variables are
 *  output to a csv file in the same order of the variables in the .i file. The
 *  Housner Spectrum Intensity is defined as the area below the velocity
 *  response spectrum between 0.25sec and 2.5 sec. Refer to Housner (1952)
 *  for details.
 */

class HousnerSpectrumIntensity : public GeneralVectorPostprocessor
{
public:
  static InputParameters validParams();
  HousnerSpectrumIntensity(const InputParameters & parameters);
  virtual void initialSetup() override;
  virtual void initialize() override;
  virtual void execute() override;

protected:
  /// Damping ratio.
  const Real & _xi;

  /// Start period for the HSI calculation. Housner (1952) recommends 0.25sec.
  const Real & _per_start;

  /// End period for the HSI calculation. Housner (1952) recommends 2.5sec.
  const Real & _per_end;

  /// Number of periods for the HSI calculation. The periods are distributed
  /// uniformly in the log scale.
  const unsigned int & _per_num;

  /// dt for HSI calculation. This dt will also be used to
  /// regularize the acceleration response to a uniform dt before calculating
  /// the response spectra for HSI.
  const Real & _reg_dt;

  /// Vector containing the HSIs for all the variables. This vector is ouput
  /// into a csv file along with the variable names.
  std::vector<VectorPostprocessorValue *> _hsi_vec;

  /// Acceleration histories corresponding to the variables for which,
  /// HSIs are requested.
  std::vector<const VectorPostprocessorValue *> _history_acc;

  /// Vector containing the time values in the simulation.
  const VectorPostprocessorValue & _history_time;
};

#endif
