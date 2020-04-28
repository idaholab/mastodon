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
#ifndef SEISMICFORCEACTION_H
#define SEISMICFORCEACTION_H

// MOOSE includes
#include "Action.h"



/**
 * This action is used to apply seismic input on a set of boundaries using
 *SeismicForce boundary condition.
 **/
class SeismicForceAction : public Action
{
public:
  static InputParameters validParams();
  SeismicForceAction(const InputParameters & params);

  virtual void act() override;

protected:
  ///@{
  /// Storage for the supplied input parameters
  const std::vector<BoundaryName> & _boundary;
  const std::vector<VariableName> & _displacements;
  const std::vector<unsigned int> & _input_components;
  const std::vector<VariableName> * _velocity_ptrs;
  const std::vector<FunctionName> * _velocity_function_ptrs;
  ///@}
};

#endif // SEISMICINPUTACTION_H
