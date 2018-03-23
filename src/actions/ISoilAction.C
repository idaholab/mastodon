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

// This code was implemented in colloboration with Ozgun Numanoglu
// (numanog2@illinois.edu) and Omar Baltaji (baltaji2@illinois.edu) from UIUC.

#include "ISoilAction.h"

#include "ColumnMajorMatrix.h"
#include "Conversion.h"
#include "FEProblem.h"
#include "Factory.h"
#include "Parser.h"

registerMooseAction("MastodonApp", ISoilAction, "add_material");

template <>
InputParameters
validParams<ISoilAction>()
{
  InputParameters params = validParams<Action>();
  params.addClassDescription("Set up I-Soil material model.");
  params.addRequiredParam<std::vector<VariableName>>(
      "displacements", "The vector of displacement variables in the problem.");
  params.addRequiredParam<std::vector<SubdomainName>>(
      "block", "The blocks where this material model is applied.");
  params.addRequiredParam<std::vector<VariableName>>(
      "layer_variable", "The auxvariable providing the soil layer identification.");
  params.addRequiredParam<std::vector<unsigned int>>(
      "layer_ids",
      "Vector of layer ids that map one-to-one to the rest of the "
      "soil layer parameters provided as input.");
  params.addRequiredParam<std::vector<Real>>("poissons_ratio",
                                             "Poissons's ratio for the soil layers. The "
                                             "size of the vector should be same as the size of "
                                             "layer_ids.");
  params.addParam<Real>("b_exp",
                        0.0,
                        "The exponential factors for pressure "
                        "dependent stiffness for all the soil "
                        "layers.");
  params.addParam<std::vector<Real>>("p_ref",
                                     "The reference pressure at which "
                                     "the parameters are defined for "
                                     "each soil layer.");
  params.addParam<Real>("a0",
                        1.0,
                        "The first coefficient for pressure dependent yield strength "
                        "calculation for all the soil layers. If a0 = 1, a1 = 0 and "
                        "a2=0 for one soil layer, then the yield strength of that "
                        "layer is independent of pressure.");
  params.addParam<Real>("a1",
                        0.0,
                        "The second coefficient for pressure dependent yield "
                        "strength calculation for all the soil layers. If a0 = "
                        "1, a1 = 0, a2 = 0 for one soil layer, then the yield "
                        "strength of that layer is independent of pressure.");
  params.addParam<Real>("a2",
                        0.0,
                        "The third coefficient for pressure dependent yield "
                        "strength calculation for all the soil layers. If a0 = "
                        "1, a1=0 and a2=0 for one soil layer, then the yield "
                        "strength of that layer is independent of pressure.");
  params.addParam<Real>("tension_pressure_cut_off",
                        -1.0,
                        "The tension cut-off for all the soil layers. If the "
                        "pressure becomes lower than this value, then the "
                        "stiffness of the soil reduces to zero. A negative "
                        "pressure indicates tension. The default "
                        "value is -1.0 for all the soil layers.");
  params.addParam<bool>("pressure_dependency",
                        false,
                        "Set to true to turn on pressure dependent stiffness "
                        "and yield strength calculation.");
  params.addParam<std::vector<FunctionName>>(
      "initial_soil_stress",
      "The function values for the initial stress distribution. 9 function "
      "names have to be provided corresponding to stress_xx, stress_xy, "
      "stress_xz, stress_yx, stress_yy, stress_yz, stress_zx, stress_zy, "
      "stress_zz. Each function can be a function of space.");
  // params for specific backbone types
  MooseEnum soil_type("user_defined darendeli gqh thin_layer");
  params.addRequiredParam<MooseEnum>(
      "soil_type",
      soil_type,
      "This parameter determines the type of backbone curve used. Use 'user_defined' "
      "for a user defined backbone curve provided in a data file, 'darendeli' "
      "if the backbone curve is to be determined using Darandeli equations, 'gqh' "
      "if the backbone curve is determined using the GQ/H approach and 'thin_layer' if the soil is "
      "being used to simulate a thin-layer friction interface.");
  // params required for user_defined backbone curve: soil_type = 'user_defined'
  params.addParam<std::vector<FileName>>(
      "backbone_curve_files",
      "The vector of file names of the files containing "
      "stress-strain backbone curves for the different soil layers. The "
      "size of the vector should be same as the size of layer_ids. All files "
      "should contain the same number of stress-strain points. Headers are not "
      "expected and it is assumed that the first column corresponds to strain values "
      "and the second column corresponds to the stress values. Additionally, two "
      "segments of a backbone curve cannot have the same slope.");
  // params required for soil_type = 'darendeli' and 'GQ/H'
  params.addParam<std::vector<Real>>(
      "initial_shear_modulus",
      "The initial shear modulus of the soil layers. "
      "This is required if Darandeli or GQ/H type backbone curves are used.");
  params.addParam<unsigned int>("number_of_points",
                                "The total number of data points in which the "
                                "backbone curve needs to be split for all soil "
                                "layers (required for Darandeli or GQ/H type backbone curves).");
  // params required for Darandeli backbone curve: soil_type = 'darendeli'
  params.addParam<std::vector<Real>>("over_consolidation_ratio",
                                     "The over consolidation ratio of the soil "
                                     "layers. Required for Darandeli backbone curve.");
  params.addParam<std::vector<Real>>(
      "plasticity_index",
      "The plasticity index of the soil layers. Required for Darandeli backbone curve.");
  // params required for GQ/H backbone curve: soil_type = "gqh"
  params.addParam<std::vector<Real>>("theta_1",
                                     "The curve fit coefficient for "
                                     "GQ/H model"
                                     "for each soil layer.");
  params.addParam<std::vector<Real>>("theta_2",
                                     "The curve fit coefficient for "
                                     "GQ/H model"
                                     "for each soil layer.");
  params.addParam<std::vector<Real>>("theta_3",
                                     "The curve fit coefficient for "
                                     "GQ/H model"
                                     "for each soil layer.");
  params.addParam<std::vector<Real>>("theta_4",
                                     "The curve fit coefficient for "
                                     "GQ/H model"
                                     "for each soil layer.");
  params.addParam<std::vector<Real>>("theta_5",
                                     "The curve fit coefficient for "
                                     "GQ/H model"
                                     "for each soil layer.");
  params.addParam<std::vector<Real>>("taumax",
                                     "The ultimate shear strength of "
                                     "the soil layers. Required for "
                                     "GQ/H model");
  // params required for thin_layer contact model soil_type = "thin_layer"
  params.addParam<std::vector<Real>>("friction_coefficient",
                                     "Friction coefficients of the thin layers.");
  params.addParam<std::vector<Real>>("hardening_ratio",
                                     "Post-yield hardening ratios of the layers. "
                                     "Defaults to 0.01, which corresponds to 1 percent hardening.");

  // flag to use finite strain calculator
  params.addParam<bool>("finite_strain",
                        false,
                        "Set to true to use finite "
                        "strain calculator instead of "
                        "incremental small strain.");

  params.addRequiredParam<std::vector<Real>>(
      "density",
      "Vector of density values that map one-to-one with the number "
      "'layer_ids' parameter.");
  return params;
}

