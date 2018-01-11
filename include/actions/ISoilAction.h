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

// This code was implemented in colloboration with Ozgun Numanoglu
// (numanog2@illinois.edu) and Omar Baltaji (baltaji2@illinois.edu) from UIUC.

/**
 * This action sets up the I-Soil material model. The stress-strain backbone *curve can either be
 * provided by the user or can be automatically generated using the Darendeli GQ/H method. This
 * stress-strain curve is then divided into multiple elastic-perfectly plastic stress-strain curves
 * and the information corresponding to each elastic-perfectly plastic curve is passed as input to
 * the ComputeISoilStress.C material model
 **/

#ifndef ISOILACTION_H
#define ISOILACTION_H

#include "Action.h"

class ISoilAction : public Action
{
public:
  ISoilAction(const InputParameters & params);

  virtual void act() override;

private:
};

template <>
InputParameters validParams<ISoilAction>();

#endif // ISOILACTION_H
