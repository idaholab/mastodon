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

// Mastodon includes
#include "NonReflectingBC.h"
#include "NonReflectingBCAction.h"

template <> InputParameters validParams<NonReflectingBCAction>() {
  InputParameters params = validParams<Action>();
  params.addClassDescription(
      "Set up Non-reflecting boundary conditions in different directions.");
  params += NonReflectingBC::commonParameters();
  params.addRequiredParam<std::vector<BoundaryName>>(
      "boundary", "The list of boundary IDs from the mesh where this boundary "
                  "condition will be applied");
  return params;
}

NonReflectingBCAction::NonReflectingBCAction(const InputParameters &params)
    : Action(params) {}

void NonReflectingBCAction::act() {
  // Apply parameters from the action level to the parameters of a
  // NonReflectingBC object
  InputParameters params = _factory.getValidParams("NonReflectingBC");
  params.applyParameters(parameters());

  // Define the name prefix for the objects to be created
  std::string prefix = "mastodon_non_reflecting_BC";

  // Loop over displacements and add NonReflectingBC object for each variable
  const std::vector<BoundaryName> &boundary =
      getParam<std::vector<BoundaryName>>("boundary");
  const std::vector<VariableName> &displacements =
      getParam<std::vector<VariableName>>("displacements");
  for (unsigned int i = 0; i < displacements.size(); ++i) {
    std::stringstream name;
    name << prefix << i << boundary[0];

    params.set<unsigned int>("component") = i;
    params.set<NonlinearVariableName>("variable") = displacements[i];

    _problem->addBoundaryCondition("NonReflectingBC", name.str(), params);
  }
}
