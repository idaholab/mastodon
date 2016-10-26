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
#ifndef SEISMICINPUTACTION_H
#define SEISMICINPUTACTION_H

#include "Action.h"

/**
 * This action is used to apply seismic input on a set of boundaries in a model.
 * The seismic input can be applied in two ways:
 *
 * If the ground motion is an outcrop motion, then it is applied using SeismicForce boundary condition.
 * This requires the ground motion velocity as input.
 *
 * If the ground motion is a within-soil motion, then it is applied using FunctionDirichletBC.
 * This requires the ground displacement as input.
 **/

class SeismicInputAction: public Action
{
public:
  SeismicInputAction(const InputParameters & params);

  virtual void act() override;
};

template<>
InputParameters validParams<SeismicInputAction>();

#endif // SEISMICINPUTACTION_H
