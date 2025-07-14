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

#include "MastodonModelAction.h"

// MOOSE includes
#include "Conversion.h"
#include "Factory.h"
#include "FEProblem.h"
#include "MooseMesh.h"
#include "Action.h"
#include "MooseObjectAction.h"

registerMooseAction("MastodonApp", MastodonModelAction, "meta_action");

registerMooseAction("MastodonApp", MastodonModelAction, "add_variable");

registerMooseAction("MastodonApp", MastodonModelAction, "add_aux_variable");

registerMooseAction("MastodonApp", MastodonModelAction, "add_kernel");

registerMooseAction("MastodonApp", MastodonModelAction, "add_aux_kernel");

InputParameters
MastodonModelAction::validParams()
{
  InputParameters params = Action::validParams();
  params.addClassDescription("Creates the required displacement Variables, velocity and "
                             "acceleration AuxVariables and AuxKernels, inertia kernels and the "
                             "DynamicTensorMechanicsAction based on the dimension of the mesh and "
                             "if static or dynamic analysis is being performed.");
  params.addParam<std::vector<SubdomainName>>("block",
                                              "A list of ids of the blocks (subdomains) "
                                              "that these model parameters apply to.");
  params.addParam<bool>("dynamic_analysis", true, "false, if static analysis is to be performed.");
  params.addParam<Real>("beta", 0.25, "beta parameter for Newmark time integration.");
  params.addParam<Real>("gamma", 0.5, "gamma parameter for Newmark time integration.");
  params.addParam<std::vector<NonlinearVariableName>>(
      "displacements",
      std::vector<NonlinearVariableName>({"disp_x", "disp_y", "disp_z"}),
      "Names of the displacement variables.");
  params.addParam<std::vector<AuxVariableName>>(
      "velocities",
      std::vector<AuxVariableName>({"vel_x", "vel_y", "vel_z"}),
      "Names of the velocity variables.");
  params.addParam<std::vector<AuxVariableName>>(
      "accelerations",
      std::vector<AuxVariableName>({"accel_x", "accel_y", "accel_z"}),
      "Names of the acceleration variables.");
  MooseEnum dim("1=1 2=2 3=3", "3");
  params.addParam<MooseEnum>("dim", dim, "Dimension of the mesh.");
  params.addParam<MaterialPropertyName>(
      "eta", 0.0, "eta parameter or mass matrix multiplier for Rayleigh damping.");
  params.addParam<MaterialPropertyName>(
      "zeta", 0.0, "zeta parameter or stiffness matrix multiplier for Rayleigh damping.");
  params.addParam<bool>(
      "use_displaced_mesh", false, "true, if calculations are performed on displaced mesh.");
  return params;
}

MastodonModelAction::MastodonModelAction(const InputParameters & params)
  : Action(params),
    _disp_variables(getParam<std::vector<NonlinearVariableName>>("displacements")),
    _vel_auxvariables(getParam<std::vector<AuxVariableName>>("velocities")),
    _accel_auxvariables(getParam<std::vector<AuxVariableName>>("accelerations")),
    _dim(getParam<MooseEnum>("dim")),
    _use_displaced_mesh(getParam<bool>("use_displaced_mesh"))
{
}

void
MastodonModelAction::act()
{
  // Adding displacement Variables
  if (_current_task == "add_variable")
    addDisplacementVariables();

  // Adding DynamicTensorMechanicsAction
  if (_current_task == "meta_action")
    addDynamicTensorMechanicsAction();

  // Adding AuxVariables and AuxKernels required for dynamic analysis. These
  // include velocity and acceleration AuxVariables and AuxKernels.
  const bool dynamic_analysis = getParam<bool>("dynamic_analysis");
  if (dynamic_analysis)
  {
    // Adding velocity and acceleration AuxVariables
    if (_current_task == "add_aux_variable")
      addVelAccelAuxVariables();

    // Adding inertia kernels
    if (_current_task == "add_kernel")
      addInertiaKernels();

    // Adding velocity and acceleration Auxkernels
    if (_current_task == "add_aux_kernel")
      addVelAccelAuxKernels();
  }
}

void
MastodonModelAction::addDynamicTensorMechanicsAction()
{
  std::vector<VariableName> dim_disp_variables(_disp_variables.begin(),
                                               _disp_variables.begin() + _dim);
  // Retrieve action parameters and set the parameters
  InputParameters action_params =
      _action_factory.getValidParams("LegacyDynamicTensorMechanicsAction");
  action_params.set<std::vector<VariableName>>("displacements") = dim_disp_variables;
  action_params.applyParameters(parameters(), {"eta", "zeta"});
  action_params.set<MaterialPropertyName>("mass_damping_coefficient") =
      getParam<MaterialPropertyName>("eta");
  action_params.set<MaterialPropertyName>("stiffness_damping_coefficient") =
      getParam<MaterialPropertyName>("zeta");
  // Create the action and add it to the action warehouse
  std::shared_ptr<Action> dynamictensormechanics_action =
      std::static_pointer_cast<Action>(_action_factory.create(
          "LegacyDynamicTensorMechanicsAction", "DynamicTensorMechanics", action_params));
  _awh.addActionBlock(dynamictensormechanics_action);
}

