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
#ifndef SEISMICSOURCEACTION_H
#define SEISMICSOURCEACTION_H

#include "Action.h"

/**
 * This Action sets up the seismic source dirac kernel in multiple directions
 **/
class SeismicSourceAction : public Action {
public:
  SeismicSourceAction(const InputParameters &params);

  virtual void act() override;
};

template <> InputParameters validParams<SeismicSourceAction>();

#endif // SEISMICSOURCEACTION_H
