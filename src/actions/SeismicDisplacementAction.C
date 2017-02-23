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

// MOOSE includes
#include "Factory.h"
#include "FEProblem.h"
#include "MooseMesh.h"

// Mastodon includes
#include "SeismicDisplacementAction.h"

template<>
InputParameters validParams<SeismicDisplacementAction>()
{
  InputParameters params = validParams<Action>();
  params.addClassDescription("This action sets up seismic displacement boundary conditions.");

  params.addRequiredParam<std::vector<BoundaryName>>("boundary", "The list of boundary IDs from the mesh where this boundary condition will be applied.");
  params.addRequiredParam<std::vector<VariableName>>("displacements", "The nonlinear displacement variables for the problem.");
  params.addRequiredParam<std::vector<VariableName>>("accelerations", "The vector of acceleration variables that are coupled to displacements. The size of this vector should be same as displacements.");
  params.addRequiredParam<std::vector<VariableName>>("velocities", "The vector of velocity variables names. The input velocities can be obtained from this variable. The size of this vector must be same size as input_component.");
  params.addRequiredParam<std::vector<unsigned int>>("input_components", "The directions in which the input ground motions are applied.");
  params.addRequiredParam<std::vector<FunctionName>>("input_functions", "The vector of function names that describes the input ground motions. Must be same size as input_component.");
  params.addParam<Real>("beta", 0.0, "The beta parameter for newmark time integration.");

  return params;
}

SeismicDisplacementAction::SeismicDisplacementAction(const InputParameters & params) :
    Action(params),
    _boundary(getParam<std::vector<BoundaryName>>("boundary")),
    _displacements(getParam<std::vector<VariableName>>("displacements")),
    _accelerations(getParam<std::vector<VariableName>>("accelerations")),
    _velocities(getParam<std::vector<VariableName>>("velocities")),
    _input_components(getParam<std::vector<unsigned int>>("input_components")),
    _input_functions(getParam<std::vector<FunctionName>>("input_functions"))
{
  if ((_displacements.size() != _input_components.size()) || (_displacements.size() != _accelerations.size()) ||
      (_displacements.size() != _velocities.size()) || _displacements.size() != _input_functions.size())
    mooseError("In the \"", name(), "\" block the 'displacements', 'velocities', 'accelerations', 'input_functions', and 'input_components' should be of the same size.");
}

void
SeismicDisplacementAction::act()
{
  InputParameters params = _factory.getValidParams("PresetDisplacement");
  params.set<std::vector<BoundaryName>>("boundary") = _boundary;
  params.set<Real>("beta") = getParam<Real>("beta");

  std::string prefix = "mastodon_preset_displacement";
  for (unsigned int j = 0; j < _input_components.size(); ++j)
  {
    params.set<FunctionName>("function") = _input_functions[j];
    params.set<NonlinearVariableName>("variable") = _displacements[j];
    params.set<std::vector<VariableName>>("velocity") = {_velocities[j]};
    params.set<std::vector<VariableName>>("acceleration") = {_accelerations[j]};

    std::stringstream obj_name;
    obj_name << prefix << "_" << name() << "_" << j;
    _problem->addBoundaryCondition("PresetDisplacement", obj_name.str(), params);
  }
}