void
MastodonModelAction::addDisplacementVariables()
{
  if (_dim != _problem->mesh().dimension())
    mooseError("Error in MastodonModelAction block, ",
               name(),
               ". dim is not equal to the mesh dimension, which is ",
               _problem->mesh().dimension(),
               ".");

  auto params = _factory.getValidParams("MooseVariable");
  // determine necessary order
  const bool second = _problem->mesh().hasSecondOrderElements();

  params.set<MooseEnum>("order") = second ? "SECOND" : "FIRST";
  params.set<MooseEnum>("family") = "LAGRANGE";
  if (isParamValid("scaling"))
    params.set<std::vector<Real>>("scaling") = {getParam<Real>("scaling")};

  // Loop through the displacement variables
  for (std::size_t j = 0; j < _problem->mesh().dimension(); j++)
  {
    // Create displacement variables
    _problem->addVariable("MooseVariable", _disp_variables[j], params);
  }
}

void
MastodonModelAction::addVelAccelAuxVariables()
{
  auto params = _factory.getValidParams("MooseVariable");
  const bool second = _problem->mesh().hasSecondOrderElements();
  params.set<MooseEnum>("order") = second ? "SECOND" : "FIRST";
  params.set<MooseEnum>("family") = "LAGRANGE";
  for (std::size_t j = 0; j < _problem->mesh().dimension(); j++)
  {
    _problem->addAuxVariable("MooseVariable", _vel_auxvariables[j], params);
    _problem->addAuxVariable("MooseVariable", _accel_auxvariables[j], params);
  }
}

void
MastodonModelAction::addInertiaKernels()
{
  const std::vector<std::string> comp = {"x", "y", "z"};
  for (std::size_t i = 0; i < _problem->mesh().dimension(); i++)
  {
    InputParameters params = _factory.getValidParams("InertialForce");
    params.set<NonlinearVariableName>("variable") = _disp_variables[i];
    params.set<std::vector<VariableName>>("velocity") = {_vel_auxvariables[i]};
    params.set<std::vector<VariableName>>("acceleration") = {_accel_auxvariables[i]};
    params.set<Real>("beta") = getParam<Real>("beta");
    params.set<Real>("gamma") = getParam<Real>("gamma");
    params.set<MaterialPropertyName>("eta") = getParam<MaterialPropertyName>("eta");
    params.set<bool>("use_displaced_mesh") = _use_displaced_mesh;
    _problem->addKernel("InertialForce", "inertia" + comp[i], params);
  }
}

void
MastodonModelAction::addVelAccelAuxKernels()
{
  const std::vector<std::string> vel_auxkernel = {"vel_x", "vel_y", "vel_z"};
  const std::vector<std::string> accel_auxkernel = {"accel_x", "accel_y", "accel_z"};
  const Real beta = getParam<Real>("beta");
  const Real gamma = getParam<Real>("gamma");
  for (std::size_t j = 0; j < _problem->mesh().dimension(); j++)
  {
    // Velocity AuxKernels: These Auxkernels are added as MooseObjectActions
    // due to conflicts with DynamicTensorMechanicsAction when added using
    // addAuxKernel method. TODO: See if this needs to be fixed later.
    InputParameters action_params = _action_factory.getValidParams("AddKernelAction");
    // Create the action
    action_params.set<std::string>("type") = "NewmarkVelAux";
    action_params.set<std::string>("task") = "add_aux_kernel";
    std::shared_ptr<MooseObjectAction> vel_action = std::static_pointer_cast<MooseObjectAction>(
        _action_factory.create("AddKernelAction", vel_auxkernel[j], action_params));

    // Assigning input parameters
    InputParameters & vel_kernel_params = vel_action->getObjectParams();
    vel_kernel_params.set<AuxVariableName>("variable") = _vel_auxvariables[j];
    vel_kernel_params.set<std::vector<VariableName>>("acceleration") = {_accel_auxvariables[j]};
    vel_kernel_params.set<Real>("gamma") = gamma;
    vel_kernel_params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;
    // Add the action to the warehouse
    _awh.addActionBlock(vel_action);

    // Acceleration AuxKernels: Added using the addAuxKernel method
    InputParameters accel_kernel_params = _factory.getValidParams("NewmarkAccelAux");
    accel_kernel_params.set<AuxVariableName>("variable") = _accel_auxvariables[j];
    accel_kernel_params.set<std::vector<VariableName>>("displacement") = {_disp_variables[j]};
    accel_kernel_params.set<std::vector<VariableName>>("velocity") = {_vel_auxvariables[j]};
    accel_kernel_params.set<Real>("beta") = beta;
    accel_kernel_params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;
    _problem->addAuxKernel("NewmarkAccelAux", accel_auxkernel[j], accel_kernel_params);
  }
}
