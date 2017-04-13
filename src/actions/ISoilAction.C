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

template <> InputParameters validParams<ISoilAction>() {
  InputParameters params = validParams<Action>();
  params.addClassDescription("Set up I-Soil material model");

  params.addRequiredParam<std::vector<NonlinearVariableName>>(
      "displacements", "The vector of displacement variables in the problem.");
  params.addRequiredParam<std::vector<SubdomainName>>(
      "block", "The blocks where this material model is applied.");
  params.addRequiredParam<NonlinearVariableName>(
      "layer_variable",
      "The variable providing the soil layer identification.");
  params.addRequiredParam<std::vector<unsigned int>>(
      "layer_ids", "Vector of layer ids that map one-to-one to the rest of the "
                   "soil layer parameters provided as input.");
  params.addParam<std::vector<FunctionName>>(
      "initial_stress",
      "The function values for the initial stress distribution. 9 function "
      "names have to be provided corresponding to stress_xx, stress_xy, "
      "stress_xz, stress_yx, stress_yy, stress_yz, stress_zx, stress_zy, "
      "stress_zz. Each function can be a function of space.");
  params.addRequiredParam<std::vector<Real>>(
      "poissons_ratio", "The poissons ratio for the different soil layers. The "
                        "size of the vector should be same as the size of "
                        "layer_ids.");
  params.addRequiredParam<unsigned int>(
      "soil_type",
      "This parameter takes the values 0, 1 and 2. All the soil layers need to "
      "have the same soil_type. If soil_type = 0, the user defined backbone "
      "stress-strain curve specified in the data_file is used. If soil_type = "
      "1, the backbone is obtained using Darendeli equations. If soiltype =2, "
      "the GQ/H model is used to determine the backbone stress-strain curve.");

  // params required for soil_type = 0 (user defined back bone curve)
  params.addParam<std::vector<FileName>>(
      "data_file", "The vector of file names of the files containing "
                   "stress-strain curves for the different soil layer. The "
                   "size of the vector should be same as the size of "
                   "layer_ids. The number of stress-strain points in each file "
                   "should be the same.");

  // params required for soil_type = 1 and 2 (darendeli and GQ/H)

  params.addParam<std::vector<Real>>(
      "initial_bulk_modulus", "The initial bulk modulus of the soil layers. "
                              "This is required for soil_type = 1 or 2.");
  params.addParam<std::vector<Real>>(
      "initial_shear_modulus", "The initial shear modulus of the soil layers. "
                               "This is required for soil_type = 1 or 2.");
  params.addParam<unsigned int>("number_of_points",
                                "The total number of data points in which the "
                                "backbone curve needs to be split for all soil "
                                "layers (required for soil_type = 1 and 2).");

  // params required for soil_type = 1 (darendeli)
  params.addParam<std::vector<Real>>("over_consolidation_ratio",
                                     "The over consolidation ratio of the soil "
                                     "layers. Required for soil_type = 1.");
  params.addParam<std::vector<Real>>(
      "plasticity_index",
      "The plasticity index of the soil layers. Required for soil_type = 1.");

  // params required for soil_type = 2 (GQ/H)
  params.addParam<std::vector<Real>>("theta_1", "The curve fit coefficient for "
                                                "GQ/H model (soil_type = 2) "
                                                "for each soil layer.");
  params.addParam<std::vector<Real>>("theta_2", "The curve fit coefficient for "
                                                "GQ/H model (soil_type = 2) "
                                                "for each soil layer.");
  params.addParam<std::vector<Real>>("theta_3", "The curve fit coefficient for "
                                                "GQ/H model (soil_type = 2) "
                                                "for each soil layer.");
  params.addParam<std::vector<Real>>("theta_4", "The curve fit coefficient for "
                                                "GQ/H model (soil_type = 2) "
                                                "for each soil layer.");
  params.addParam<std::vector<Real>>("theta_5", "The curve fit coefficient for "
                                                "GQ/H model (soil_type = 2) "
                                                "for each soil layer.");
  params.addParam<std::vector<Real>>("taumax", "The ultimate shear strength of "
                                               "the soil layers. Required for "
                                               "GQ/H model (soil_type =2).");

  // params required for pressure dependent stiffness
  params.addParam<bool>("pressure_dependency", false,
                        "Set to true to turn on pressure dependent stiffness "
                        "and yield strength calculation for all soil layers.");
  params.addParam<Real>("b_exp", 0.0, "The exponential factors for pressure "
                                      "dependent stiffness for all the soil "
                                      "layers.");
  params.addParam<Real>("tension_pressure_cut_off",
                        "The tension cut-off for all the soil layers. If the "
                        "pressure becomes lower than this value, then the "
                        "stiffness of the soil reduces to zero. A negative "
                        "pressure indicates tensile pressure. The default "
                        "value is -1.0 for all the soil layers.");
  params.addParam<std::vector<Real>>("p_ref", "The reference pressure at which "
                                              "the parameters are defined for "
                                              "each soil layer.");
  params.addParam<Real>(
      "a0", 1.0, "The first coefficient for pressure dependent yield strength "
                 "calculation for all the soil layers. If a0 = 1, a1 = 0 and "
                 "a2=0 for one soil layer,  then the yield strength of that "
                 "layer is independent of pressure.");
  params.addParam<Real>("a1", 0.0,
                        "The second coefficient for pressure dependent yield "
                        "strength calculation for all the soil layers. If a0 = "
                        "1, a1 = 0, a2 = 0 for one soil layer, then the yield "
                        "strength of that layer is independent of pressure.");
  params.addParam<Real>("a2", 0.0,
                        "The third coefficient for pressure dependent yield "
                        "strength calculation for all the soil layers. If a0 = "
                        "1, a1=0 and a2=0 for one soil layer, then the yield "
                        "strength of that layer is independent of preesue.");
  params.addParam<Real>("tension_pressure_cut_off", -1.0,
                        "The tension cut-off for all the soil layers. If the "
                        "pressure becomes lower than this value, then the "
                        "stiffness of the soil reduces to zero. A negative "
                        "pressure indicates tensile pressure. The default "
                        "value is -1.0 for all the soil layers.");
  params.addParam<std::vector<Real>>("p_ref", "The reference pressure at which "
                                              "the parameters are defined for "
                                              "each soil layer.");

  // flag to use finite strain calculator
  params.addParam<bool>("finite_strain", false, "Set to true to use finite "
                                                "strain calculator instead of "
                                                "incremental small strain.");

  params.addRequiredParam<std::vector<Real>>(
      "density", "Vector of density values that map one-to-one with the number "
                 "'layer_ids' parameter.");
  return params;
}

