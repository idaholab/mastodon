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
#include "SeismicSourceAction.h"
#include "SeismicSource.h"
#include "MooseMesh.h"

#include "FEProblem.h"
#include "Factory.h"
#include "Parser.h"

/**
* This action sets up the seismic source dirackernel in multiple directions.
**/
registerMooseAction("MastodonApp", SeismicSourceAction, "add_dirac_kernel");

InputParameters
SeismicSourceAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("Set up Seismic source dirac kernels in multiple directions.");
  params += SeismicSource::commonParameters();
  params.addRequiredParam<std::vector<NonlinearVariableName>>(
      "displacements", "The vector of displacement variables.");
  return params;
}

SeismicSourceAction::SeismicSourceAction(const InputParameters & params) : Action(params) {}

void
SeismicSourceAction::act()
{
  std::vector<NonlinearVariableName> displacements =
      getParam<std::vector<NonlinearVariableName>>("displacements");

  if (displacements.size() != _mesh->dimension())
    mooseError("Error in " + name() +
               ". The number of displacement variables must match the mesh dimension.");

  // Apply parameters from the action level to the parameters of a
  // SeismicSource object
  InputParameters params = _factory.getValidParams("SeismicSource");
  params.applyParameters(parameters());

  std::string short_name = "mastodon_seismic_source";

  // Loop over displacements and add SeismicSource object for each variable
  for (unsigned int i = 0; i < displacements.size(); ++i)
  {
    std::stringstream name;
    name << short_name;
    name << i;

    params.set<unsigned int>("component") = i;
    params.set<NonlinearVariableName>("variable") = displacements[i];

    _problem->addDiracKernel("SeismicSource", name.str(), params);
  }
}
