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

#include "MastodonAddKernelsAction.h"

// MOOSE includes
#include "Conversion.h"
#include "Factory.h"
#include "FEProblem.h"
#include "MooseMesh.h"
#include "Action.h"
#include "MooseObjectAction.h"

template <>
InputParameters
validParams<MastodonAddKernelsAction>()
{
  InputParameters params = validParams<Action>();
  params.addClassDescription("Create inertia Kernels, and acceleration, velocity, stress and "
                             "strain AuxKernels automatically, based on the dimension of the "
                             "problem.");
  params.addParam<bool>("inertia_kernels", true, "false, if inertia Kernels are created manually.");
  params.addParam<bool>(
      "auxkernels",
      true,
      "false, if velocity, acceleration, stress and strain AuxKernels are created manually.");
  params.addParam<Real>("beta", 0.25, "beta parameter for Newmark time integration.");
  params.addParam<Real>("gamma", 0.5, "gamma parameter for Newmark time integration.");
  return params;
}

MastodonAddKernelsAction::MastodonAddKernelsAction(const InputParameters & params) : Action(params)
{
}

void
MastodonAddKernelsAction::act()
{
  const bool inertia_kernels = getParam<bool>("inertia_kernels");
  const bool auxkernels = getParam<bool>("auxkernels");
  const std::vector<VariableName> disp_variables = {"disp_x", "disp_y", "disp_z"};
  const std::vector<AuxVariableName> vel_variables = {"vel_x", "vel_y", "vel_z"};
  const std::vector<AuxVariableName> accel_variables = {"accel_x", "accel_y", "accel_z"};
  const std::vector<std::string> comp = {"x", "y", "z"};

  // Adding inertia kernels
  if (inertia_kernels && _current_task == "add_kernel")
  {
    for (std::size_t i = 0; i < _problem->mesh().dimension(); i++)
    {
      InputParameters params = _factory.getValidParams("InertialForce");
      params.set<NonlinearVariableName>("variable") = disp_variables[i];
      params.set<std::vector<VariableName>>("velocity") = {vel_variables[i]};
      params.set<std::vector<VariableName>>("acceleration") = {accel_variables[i]};
      params.set<Real>("beta") = getParam<Real>("beta");
      params.set<Real>("gamma") = getParam<Real>("gamma");
      _problem->addKernel("InertialForce", "inertia" + comp[i], params);
    }
  }

  // Adding AuxKernels
  if (auxkernels && _current_task == "add_kernel")
  {
    // velocity and acceleration AuxKernels
    const std::vector<std::string> vel_auxkernel = {"vel_x", "vel_y", "vel_z"};
    const std::vector<std::string> accel_auxkernel = {"accel_x", "accel_y", "accel_z"};
    const Real beta = getParam<Real>("beta");
    const Real gamma = getParam<Real>("gamma");
    for (std::size_t j = 0; j < _problem->mesh().dimension(); j++)
    {
      // Velocity AuxKernels: These Auxkernels are added as MooseObjectActions
      // due to conflicts with DynamicTensorMechanicsAction when added using
      // addAuxKernel method. TODO: Investigate this conflict. Talk to Ben Spencer.
      InputParameters action_params = _action_factory.getValidParams("AddKernelAction");
      // Create the action
      action_params.set<std::string>("type") = "NewmarkVelAux";
      std::shared_ptr<MooseObjectAction> vel_action = std::static_pointer_cast<MooseObjectAction>(
          _action_factory.create("AddKernelAction", vel_auxkernel[j], action_params));
      vel_action->appendTask("add_aux_kernel");
      // Assigning input parameters
      InputParameters & vel_kernel_params = vel_action->getObjectParams();
      vel_kernel_params.set<AuxVariableName>("variable") = vel_variables[j];
      vel_kernel_params.set<std::vector<VariableName>>("acceleration") = {accel_variables[j]};
      vel_kernel_params.set<Real>("gamma") = gamma;
      vel_kernel_params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;
      // Add the action to the warehouse
      _awh.addActionBlock(vel_action);

      // Acceleration AuxKernels: Added using the addAuxKernel method
      InputParameters accel_kernel_params = _factory.getValidParams("NewmarkAccelAux");
      accel_kernel_params.set<AuxVariableName>("variable") = accel_variables[j];
      accel_kernel_params.set<std::vector<VariableName>>("displacement") = {disp_variables[j]};
      accel_kernel_params.set<std::vector<VariableName>>("velocity") = {vel_variables[j]};
      accel_kernel_params.set<Real>("beta") = beta;
      accel_kernel_params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;
      _problem->addAuxKernel("NewmarkAccelAux", accel_auxkernel[j], accel_kernel_params);
    }

    // stress and strain AuxKernels: Added using the addAuxKernel method.
    const std::vector<std::string> stress_names = {
        "stress_xx", "stress_yy", "stress_xy", "stress_zz", "stress_zx", "stress_yz"};
    const std::vector<std::string> strain_names = {
        "strain_xx", "strain_yy", "strain_xy", "strain_zz", "strain_zx", "strain_yz"};
    const std::vector<unsigned int> index_i = {0, 1, 1, 2, 2, 1};
    const std::vector<unsigned int> index_j = {0, 1, 0, 2, 0, 2};
    const std::vector<std::size_t> matrix_size = {1, 3, 6}; // =1 for 1D, =3 for 2D, and =6 for 3D
    for (std::size_t k = 0; k < matrix_size[_problem->mesh().dimension() - 1]; k++)
    {
      InputParameters stress_kernel_params = _factory.getValidParams("RankTwoAux");
      stress_kernel_params.set<MaterialPropertyName>("rank_two_tensor") = "stress";
      stress_kernel_params.set<AuxVariableName>("variable") = stress_names[k];
      stress_kernel_params.set<unsigned int>("index_i") = index_i[k];
      stress_kernel_params.set<unsigned int>("index_j") = index_j[k];
      _problem->addAuxKernel("RankTwoAux", stress_names[k], stress_kernel_params);

      InputParameters strain_kernel_params = _factory.getValidParams("RankTwoAux");
      strain_kernel_params.set<MaterialPropertyName>("rank_two_tensor") = "total_strain";
      strain_kernel_params.set<AuxVariableName>("variable") = strain_names[k];
      strain_kernel_params.set<unsigned int>("index_i") = index_i[k];
      strain_kernel_params.set<unsigned int>("index_j") = index_j[k];
      _problem->addAuxKernel("RankTwoAux", strain_names[k], strain_kernel_params);
    }
  }
}