ISoilAction::ISoilAction(const InputParameters &params) : Action(params) {}

void ISoilAction::act() {

  std::vector<unsigned int> layer_ids =
      getParam<std::vector<unsigned int>>("layer_ids");

  // checks for poisson's ratio
  std::vector<Real> poissons_ratio =
      getParam<std::vector<Real>>("poissons_ratio");
  if (poissons_ratio.size() != layer_ids.size())
    mooseError("Error in" + name() +
               ". poissons_ratio should be same size as layer_ids.");

  // checks for density
  std::vector<Real> density = getParam<std::vector<Real>>("density");

  if (density.size() != layer_ids.size())
    mooseError("Error in" + name() +
               ".Density should be same size as layer_ids.");

  // checks for pressure dependency
  bool pressure_dependency = getParam<bool>("pressure_dependency");
  Real b_exp = getParam<Real>("b_exp");
  Real a0 = getParam<Real>("a0");
  Real a1 = getParam<Real>("a1");
  Real a2 = getParam<Real>("a2");
  Real tension_pressure_cut_off = getParam<Real>("tension_pressure_cut_off");
  std::vector<Real> p_ref = getParam<std::vector<Real>>("p_ref");

  if (pressure_dependency && b_exp == 0.0)
    mooseWarning("Warning in" + name() + ". Pressure dependency is set to true "
                                         "but b_exp is set to 0.0. Strength "
                                         "dependent pressure dependency is not "
                                         "turned on.");

  if (pressure_dependency && (a0 == 1.0 && a1 == 0.0 && a2 == 0.0))
    mooseWarning("Warning in" + name() +
                 ". Pressure dependency is set to true but a0, a1 and a2 are "
                 "set to 1.0, 0.0 and 0.0, respectively. Yield strength "
                 "pressure dependency is not turned on.");

  if (pressure_dependency && p_ref.size() != layer_ids.size())
    mooseError("Error in" + name() + ". When pressure dependency is turned on, "
                                     "a positive reference pressure "
                                     "(compressive) has to be defined for all "
                                     "the soil layers.");

  if (pressure_dependency) {
    for (unsigned int i = 0.0; i < layer_ids.size(); i++) {
      if (p_ref[i] <= 0.0)
        mooseError(
            "Error in" + name() +
            ". Please provide a positive value for the reference pressure.");
    }
  }

  // Backbone curve
  unsigned int soil_type = getParam<unsigned int>("soil_type");
  std::vector<std::vector<Real>> stress(layer_ids.size());
  std::vector<std::vector<Real>> strain(layer_ids.size());

  if (soil_type == 0)
    computeUserDefinedBackbone(stress, strain, layer_ids);
  else if (soil_type == 1)
    computeDarendeliBackBone(stress, strain, layer_ids);
  else if (soil_type == 2)
    computeGQHBackbone(stress, strain, layer_ids);
  else
    mooseError("Error in" + name() +
               ". The parameter soil_type can take only values 0, 1 and 2.");

  std::vector<std::vector<Real>> E0_component(layer_ids.size());
  std::vector<std::vector<Real>> scaled_yield0_component(layer_ids.size());

  // calculate youngs modulus and yield stress for each of the n curves for all
  // soil layers
  computeSoilLayerProperties(stress, strain, layer_ids, E0_component,
                             scaled_yield0_component, poissons_ratio);

  // Create stress, strain and elasticity tensor calculators
  std::vector<SubdomainName> block =
      getParam<std::vector<SubdomainName>>("block");
  NonlinearVariableName layer_var =
      getParam<NonlinearVariableName>("layer_variable");
  std::vector<VariableName> layer_variable;
  layer_variable.push_back(layer_var);

  // Stress calculation
  InputParameters params = _factory.getValidParams("ComputeISoilStress");

  std::vector<std::string> base_names(E0_component[0].size());
  for (unsigned int j = 0; j < E0_component[0].size(); j++)
    base_names[j] = "ISoil_" + Moose::stringify(j);

  params.set<std::vector<std::string>>("base_models") = base_names;
  params.set<std::vector<std::vector<Real>>>("youngs_modulus") = E0_component;
  params.set<std::vector<SubdomainName>>("block") = block;
  params.set<std::vector<std::vector<Real>>>("yield_stress") =
      scaled_yield0_component;
  params.set<std::vector<Real>>("poissons_ratio") = poissons_ratio;
  params.set<Real>("a0") = a0;
  params.set<Real>("a1") = a1;
  params.set<Real>("a2") = a2;
  params.set<Real>("b_exp") = b_exp;
  params.set<Real>("tension_pressure_cut_off") = tension_pressure_cut_off;
  params.set<std::vector<Real>>("p_ref") = p_ref;
  params.set<bool>("pressure_dependency") =
      getParam<bool>("pressure_dependency");
  params.set<bool>("store_stress_old") = true;
  params.set<std::vector<FunctionName>>("initial_stress") =
      getParam<std::vector<FunctionName>>("initial_stress");
  params.set<bool>("wave_speed_calculation") = true;
  params.set<std::vector<unsigned int>>("layer_ids") = layer_ids;
  params.set<std::vector<VariableName>>("layer_variable") = layer_variable;
  _problem->addMaterial("ComputeISoilStress", "stress_" + block[0], params);

  // strain calculation
  std::vector<NonlinearVariableName> displacements =
      getParam<std::vector<NonlinearVariableName>>("displacements");
  unsigned int ndisp = displacements.size();

  std::vector<VariableName> coupled_displacements;
  for (unsigned int i = 0; i < ndisp; ++i)
    coupled_displacements.push_back(displacements[i]);

  bool finite_strain = getParam<bool>("finite_strain");
  if (!finite_strain) {
    // create small incremental strain block
    params = _factory.getValidParams("ComputeIncrementalSmallStrain");
    std::string unique_strain_name = "strain_" + block[0];
    params.set<std::vector<SubdomainName>>("block") = block;
    params.set<std::vector<VariableName>>("displacements") =
        coupled_displacements;
    params.set<bool>("stateful_displacements") = true;
    _problem->addMaterial("ComputeIncrementalSmallStrain", unique_strain_name,
                          params);
  } else {
    // create finite strain block
    params = _factory.getValidParams("ComputeFiniteStrain");
    std::string unique_strain_name = "strain_" + block[0];
    params.set<std::vector<SubdomainName>>("block") = block;
    params.set<std::vector<VariableName>>("displacements") =
        coupled_displacements;
    params.set<bool>("stateful_displacements") = true;
    _problem->addMaterial("ComputeFiniteStrain", unique_strain_name, params);
  }

  // create Elasticty tensor with E = 1 and actual poissons ratio as input
  params = _factory.getValidParams("ComputeIsotropicElasticityTensorSoil");

  std::vector<Real> shear_mod(layer_ids.size());
  for (unsigned int i = 0; i < layer_ids.size(); i++)
    shear_mod[i] = 1.0 / (2.0 * (1.0 + poissons_ratio[i]));

  std::string unique_elasticity_name = "elasticity_" + block[0];
  params.set<std::vector<SubdomainName>>("block") = block;
  params.set<std::vector<Real>>("shear_modulus") = shear_mod;
  params.set<std::vector<Real>>("poissons_ratio") = poissons_ratio;
  params.set<std::vector<Real>>("density") = density;
  params.set<bool>("wave_speed_calculation") = false;
  params.set<std::vector<unsigned int>>("layer_ids") = layer_ids;
  params.set<std::vector<VariableName>>("layer_variable") = layer_variable;
  _problem->addMaterial("ComputeIsotropicElasticityTensorSoil",
                        unique_elasticity_name, params);
}

