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
#ifndef NONREFLECTINGBCACTION_H
#define NONREFLECTINGBCACTION_H

#include "Action.h"

/**
 * Creates NonReflectingBC object for each displacement direction.
 **/
class NonReflectingBCAction : public Action
{
public:
  static InputParameters validParams();
  NonReflectingBCAction(const InputParameters & params);

  virtual void act() override;
};


#endif // NONREFLECTINGBCACTION_H
