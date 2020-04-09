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
 * This action automatically creates the displacement, velocity and acceleration
 * Variables, AuxVariables, inertia kernels, and the DynamicTensorMechanicsAction
 * automatically based on the dimension of the mesh and if the analysis is
 * static or dynamic. This action also provides the option to add Rayleigh
 * damping.
 **/

#ifndef MASTODONMODELACTION_H
#define MASTODONMODELACTION_H

#include "Action.h"

class MastodonModelAction : public Action
{
public:
  static InputParameters validParams();
  MastodonModelAction(const InputParameters & params);

  virtual void act() override;

private:
  void addDynamicTensorMechanicsAction();
  void addDisplacementVariables();
  void addVelAccelAuxVariables();
  void addInertiaKernels();
  void addVelAccelAuxKernels();

  // Displacement Variable names
  const std::vector<NonlinearVariableName> _disp_variables;

  // Velocity AuxVariable names
  const std::vector<AuxVariableName> _vel_auxvariables;

  // Acceleration AuxVariable names
  const std::vector<AuxVariableName> _accel_auxvariables;

  // Mesh dimension
  const unsigned int _dim;

  // Switch for performing calculations with displaced mesh
  const bool _use_displaced_mesh;
};


#endif // MASTODONMODELACTION_H