void ISoilAction::computeUserDefinedBackbone(
    std::vector<std::vector<Real>> &stress,
    std::vector<std::vector<Real>> &strain,
    std::vector<unsigned int> &layer_ids) {
  std::vector<FileName> data_file =
      getParam<std::vector<FileName>>("data_file");
  if (data_file.size() != layer_ids.size())
    mooseError("Error in" + name() + ". The vector of data file names needs to "
                                     "be provided and the size of this vector "
                                     "should be same as that of layers_ids.");

  for (unsigned int i = 0.0; i < layer_ids.size(); i++)
    parseColumns(strain[i], stress[i], data_file[i]);
}

void ISoilAction::computeDarendeliBackBone(
    std::vector<std::vector<Real>> &stress,
    std::vector<std::vector<Real>> &strain,
    std::vector<unsigned int> &layer_ids) {
  std::vector<Real> initial_bulk_modulus =
      getParam<std::vector<Real>>("initial_bulk_modulus");
  std::vector<Real> initial_shear_modulus =
      getParam<std::vector<Real>>("initial_shear_modulus");
  std::vector<Real> over_consolidation_ratio =
      getParam<std::vector<Real>>("over_consolidation_ratio");
  std::vector<Real> plasticity_index =
      getParam<std::vector<Real>>("plasticity_index");
  std::vector<Real> p_ref = getParam<std::vector<Real>>("p_ref");

  unsigned int number_of_points = getParam<unsigned int>("number_of_points");

  if (initial_bulk_modulus.size() != layer_ids.size() ||
      initial_shear_modulus.size() != layer_ids.size() ||
      over_consolidation_ratio.size() != layer_ids.size() ||
      plasticity_index.size() != layer_ids.size() ||
      p_ref.size() != layer_ids.size())
    mooseError("Error in" + name() +
               ". initial_bulk_modulus, initial_shear_modulus, "
               "over_consolidation_ratio, plasticity_index and p_ref must be "
               "of the same size as layer_ids.");

  for (unsigned int i = 0.0; i < layer_ids.size(); i++) {
    if (over_consolidation_ratio[i] <= 0.0 || plasticity_index[i] < 0.0 ||
        number_of_points <= 0 || initial_bulk_modulus[i] <= 0.0 ||
        initial_shear_modulus[i] <= 0.0)
      mooseError("Error in" + name() + ". Positive values have to be provided "
                                       "for over_consolidation_ratio, "
                                       "plasticity_index, number_of_points, "
                                       "initial_bulk_modulus and "
                                       "initial_bulk_modulus.");

    strain[i].resize(number_of_points);
    stress[i].resize(number_of_points);
    Real phi1 = 0.0352;
    Real phi2 = 0.001;
    Real phi3 = 0.3246;
    Real phi4 = 0.3483;
    Real phi5 = 0.9190;
    Real ref_strain =
        (phi1 +
         phi2 * plasticity_index[i] * pow(over_consolidation_ratio[i], phi3)) *
        pow(p_ref[i] * 0.00987, phi4);
    for (unsigned int j = 0; j < number_of_points; j++) {
      strain[i][j] = pow(10.0, -6.0 + 5.0 / (number_of_points - 1) * j);
      stress[i][j] =
          initial_shear_modulus[i] *
          (1.0 / (1.0 + pow(100.0 * strain[i][j] / ref_strain, phi5))) *
          strain[i][j];
    }
  }
}

