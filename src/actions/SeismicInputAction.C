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
#include "SeismicInputAction.h"

#include "Factory.h"
#include "FEProblem.h"
#include "Parser.h"

/**
 * This action is used to apply seismic input on a set of boundaries in a model.
 * The seismic input can be applied in two ways:
 *
 * If the ground motion is an outcrop motion, then it is applied using SeismicForce boundary condition.
 * This requires the ground motion velocity as input either from a function or from a aux_velocity variable.
 *
 * If the ground motion is a within-soil motion, then it is applied using PresetDisplacement.
 * This requires the ground displacement as input.
 **/

template<>
InputParameters validParams<SeismicInputAction>()
{
  InputParameters params = validParams<Action>();
  params.addClassDescription("This action sets up Seismic force/displacement boundary conditions.");

  params.addRequiredParam<std::vector<BoundaryName> >("boundary", "The list of boundary IDs from the mesh where this boundary condition will be applied.");
  params.addParam<std::vector<NonlinearVariableName> >("displacements", "The nonlinear displacement variables for the problem.");
  params.addRequiredParam<std::vector<unsigned int> >("input_component", "The directions in which the input ground motions are applied.");
  params.addParam<std::vector<FunctionName> >("input_function", "The vector of function names that describes the input ground motions. Must be same size as input_component.");
  params.addParam<std::vector<NonlinearVariableName> >("aux_velocities", "The vector of velocity aux variables names. The input velocities can be obtained from this variable. The size of this vector must be same size as input_component.");
  params.addParam<Real>("alpha", 0.0, "The alpha parameter for HHT time integration.");
  params.addParam<bool>("outcrop", false, "Set to true if ground motion is to applied at rock outcrop (velocities are converted into forces). By default the ground motion is applied within the soil as a Dirichlet displacement BC.");
  params.addParam<std::vector<NonlinearVariableName> >("velocity", "The vector of velocity variables that are coupled to displacements. The size of this vector should be same as displacements.");
  params.addParam<std::vector<NonlinearVariableName> >("acceleration", "The vector of acceleration variables that are coupled to displacements. The size of this vector should be same as displacements.");
  params.addParam<Real>("beta", 0.0, "The beta parameter for newmark time integration.");

  return params;
}

SeismicInputAction::SeismicInputAction(const InputParameters & params) :
  Action(params)
{
}

void
SeismicInputAction::act()
{
  std::vector<NonlinearVariableName> displacements;
  if (isParamValid("displacements"))
    displacements = getParam<std::vector<NonlinearVariableName> >("displacements");
  else
    mooseError("Error in " + name() + ". A string of displacements is required as input.");

  std::vector<FunctionName> input_function;
  std::vector<NonlinearVariableName> velocities;
  std::vector<unsigned int> input_component = getParam<std::vector<unsigned int> >("input_component");

  if (isParamValid("input_function"))
  {
    input_function = getParam<std::vector<FunctionName> >("input_function");
    if (input_component.size() != input_function.size())
      mooseError("Error in " + name() + ". input_component and input_function must be of the same size.");
  }
  else if (isParamValid("velocities"))
  {
    velocities = getParam<std::vector<NonlinearVariableName> >("velocities");
    if (velocities.size() != input_component.size())
      mooseError("Error in " + name() + ". input_component and velocities must be of the same size.");
  }
  else
    mooseError("Error in " + name() + ". Please provide a function or aux variable describing the input velocity");

  std::vector<BoundaryName> boundary = getParam<std::vector<BoundaryName> >("boundary");
  bool outcrop = getParam<bool>("outcrop");
  if (outcrop)
  {
    // convert input velocities into forces using seismic force
    InputParameters params = _factory.getValidParams("SeismicForce");
    params.set<std::vector<BoundaryName> >("boundary") = boundary;
    params.set<Real>("alpha") = getParam<Real>("alpha");

    std::string short_name = "mastodon_seismic_force";

    for (unsigned int j = 0; j < input_component.size(); ++j)
    {
      unsigned int input_component_j = input_component[j];
      params.set<unsigned int>("vel_component") = input_component_j;

      if (isParamValid("input_function"))
        params.set<FunctionName>("velocity_function") = input_function[j];
      else
      {
        std::vector<VariableName> coupled_velocity;
        coupled_velocity.push_back(velocities[j]);
        params.set<std::vector<VariableName> >("velocity") = coupled_velocity;
      }
      for (unsigned int i = 0; i < displacements.size(); ++i)
      {
        std::stringstream name;
        name << short_name;
        name << i;
        name << input_component_j;
        name << boundary[0];
        params.set<unsigned int>("component") = i;
        params.set<NonlinearVariableName>("variable") = displacements[i];

        _problem->addBoundaryCondition("SeismicForce", name.str(), params);
      }
    }
  }
  else
  {
    // apply input displacement functions as preset displacement
    std::vector<NonlinearVariableName> velocity;
    std::vector<NonlinearVariableName> acceleration;
    if (isParamValid("acceleration") && isParamValid("velocity"))
    {
       velocity = getParam<std::vector<NonlinearVariableName> >("velocity");
       acceleration = getParam<std::vector<NonlinearVariableName> >("acceleration");
    }

    if (input_component.size() != displacements.size() || displacements.size() != acceleration.size() || displacements.size() != velocity.size())
      mooseError("Error in " + name() + ". displacements, velocity, acceleration and input components should be of the same size.");

    InputParameters params = _factory.getValidParams("PresetDisplacement");
    params.set<std::vector<BoundaryName> >("boundary") = boundary;
    params.set<Real>("beta") = getParam<Real>("beta");

    std::string short_name = "mastodon_preset_displacement";

    for (unsigned int j = 0; j < input_component.size(); ++j)
    {
      params.set<FunctionName>("function") = input_function[j];
      VariableName vel_variable(velocity[j]);
      VariableName accel_variable(acceleration[j]);
      params.addCoupledVar("velocity", "");
      params.addCoupledVar("acceleration", "");
      params.set<NonlinearVariableName>("variable") = displacements[j];
      params.set<std::vector<VariableName> >("velocity") = {vel_variable};
      params.set<std::vector<VariableName> >("acceleration") = {accel_variable};

      std::stringstream name;
      name << boundary[0];
      name << j;

      _problem->addBoundaryCondition("PresetDisplacement", name.str(), params);
    }
  }
}
