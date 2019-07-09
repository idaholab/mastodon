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
  std::vector<VectorPostprocessorValue *> _mean_acc;

  /// Vector containing the time values in the simulation.
  const VectorPostprocessorValue & _history_time;

  std::vector<VectorPostprocessorValue *> _history_acc;

  /// Vector of pointers to the response histories of different variables at the node.
  std::vector<VectorPostprocessorValue *> _history;

  /// Vector of names of VPPs output to the csv file in the same order as in _history
  std::vector<std::string> _history_names;

  /// Vector of pointers to the values of the variables at each time step.
  std::vector<const VariableValue *> _variables;

  /// Stores the data for each VPP on the current timestep
  std::vector<Real> _current_data;
};

#endif