void ISoilAction::computeGQHBackbone(std::vector<std::vector<Real>> &stress,
                                     std::vector<std::vector<Real>> &strain,
                                     std::vector<unsigned int> &layer_ids) {
  std::vector<Real> initial_bulk_modulus =
      getParam<std::vector<Real>>("initial_bulk_modulus");
  std::vector<Real> initial_shear_modulus =
      getParam<std::vector<Real>>("initial_shear_modulus");
  unsigned int number_of_points = getParam<unsigned int>("number_of_points");
  std::vector<Real> theta_1 = getParam<std::vector<Real>>("theta_1");
  std::vector<Real> theta_2 = getParam<std::vector<Real>>("theta_2");
  std::vector<Real> theta_3 = getParam<std::vector<Real>>("theta_3");
  std::vector<Real> theta_4 = getParam<std::vector<Real>>("theta_4");
  std::vector<Real> theta_5 = getParam<std::vector<Real>>("theta_5");
  std::vector<Real> taumax = getParam<std::vector<Real>>("taumax");
  std::vector<std::vector<Real>> modulus(layer_ids.size());

  if (initial_bulk_modulus.size() != layer_ids.size() ||
      initial_shear_modulus.size() != layer_ids.size() ||
      theta_1.size() != layer_ids.size() ||
      theta_2.size() != layer_ids.size() ||
      theta_3.size() != layer_ids.size() ||
      theta_4.size() != layer_ids.size() ||
      theta_5.size() != layer_ids.size() || taumax.size() != layer_ids.size())
    mooseError("Error in" + name() +
               ". initial_bulk_modulus, initial_shear_modulus, theta_1, "
               "theta_2, theta_3, theta_4, theta_5 and taumax should be of the "
               "same size as layer_ids.");

  for (unsigned int i = 0; i < layer_ids.size(); i++) {
    if (theta_1[i] + theta_2[i] + theta_3[i] + theta_4[i] + theta_5[i] > 1.0)
      mooseError("Error in" + name() + "Sum of theta 1 through 5 should be "
                                       "smaller or equal to 1 for each soil "
                                       "layer.");

    if (number_of_points <= 0 || taumax[i] <= 0.0 ||
        initial_shear_modulus[i] <= 0.0 || initial_bulk_modulus[i] <= 0.0)
      mooseError("Error in" + name() +
                 ". Please provide positive values for number of points, "
                 "taumax, initial_shear_modulus and intial_bulk_modulus.");

    strain[i].resize(number_of_points);
    stress[i].resize(number_of_points);
    modulus[i].resize(number_of_points);
    for (unsigned int j = 0; j < number_of_points; ++j) {
      strain[i][j] = pow(10.0, -6.0 + 5.0 / (number_of_points - 1) * j);
      Real theta =
          theta_1[i] +
          theta_2[i] *
              pow(strain[i][j] / (taumax[i] / initial_shear_modulus[i]),
                  theta_5[i]) /
              (pow(theta_3[i], theta_5[i]) +
               pow(strain[i][j] / (taumax[i] / initial_shear_modulus[i]),
                   theta_5[i]));
      modulus[i][j] =
          (theta == 0)
              ? 1.0 / (1.0 +
                       strain[i][j] / (taumax[i] / initial_shear_modulus[i]))
              : (1.0 /
                 (strain[i][j] / (taumax[i] / initial_shear_modulus[i]))) *
                    (0.5 / theta *
                     (1.0 +
                      strain[i][j] / (taumax[i] / initial_shear_modulus[i]) -
                      sqrt(pow(1.0 +
                                   strain[i][j] /
                                       (taumax[i] / initial_shear_modulus[i]),
                               2.0) -
                           4.0 * theta * strain[i][j] /
                               (taumax[i] / initial_shear_modulus[i]))));
      stress[i][j] = modulus[i][j] * initial_shear_modulus[i] * strain[i][j];
    }
  }
}

