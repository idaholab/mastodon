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

#pragma once

#include "PiecewiseLinearBase.h"
#include "DataFileInterface.h"

/**
 * Class for Ground Motion Simulation
 */
class GroundMotionSim : public PiecewiseLinearBase

{
public:
  static InputParameters validParams();

  GroundMotionSim(const InputParameters & parameters);

private:
  const Real _scale_factor;
  void parse_output(const char * cmd, std::string & t, std::string & Acc) const;
};
