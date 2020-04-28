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
#ifndef FUNCTIONPOINTFORCE_H
#define FUNCTIONPOINTFORCE_H

// Moose Includes
#include "DiracKernel.h"

// Forward Declarations
class Function;

/**
 * This class applies a force at the given point/points in a given direction.
 * The force can be given as a function of space and/or time.
 **/
class FunctionPointForce : public DiracKernel
{
public:
  static InputParameters validParams();
  FunctionPointForce(const InputParameters & parameters);

  virtual void addPoints() override;

  virtual Real computeQpResidual() override;

protected:
  /// location of the point force
  Point _p;
};

#endif // FUNCTIONPOINTFORCE_H
