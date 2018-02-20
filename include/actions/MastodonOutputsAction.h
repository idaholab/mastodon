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

/**
 * This action automatically creates the required objects for creating the
 * requested outputs.
 **/

#ifndef MASTODONOUTPUTSACTION_H
#define MASTODONOUTPUTSACTION_H

#include "Action.h"

class MastodonOutputsAction : public Action
{
public:
  MastodonOutputsAction(const InputParameters & params);

  virtual void act() override;

private:
  void addStressStrainAuxVariables();
  void addStressStrainAuxKernels();

  // stress AuxVariable names
  const std::vector<AuxVariableName> _stress_auxvariables;

  // strain AuxVariable names
  const std::vector<AuxVariableName> _strain_auxvariables;

  // stress and strain tensor sizes based on problem dimension
  const std::vector<std::size_t> _matrix_size;
};

template <>
InputParameters validParams<MastodonOutputsAction>();

#endif // MASTODONOUTPUTSACTION_H
