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
 * This action automatically creates the displacement Variables and the
 * velocity, acceleration, stress and strain AuxVariables based on the dimension of the mesh
 * of the problem.
 **/

#ifndef MASTODONADDVARIABLESACTION_H
#define MASTODONADDVARIABLESACTION_H

#include "Action.h"

class MastodonAddVariablesAction : public Action
{
public:
  MastodonAddVariablesAction(const InputParameters & params);

  virtual void act() override;
};

template <>
InputParameters validParams<MastodonAddVariablesAction>();

#endif // MASTODONADDVARIABLESACTION_H
