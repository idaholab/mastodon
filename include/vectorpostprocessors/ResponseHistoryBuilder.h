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

#ifndef RESPONSEHISTORYBUILDER_H
#define RESPONSEHISTORYBUILDER_H

// MOOSE includes
#include "NodalVectorPostprocessor.h"

// Forward Declarations
class ResponseHistoryBuilder;

template <>
InputParameters validParams<ResponseHistoryBuilder>();

/**
 *  ResponseHistoryBuilder is a type of NodalVectorPostprocessor that builds the
 *  response histories of a nodal response such as acceleration, velocity,
 *  displacement, etc., and outputs them into a csv file.
 */
class ResponseHistoryBuilder : public NodalVectorPostprocessor
{
public:
  /**
    * Class constructor
    * @param parameters The input parameters
    */
  ResponseHistoryBuilder(const InputParameters & parameters);

  virtual void initialize() override;

  virtual void threadJoin(const UserObject & uo) override;

  /**
   * Builds response histories
   */
  virtual void execute() override;

protected:
  /// Reference to the time vector from the analysis. Will be output in the first column of the csv file.
  VectorPostprocessorValue & _history_time;

  /// Dummy variable for current time.
  Real _cur_time;

  /// Vector of pointers to the response histories of different variables at the node.
  std::vector<VectorPostprocessorValue *> _history;

  /// Vector of pointers to the values of the variables at each time step.
  std::vector<const VariableValue *> _variables;

  /// Reference to the node number where the response histories are requested.
  const unsigned int & _node;
};

#endif
