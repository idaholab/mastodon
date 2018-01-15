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

#ifndef MASTODONADDKERNELSACTION_H
#define MASTODONADDKERNELSACTION_H

#include "Action.h"

class MastodonAddKernelsAction : public Action
{
public:
  MastodonAddKernelsAction(const InputParameters & params);

  virtual void act() override;
};

template <>
InputParameters validParams<MastodonAddKernelsAction>();

#endif // MASTODONADDKERNELSACTION_H
