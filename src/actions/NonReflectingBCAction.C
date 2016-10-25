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
#include "NonReflectingBCAction.h"

#include "Factory.h"
#include "FEProblem.h"
#include "Parser.h"

/**
 * Applies the non-reflecing boundary condition in different directions.
 **/
template<>
InputParameters validParams<NonReflectingBCAction>()
{
  InputParameters params = validParams<Action>();
  params.addClassDescription("Set up Non-reflecting boundary conditions in different directions.");

  params.addRequiredParam<std::vector<BoundaryName> >("boundary", "The list of boundary IDs from the mesh where this boundary condition will be applied");
  params.addParam<std::vector<NonlinearVariableName> >("displacements", "The string of displacements.");
  params.addParam<std::vector<NonlinearVariableName> >("velocities", "The string of velocities.");
  params.addParam<std::vector<NonlinearVariableName> >("accelerations", "The string of accelerations.");
  params.addRequiredParam<Real>("beta", "The beta parameter for Newmark time integration.");
  params.addRequiredParam<Real>("gamma", "The gamma parameter for Newmark time integration.");
  params.addParam<Real>("alpha", 0.0, "The alpha parameter for HHT time integration.");
  return params;
}

NonReflectingBCAction::NonReflectingBCAction(const InputParameters & params) :
  Action(params)
{
}

void
NonReflectingBCAction::act()
{
  std::vector<NonlinearVariableName> displacements;
  if (isParamValid("displacements"))
    displacements = getParam<std::vector<NonlinearVariableName> > ("displacements");
  else
    mooseError("Error in " + name() + ". The input file should specify a string of displacement names; these name should match the variable block names.");

  std::vector<NonlinearVariableName> velocities;
  if (isParamValid("velocities"))
    velocities = getParam<std::vector<NonlinearVariableName> > ("velocities");
  else
    mooseError("Error in " + name() + ". The input file should specify a string of velocity names.");

  std::vector<NonlinearVariableName> accelerations;
  if (isParamValid("accelerations"))
    accelerations = getParam<std::vector<NonlinearVariableName> > ("accelerations");
  else
    mooseError("Error in " + name() + ". The input file should specify a string of acceleration names.");

  std::vector<VariableName> coupled_displacements;
  std::vector<VariableName> coupled_velocities;
  std::vector<VariableName> coupled_accelerations;

  if ((displacements.size() != velocities.size()) || (displacements.size() != accelerations.size()))
    mooseError("Error in " + name() + ". Displacements, velocities and accelerations should be of the same size.");
  else
  {
    for (unsigned int i = 0; i < displacements.size(); ++i)
    {
      coupled_displacements.push_back(displacements[i]);
      coupled_velocities.push_back(velocities[i]);
      coupled_accelerations.push_back(accelerations[i]);
    }
  }
  std::vector<BoundaryName> boundary = getParam<std::vector<BoundaryName> >("boundary");

  InputParameters params = _factory.getValidParams("NonReflectingBC");
  params.set<std::vector<VariableName> >("displacements") = coupled_displacements;
  params.set<std::vector<VariableName> >("velocities") = coupled_velocities;
  params.set<std::vector<VariableName> >("accelerations") = coupled_accelerations;
  params.set<std::vector<BoundaryName> >("boundary") = boundary;
  params.set<Real>("beta") = getParam<Real>("beta");
  params.set<Real>("gamma") = getParam<Real>("gamma");
  params.set<Real>("alpha") = getParam<Real>("alpha");

  std::string short_name = "mastodon_non_reflecting_BC";

  for (unsigned int i = 0; i < displacements.size(); ++i)
  {
    std::stringstream name;
    name << short_name;
    name << i;
    name << boundary[0];

    params.set<unsigned int>("component") = i;
    params.set<NonlinearVariableName>("variable") = displacements[i];

    _problem->addBoundaryCondition("NonReflectingBC", name.str(), params);
  }
}