void ISoilAction::computeSoilLayerProperties(
    std::vector<std::vector<Real>> &stress,
    std::vector<std::vector<Real>> &strain,
    std::vector<unsigned int> &layer_ids,
    std::vector<std::vector<Real>> &E0_component,
    std::vector<std::vector<Real>> &scaled_yield0_component,
    std::vector<Real> &poissons_ratio) {
  for (unsigned int k = 0; k < layer_ids.size(); k++) {
    Real number = strain[k].size();

    // Calculating the Youngs modulus for each of the n curves for soil layer k
    ColumnMajorMatrix A(number, number);
    ColumnMajorMatrix InvA(number, number);

    for (unsigned int i = 0; i < number; i++) {
      for (unsigned int j = 0; j < number; j++) {
        if (i <= j)
          A(i, j) = strain[k][i];
        else
          A(i, j) = strain[k][j];
      }
    }

    // create upper triangular matrix and modified stress
    E0_component[k].resize(number);
    std::vector<Real> G0_component(number);
    scaled_yield0_component[k].resize(number);
    std::vector<Real> modified_stress(number);
    InvA = A;
    for (unsigned int i = 1; i < number; i++) {
      for (unsigned int j = 0; j < number; j++) {
        InvA(i, j) = A(i, j) - A(i - 1, j);
        modified_stress[i] = stress[k][i] - stress[k][i - 1];
      }
    }
    modified_stress[0] = stress[k][0];

    // backward substitution
    G0_component[number - 1] =
        modified_stress[number - 1] / InvA(number - 1, number - 1);
    scaled_yield0_component[k][number - 1] = strain[k][number - 1];
    int i = number - 2;
    while (i >= 0) {
      G0_component[i] = 0.0;
      for (unsigned int j = i + 1; j < number; j++)
        G0_component[i] += InvA(i, j) * G0_component[j];

      G0_component[i] = (modified_stress[i] - G0_component[i]) / InvA(i, i);
      scaled_yield0_component[k][i] = strain[k][i];
      i = i - 1;
    }

    // scaling
    for (unsigned int i = 0; i < number; i++) {
      E0_component[k][i] = G0_component[i] * 2.0 * (1.0 + poissons_ratio[k]);
      scaled_yield0_component[k][i] = scaled_yield0_component[k][i] *
                                      std::sqrt(3.0) /
                                      (2.0 * (1.0 + poissons_ratio[k]));
    }
  }
}