ISoilAction::ISoilAction(const InputParameters & params) : Action(params) {}

void
ISoilAction::act()
{
  std::vector<SubdomainName> block = getParam<std::vector<SubdomainName>>("block");
  std::vector<VariableName> layer_variable = getParam<std::vector<VariableName>>("layer_variable");
  std::vector<unsigned int> layer_ids = getParam<std::vector<unsigned int>>("layer_ids");
  std::vector<Real> poissons_ratio = getParam<std::vector<Real>>("poissons_ratio");
  if (poissons_ratio.size() != layer_ids.size())
    mooseError("Error in" + name() + ". Poisson's ratio should be of the same size as layer_ids.");
  std::vector<Real> density = getParam<std::vector<Real>>("density");
  if (density.size() != layer_ids.size())
    mooseError("Error in" + name() + ". Density should be of the same size as layer_ids.");
  MooseEnum soil_type = getParam<MooseEnum>("soil_type");
  // Stress calculation
  InputParameters params = _factory.getValidParams("ComputeISoilStress");
  params.set<std::vector<unsigned int>>("layer_ids") = layer_ids;
  params.set<std::vector<VariableName>>("layer_variable") = layer_variable;
  params.set<std::vector<SubdomainName>>("block") = block;
  params.set<std::vector<Real>>("poissons_ratio") = poissons_ratio;
  params.set<Real>("b_exp") = getParam<Real>("b_exp");
  params.set<std::vector<Real>>("p_ref") = getParam<std::vector<Real>>("p_ref");
  params.set<Real>("a0") = getParam<Real>("a0");
  params.set<Real>("a1") = getParam<Real>("a1");
  params.set<Real>("a2") = getParam<Real>("a2");
  params.set<Real>("tension_pressure_cut_off") = getParam<Real>("tension_pressure_cut_off");
  params.set<bool>("pressure_dependency") = getParam<bool>("pressure_dependency");
  params.set<bool>("wave_speed_calculation") = true;
  params.set<std::vector<FunctionName>>("initial_soil_stress") =
      getParam<std::vector<FunctionName>>("initial_soil_stress");
  params.set<bool>("store_stress_old") = true;
  params.set<MooseEnum>("soil_type") = soil_type;
  // setting soiltype specific parameters
  if (soil_type == "user_defined")
    params.set<std::vector<FileName>>("backbone_curve_files") =
        getParam<std::vector<FileName>>("backbone_curve_files");
  if (soil_type == "darendeli")
  {
    params.set<std::vector<Real>>("initial_shear_modulus") =
        getParam<std::vector<Real>>("initial_shear_modulus");
    params.set<unsigned int>("number_of_points") = getParam<unsigned int>("number_of_points");
    params.set<std::vector<Real>>("over_consolidation_ratio") =
        getParam<std::vector<Real>>("over_consolidation_ratio");
    params.set<std::vector<Real>>("plasticity_index") =
        getParam<std::vector<Real>>("plasticity_index");
  }
  if (soil_type == "gqh")
  {
    params.set<std::vector<Real>>("initial_shear_modulus") =
        getParam<std::vector<Real>>("initial_shear_modulus");
    params.set<unsigned int>("number_of_points") = getParam<unsigned int>("number_of_points");
    params.set<std::vector<Real>>("theta_1") = getParam<std::vector<Real>>("theta_1");
    params.set<std::vector<Real>>("theta_2") = getParam<std::vector<Real>>("theta_2");
    params.set<std::vector<Real>>("theta_3") = getParam<std::vector<Real>>("theta_3");
    params.set<std::vector<Real>>("theta_4") = getParam<std::vector<Real>>("theta_4");
    params.set<std::vector<Real>>("theta_5") = getParam<std::vector<Real>>("theta_5");
    params.set<std::vector<Real>>("taumax") = getParam<std::vector<Real>>("taumax");
  }
  if (soil_type == "thin_layer")
  {
    params.set<std::vector<Real>>("initial_shear_modulus") =
        getParam<std::vector<Real>>("initial_shear_modulus");
    params.set<std::vector<Real>>("friction_coefficient") =
        getParam<std::vector<Real>>("friction_coefficient");
    params.set<std::vector<Real>>("hardening_ratio") =
        getParam<std::vector<Real>>("hardening_ratio");
  }
  _problem->addMaterial("ComputeISoilStress", "stress_" + block[0], params);

  // strain calculation
  std::vector<VariableName> displacements = getParam<std::vector<VariableName>>("displacements");
  bool finite_strain = getParam<bool>("finite_strain");
  if (!finite_strain && soil_type != "thin_layer")
  {
    // create small incremental strain block
    params = _factory.getValidParams("ComputeIncrementalSmallStrain");
    std::string unique_strain_name = "strain_" + block[0];
    params.set<std::vector<SubdomainName>>("block") = block;
    params.set<std::vector<VariableName>>("displacements") = displacements;
    // params.set<bool>("stateful_displacements") = true; // deprecated
    _problem->addMaterial("ComputeIncrementalSmallStrain", unique_strain_name, params);
  }
  else
  {
    if (soil_type == "thin_layer")
      // create finite strain block
      params = _factory.getValidParams("ComputeFiniteStrain");
    std::string unique_strain_name = "strain_" + block[0];
    params.set<std::vector<SubdomainName>>("block") = block;
    params.set<std::vector<VariableName>>("displacements") = displacements;
    // params.set<bool>("stateful_displacements") = true; // deprecated
    _problem->addMaterial("ComputeFiniteStrain", unique_strain_name, params);
  }

  // create Elasticty tensor with E = 1 and actual poissons ratio as input
  params = _factory.getValidParams("ComputeIsotropicElasticityTensorSoil");
  std::vector<Real> elastic_mod(layer_ids.size());
  for (std::size_t i = 0; i < layer_ids.size(); i++)
    elastic_mod[i] = 1.0;
  std::string unique_elasticity_name = "elasticity_" + block[0];
  params.set<std::vector<SubdomainName>>("block") = block;
  params.set<std::vector<Real>>("elastic_modulus") = elastic_mod;
  params.set<std::vector<Real>>("poissons_ratio") = poissons_ratio;
  params.set<std::vector<Real>>("density") = density;
  params.set<bool>("wave_speed_calculation") = false;
  params.set<std::vector<unsigned int>>("layer_ids") = layer_ids;
  params.set<std::vector<VariableName>>("layer_variable") = layer_variable;
  _problem->addMaterial("ComputeIsotropicElasticityTensorSoil", unique_elasticity_name, params);
}
