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
#ifndef SEISMICDISPLACEMENTACTION_H
#define SEISMICDISPLACEMENTACTION_H

// MOOSE includes
#include "Action.h"

class SeismicDisplacementAction;

template <> InputParameters validParams<SeismicDisplacementAction>();

/**
 * This action is used to apply seismic input by using PresetDisplacement
 *object.
 **/
class SeismicDisplacementAction : public Action {
public:
  SeismicDisplacementAction(const InputParameters &params);

  virtual void act() override;

protected:
  ///@{
  /// Storage for the supplied input parameters
  const std::vector<BoundaryName> &_boundary;
  const std::vector<VariableName> &_displacements;
  const std::vector<VariableName> &_accelerations;
  const std::vector<VariableName> &_velocities;
  const std::vector<unsigned int> &_input_components;
  const std::vector<FunctionName> &_input_functions;
  ///@}
};

#endif // SEISMICDISPLACEMENTACTION_H
