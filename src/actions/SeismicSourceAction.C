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

#include "FEProblem.h"
#include "Factory.h"
#include "Parser.h"

/**
* This action sets up the seismic source dirackernel in multiple directions.
**/

registerMooseAction("MastodonApp", SeismicSourceAction, "add_dirac_kernel");

template <>
InputParameters
validParams<SeismicSourceAction>()
{
  InputParameters params = validParams<Action>();
  params.addClassDescription("Set up Seismic source dirac kernels in multiple directions.");
  params.addRequiredParam<std::vector<NonlinearVariableName>>(
      "displacements", "The vector of displacement variables.");
  params.addRequiredParam<Real>("strike", "The strike angle of the fault in degrees.");
  params.addRequiredParam<Real>("dip", "The dip angle of the fault in degrees.");
  params.addRequiredParam<Real>("rake", "The rake angle of the fault in degrees.");
  params.addRequiredParam<Real>("area", "The area over which slip is distributed.");
  params.addRequiredParam<FunctionName>("slip",
                                        "The function describing slip as a function of time.");
  params.addRequiredParam<Real>("shear_modulus",
                                "The shear modulus of the soil around the seismic source.");
  params.addParam<std::vector<FunctionName>>("position_function",
                                             "The vector of function names that describes the x, "
                                             "y and z coordinates of the source.");
  params.addParam<unsigned int>("number",
                                "The number of point sources. This number should be same as "
                                "number of entries in the function files describing the "
                                "position of the point source.");
  params.addParam<std::vector<Real>>("point",
                                     "The x, y and z coordinate of a single point source.");
  params.addParam<Real>("alpha", 0.0, "The Hilber Hughes Taylor time integration parameter.");
  return params;
}

SeismicSourceAction::SeismicSourceAction(const InputParameters & params) : Action(params) {}

void
SeismicSourceAction::act()
{
  std::vector<NonlinearVariableName> displacements;
  if (isParamValid("displacements"))
    displacements = getParam<std::vector<NonlinearVariableName>>("displacements");
  else
    mooseError("Error in " + name() + ". A string of displacements is required as input.");

  InputParameters params = _factory.getValidParams("SeismicSource");
  params.set<Real>("strike") = getParam<Real>("strike");
  params.set<Real>("dip") = getParam<Real>("dip");
  params.set<Real>("rake") = getParam<Real>("rake");
  params.set<Real>("area") = getParam<Real>("area");
  params.set<FunctionName>("slip") = getParam<FunctionName>("slip");
  params.set<Real>("shear_modulus") = getParam<Real>("shear_modulus");
  params.set<Real>("alpha") = getParam<Real>("alpha");

  if (isParamValid("position_function") && isParamValid("number"))
  {
    params.set<unsigned int>("number") = getParam<unsigned int>("number");
    std::vector<FunctionName> position_function =
        getParam<std::vector<FunctionName>>("position_function");
    params.set<FunctionName>("x_position") = position_function[0];
    if (position_function.size() > 1)
    {
      params.set<FunctionName>("y_position") = position_function[1];
      if (position_function.size() > 2)
        params.set<FunctionName>("z_position") = position_function[2];
    }
  }

  if (isParamValid("point"))
    params.set<std::vector<Real>>("point") = getParam<std::vector<Real>>("point");
  std::string short_name = "mastodon_seismic_source";

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
