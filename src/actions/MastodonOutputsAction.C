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

#include "MastodonOutputsAction.h"

// MOOSE includes
#include "Conversion.h"
#include "Factory.h"
#include "FEProblem.h"
#include "MooseMesh.h"
#include "Action.h"
#include "MooseObjectAction.h"

registerMooseAction("MastodonApp", MastodonOutputsAction, "add_aux_variable");

registerMooseAction("MastodonApp", MastodonOutputsAction, "add_aux_kernel");

template <>
InputParameters
validParams<MastodonOutputsAction>()
{
  InputParameters params = validParams<Action>();
  params.addClassDescription("Create required objects for requested output.");
  params.addParam<bool>(
      "stress_strain_output", false, "true, if stress and strain output is required.");
  return params;
}

MastodonOutputsAction::MastodonOutputsAction(const InputParameters & params)
  : Action(params),
    _stress_auxvariables(
        {"stress_xx", "stress_yy", "stress_xy", "stress_zz", "stress_zx", "stress_yz"}),
    _strain_auxvariables(
        {"strain_xx", "strain_yy", "strain_xy", "strain_zz", "strain_zx", "strain_yz"}),
    _matrix_size({1, 3, 6}) // =1 for 1D, =3 for 2D, and =6 for 3D problems
{
}

void
MastodonOutputsAction::act()
{
  // Creating stress and strain AuxVariables and AuxKernels when stress and strain output is
  // requested
  const bool stress_strain_output = getParam<bool>("stress_strain_output");
  if (stress_strain_output)
  {
    if (_current_task == "add_aux_variable")
      addStressStrainAuxVariables();

    if (_current_task == "add_aux_kernel")
      addStressStrainAuxKernels();
  }
}

void
MastodonOutputsAction::addStressStrainAuxVariables()
{
  FEType stress_strain_fe_type(CONSTANT, MONOMIAL);
  for (std::size_t k = 0; k < _matrix_size[_problem->mesh().dimension() - 1]; k++)
  {
    _problem->addAuxVariable(_stress_auxvariables[k], stress_strain_fe_type);
    _problem->addAuxVariable(_strain_auxvariables[k], stress_strain_fe_type);
  }
}

void
MastodonOutputsAction::addStressStrainAuxKernels()
{
  const std::vector<unsigned int> index_i = {0, 1, 1, 2, 2, 1};
  const std::vector<unsigned int> index_j = {0, 1, 0, 2, 0, 2};
  for (std::size_t k = 0; k < _matrix_size[_problem->mesh().dimension() - 1]; k++)
  {
    InputParameters stress_kernel_params = _factory.getValidParams("RankTwoAux");
    stress_kernel_params.set<MaterialPropertyName>("rank_two_tensor") = "stress";
    stress_kernel_params.set<AuxVariableName>("variable") = _stress_auxvariables[k];
    stress_kernel_params.set<unsigned int>("index_i") = index_i[k];
    stress_kernel_params.set<unsigned int>("index_j") = index_j[k];
    stress_kernel_params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;
    _problem->addAuxKernel("RankTwoAux", _stress_auxvariables[k], stress_kernel_params);

    InputParameters strain_kernel_params = _factory.getValidParams("RankTwoAux");
    strain_kernel_params.set<MaterialPropertyName>("rank_two_tensor") = "total_strain";
    strain_kernel_params.set<AuxVariableName>("variable") = _strain_auxvariables[k];
    strain_kernel_params.set<unsigned int>("index_i") = index_i[k];
    strain_kernel_params.set<unsigned int>("index_j") = index_j[k];
    strain_kernel_params.set<ExecFlagEnum>("execute_on") = EXEC_TIMESTEP_END;
    _problem->addAuxKernel("RankTwoAux", _strain_auxvariables[k], strain_kernel_params);
  }
}
