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

#include "MastodonAddVariableAction.h"

// MOOSE includes
#include "Conversion.h"
#include "Factory.h"
#include "FEProblem.h"
#include "MooseMesh.h"

template <>
InputParameters
validParams<MastodonAddVariableAction>()
{
  InputParameters params = validParams<Action>();
  params.addClassDescription("Create displacement Variables, and velocity, acceleration, stress "
                             "and strain AuxVariables automatically.");

  params.addParam<bool>(
      "add_variables", true, "false, if displacement Variables are created manually.");
  params.addParam<bool>(
      "add_auxvariables",
      true,
      "false, if velocity, acceleration, stress and strain AuxVariables are created manually.");
  return params;
}

MastodonAddVariableAction::MastodonAddVariableAction(const InputParameters & params)
  : Action(params)
{
}

void
MastodonAddVariableAction::act()
{
  bool add_variables = getParam<bool>("add_variables");
  bool add_auxvariables = getParam<bool>("add_auxvariables");
  const bool second =
      _problem->mesh().hasSecondOrderElements(); // if mesh has second order elements
  // Adding displacement variable
  if (add_variables)
  {
    FEType disp_fe_type(second ? SECOND : FIRST, LAGRANGE);
    std::vector<VariableName> disp_variable = {"disp_x", "disp_y", "disp_z"};
    for (std::size_t i = 0; i < _problem->mesh().dimension(); i++)
      _problem->addVariable(disp_variable[i], disp_fe_type, 1.0);
  }
  // Adding AuxVariables
  if (add_auxvariables)
  {
    FEType vel_acc_fe_type(second ? SECOND : FIRST);
    std::vector<AuxVariableName> vel_auxvariable = {"vel_x", "vel_y", "vel_z"};
    std::vector<AuxVariableName> accel_auxvariable = {"accel_x", "accel_y", "accel_z"};
    // velocity and acceleration
    for (std::size_t j = 0; j < _problem->mesh().dimension(); j++)
    {
      _problem->addAuxVariable(vel_auxvariable[j], vel_acc_fe_type);
      _problem->addAuxVariable(accel_auxvariable[j], vel_acc_fe_type);
    }
    // stress and strain
    FEType stress_strain_fe_type(CONSTANT, MONOMIAL);
    std::vector<AuxVariableName> stress_auxvariable = {
        "stress_xx", "stress_yy", "stress_xy", "stress_zz", "stress_zx", "stress_yz"};
    std::vector<AuxVariableName> strain_auxvariable = {
        "strain_xx", "strain_yy", "strain_xy", "strain_zz", "strain_zx", "strain_yz"};
    std::vector<std::size_t> matrix_size = {1, 3, 6}; // =1 for 1D, =3 for 2D, and =6 for 3D
    for (std::size_t k = 0; k < matrix_size[_problem->mesh().dimension() - 1]; k++)
    {
      _problem->addAuxVariable(stress_auxvariable[k], stress_strain_fe_type);
      _problem->addAuxVariable(strain_auxvariable[k], stress_strain_fe_type);
    }
  }
}
