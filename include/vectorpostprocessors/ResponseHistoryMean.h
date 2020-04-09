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

#pragma once

// MOOSE includes
#include "GeneralVectorPostprocessor.h"

// MASTODON includes
#include "ResponseHistoryBuilder.h"

/**
 *  ResponseHistoryMean is a type of NodalVectorPostprocessor computes the mean
 *  response history given a suite of response histories from the
 *  ResponseHistoryBuilder, and outputs them into a csv file.
 */
class ResponseHistoryMean : public GeneralVectorPostprocessor
{
public:
  static InputParameters validParams();
  ResponseHistoryMean(const InputParameters & parameters);
  virtual void initialize() override;
  virtual void execute() override;

protected:
  /// Times from ResponseHistoryBuilder, used to set the times for this VPP
  const VectorPostprocessorValue & _builder_time;

  /// Storage for the times of response history mean
  VectorPostprocessorValue & _history_time;

  /// Vector containing the time values in the simulation.
  VectorPostprocessorValue & _history_mean;

  /// Vector of pointers to the response histories of different variables at the node.
  const ResponseHistoryBuilder & _builder;
};
