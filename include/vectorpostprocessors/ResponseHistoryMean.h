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

#ifndef RESPONSEHISTORYMEAN_H
#define RESPONSEHISTORYMEAN_H

// MOOSE includes
#include "GeneralVectorPostprocessor.h"
#include "ResponseHistoryBuilder.h"

// Forward Declarations
class ResponseHistoryMean;

template <>
InputParameters validParams<ResponseHistoryMean>();

/**
 *  ResponseHistoryMean is a type of NodalVectorPostprocessor computes the mean
 *  response history given a suite of response histories from the
 *  ResponseHistoryBuilder, and outputs them into a csv file.
 */
class ResponseHistoryMean : public GeneralVectorPostprocessor
{
public:
  ResponseHistoryMean(const InputParameters & parameters);
  virtual void initialize() override;
  virtual void initialSetup() override;

  virtual void execute() override;

protected:

  /// Vector containing the time values in the simulation.
  const VectorPostprocessorValue & _tmp; // This is a temporary variable.
  VectorPostprocessorValue & _history_time;

  /// Vector containing the time values in the simulation.
  VectorPostprocessorValue & _history_mean;
  //std::vector<VectorPostprocessorValue *> _history_mean


  /// Vector of pointers to the response histories of different variables at the node.
  const ResponseHistoryBuilder & _builder;

};

#endif