bool ISoilAction::parseNextLineReals(std::ifstream &ifs,
                                     std::vector<Real> &myvec) {
  std::string line;
  myvec.clear();
  bool gotline(false);
  if (getline(ifs, line)) {
    gotline = true;

    // Replace all commas with spaces
    while (size_t pos = line.find(',')) {
      if (pos == line.npos)
        break;
      line.replace(pos, 1, 1, ' ');
    }

    // Harvest floats separated by whitespace
    std::istringstream iss(line);
    Real f;
    while (iss >> f) {
      myvec.push_back(f);
    }
  }
  return gotline;
}

void ISoilAction::parseColumns(std::vector<Real> &x, std::vector<Real> &y,
                               FileName &data_file) {
  std::ifstream file(data_file.c_str());
  if (!file.good())
    mooseError("In ", name(), ": Error opening file '" + data_file + "'.");

  std::vector<Real> scratch;
  unsigned int x_index = 0;
  unsigned int y_index = 1;

  unsigned int line_index = 0;
  while (parseNextLineReals(file, scratch)) {
    if (scratch.size() > 0) {
      if (x_index < scratch.size())
        x.push_back(scratch[x_index]);
      else
        mooseError("In ", name(), ": column ", x_index,
                   " for x does not exist on line ", line_index);

      if (y_index < scratch.size())
        y.push_back(scratch[y_index]);
      else
        mooseError("In ", name(), ": column ", y_index,
                   " for y does not exist on line ", line_index);

      if (scratch.size() != 2)
        mooseError("In ", name(),
                   ": Read more than 2 columns of data from file '" +
                       data_file + "'.");
    }

    line_index++;
  }
}
