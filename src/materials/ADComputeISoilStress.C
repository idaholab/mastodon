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

#include "ADComputeISoilStress.h"

#include "MooseMesh.h"
#include "Function.h"
#include "MastodonUtils.h"
#include "ISoilUtils.h"
#include "Conversion.h"
#include "FEProblem.h"
#include "MooseUtils.h"

#include "metaphysicl/numberarray.h"
#include "metaphysicl/dualnumber.h"

registerMooseObject("MastodonApp", ADComputeISoilStress);

InputParameters
ADComputeISoilStress::validParams()
{
  InputParameters params = ADComputeFiniteStrainElasticStress::validParams();
  params.addClassDescription(
      "Compute total stress for the nonlinear material "
      "model I-Soil using a backbone curve and automatically computes the Jacobian.");
  params.addRequiredCoupledVar("layer_variable",
                               "The auxvariable providing the soil layer identification.");
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
                                     {},
                                     "The reference pressure at which "
                                     "the parameters are defined for "
                                     "each soil layer. If 'soil_type = "
                                     "darendeli', then the reference "
                                     "pressure must be input in kilopascals.");
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
  params.addParam<bool>(
      "wave_speed_calculation", true, "Set to false to turn off P and S wave speed calculation.");
  params.addParam<std::vector<FunctionName>>(
      "initial_soil_stress",
      {},
      "A list of functions describing the initial stress. There "
      "must be 9 functions, corresponding to the xx, yx, zx, xy, yy, zy, xz, yz, "
      "zz components respectively. If not provided, all components of the "
      "initial stress will be zero.");
  // params for specific backbone types
  MooseEnum soil_type("user_defined darendeli gqh thin_layer");
  params.addRequiredParam<MooseEnum>(
      "soil_type",
      soil_type,
      "This parameter determines the type of backbone curve used. Use 'user_defined' "
      "for a user defined backbone curve provided in a data file, 'darendeli' "
      "if the backbone curve is to be determined using Darandeli equations, 'gqh' "
      "if the backbone curve is determined using the GQ/H approach and 'thin_layer' if the soil "
      "is "
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
  // params required for soil_type = 'darendeli', 'GQ/H' and 'thin_layer'
  params.addParam<std::vector<Real>>(
      "initial_shear_modulus",
      "The initial shear modulus of the soil layers. "
      "This is required if Darandeli or GQ/H type backbone curves are used.");
  // params required for soil_type = 'darendeli' and 'GQ/H'
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
                                     "Post-yield hardening ratios of the layers.");
  return params;
}

ADComputeISoilStress::ADComputeISoilStress(const InputParameters & parameters)
  : ADComputeFiniteStrainElasticStress(parameters),
    _base_models(),
    _stress_model(),
    _stress_model_old(),
    _yield_stress(), // *** YIELD STRAIN NOT YIELD STRESS ***
    _youngs(),
    _soil_layer_variable(coupledValue("layer_variable")),
    _layer_ids(getParam<std::vector<unsigned int>>("layer_ids")),
    _wave_speed_calculation(getParam<bool>("wave_speed_calculation")),
    _poissons_ratio(getParam<std::vector<Real>>("poissons_ratio")),
    _density(_wave_speed_calculation ? &getADMaterialProperty<Real>("density") : nullptr),
    _b_exp(getParam<Real>("b_exp")),
    _p_ref(getParam<std::vector<Real>>("p_ref")),
    _a0(getParam<Real>("a0")),
    _a1(getParam<Real>("a1")),
    _a2(getParam<Real>("a2")),
    _p0(getParam<Real>("tension_pressure_cut_off")),
    _pressure_dependency(getParam<bool>("pressure_dependency")),
    _strength_pressure_correction(1.0),
    _stiffness_pressure_correction(1.0),
    _shear_wave_speed(_wave_speed_calculation ? &declareADProperty<Real>("shear_wave_speed")
                                              : nullptr),
    _P_wave_speed(_wave_speed_calculation ? &declareADProperty<Real>("P_wave_speed") : nullptr),
    _pos(0),
    _initial_soil_stress_provided(
        getParam<std::vector<FunctionName>>("initial_soil_stress").size() ==
        LIBMESH_DIM * LIBMESH_DIM)
{
  // checking that density, and Poisson's ratio are the same size as layer_ids
  if (_poissons_ratio.size() != _layer_ids.size())
    mooseError("Error in ADComputeISoilStress. Poisson's ratio should be of the same "
               "size as layer_ids.");
  // checks for pressure dependency
  if (_pressure_dependency && _b_exp == 0.0)
    mooseWarning("Warning in ADComputeISoilStress. Pressure dependency is set to true "
                 "but b_exp is set to 0.0. Stiffness "
                 "pressure dependency is NOT "
                 "turned on.");
  if (_pressure_dependency && (_a0 == 1.0 && _a1 == 0.0 && _a2 == 0.0))
    mooseWarning(
        "Warning in ADComputeISoilStress. Pressure dependency is set to true but a0, a1 and a2 are "
        "set to 1.0, 0.0 and 0.0, respectively. Strength "
        "pressure dependency is NOT turned on.");
  if (_pressure_dependency && _p_ref.size() != _layer_ids.size())
    mooseError("Error in ADComputeISoilStress. When pressure dependency is turned on, "
               "a positive reference pressure "
               "(compressive) has to be defined for all "
               "the soil layers and the same number of reference "
               "pressures as soil layers should be provided.");
  if (_pressure_dependency && MastodonUtils::isNegativeOrZero(_p_ref))
    mooseError("Error in ADComputeISoilStress. Please provide positive (compressive) values for "
               "reference pressure.");

  // Initializing backbone curve
  const MooseEnum & soil_type = getParam<MooseEnum>("soil_type");
  std::vector<std::vector<Real>> backbone_stress(_layer_ids.size());
  std::vector<std::vector<Real>> backbone_strain(_layer_ids.size());
  // Calculating backbone curve for soil_type = user_defined
  if (soil_type == "user_defined")
  {
    std::vector<FileName> backbone_curve_files =
        getParam<std::vector<FileName>>("backbone_curve_files");
    if (backbone_curve_files.size() != _layer_ids.size())
      mooseError("Error in ADComputeISoilStress. A vector of file names needs to "
                 "be provided for `backbone_curve_files` and the size of this vector "
                 "should be same as that of `layers_ids`.");
    ISoilUtils::computeUserDefinedBackbone(
        backbone_stress, backbone_strain, _layer_ids, backbone_curve_files, "ADComputeISoilStress");
  }
  // Calculating backbone curve for soil_type = darendeli
  else if (soil_type == "darendeli")
  {
    const std::vector<Real> initial_shear_modulus =
        getParam<std::vector<Real>>("initial_shear_modulus");
    const std::vector<Real> over_consolidation_ratio =
        getParam<std::vector<Real>>("over_consolidation_ratio");
    const std::vector<Real> plasticity_index = getParam<std::vector<Real>>("plasticity_index");
    unsigned int number_of_points = getParam<unsigned int>("number_of_points");
    if (initial_shear_modulus.size() != _layer_ids.size() ||
        over_consolidation_ratio.size() != _layer_ids.size() ||
        plasticity_index.size() != _layer_ids.size())
      mooseError("Error in ADComputeISoilStress. initial_shear_modulus, "
                 "over_consolidation_ratio and plasticity_index must be "
                 "of the same size as layer_ids.");
    if (MastodonUtils::isNegativeOrZero(over_consolidation_ratio) || number_of_points <= 0 ||
        MastodonUtils::isNegativeOrZero(initial_shear_modulus))
      mooseError("Error in ADComputeISoilStress. Positive values have to be provided "
                 "for over_consolidation_ratio, "
                 "number_of_points, "
                 "and initial_shear_modulus.");
    ISoilUtils::computeDarendeliBackbone(backbone_stress,
                                         backbone_strain,
                                         _layer_ids,
                                         initial_shear_modulus,
                                         over_consolidation_ratio,
                                         plasticity_index,
                                         _p_ref,
                                         number_of_points,
                                         "ADComputeISoilStress");
  }
  // Calculating backbone curve for soil_type = gqh
  else if (soil_type == "gqh")
  {
    const std::vector<Real> initial_shear_modulus =
        getParam<std::vector<Real>>("initial_shear_modulus");
    unsigned int number_of_points = getParam<unsigned int>("number_of_points");
    const std::vector<Real> theta_1 = getParam<std::vector<Real>>("theta_1");
    const std::vector<Real> theta_2 = getParam<std::vector<Real>>("theta_2");
    const std::vector<Real> theta_3 = getParam<std::vector<Real>>("theta_3");
    const std::vector<Real> theta_4 = getParam<std::vector<Real>>("theta_4");
    const std::vector<Real> theta_5 = getParam<std::vector<Real>>("theta_5");
    const std::vector<Real> taumax = getParam<std::vector<Real>>("taumax");
    const std::vector<std::vector<Real>> modulus(_layer_ids.size());
    if (initial_shear_modulus.size() != _layer_ids.size() || theta_1.size() != _layer_ids.size() ||
        theta_2.size() != _layer_ids.size() || theta_3.size() != _layer_ids.size() ||
        theta_4.size() != _layer_ids.size() || theta_5.size() != _layer_ids.size() ||
        taumax.size() != _layer_ids.size())
      mooseError("Error in ADComputeISoilStress. initial_shear_modulus, theta_1, "
                 "theta_2, theta_3, theta_4, theta_5 and taumax should be of the "
                 "same size as layer_ids.");
    if (number_of_points <= 0 || MastodonUtils::isNegativeOrZero(taumax) ||
        MastodonUtils::isNegativeOrZero(initial_shear_modulus))
      mooseError(
          "Error in ADComputeISoilStress. Please provide positive values for number of points, "
          "taumax and initial_shear_modulus.");
    ISoilUtils::computeGQHBackbone(backbone_stress,
                                   backbone_strain,
                                   _layer_ids,
                                   initial_shear_modulus,
                                   number_of_points,
                                   theta_1,
                                   theta_2,
                                   theta_3,
                                   theta_4,
                                   theta_5,
                                   taumax);
  }
  // Calculating backbone curve for soil_type = thin_layer
  else if (soil_type == "thin_layer")
  {
    _pressure_dependency = true;
    _a0 = 0.0;
    _a1 = 0.0;
    _a2 = 1.0;
    std::vector<Real> initial_shear_modulus = getParam<std::vector<Real>>("initial_shear_modulus");
    std::vector<Real> friction_coefficient = getParam<std::vector<Real>>("friction_coefficient");
    std::vector<Real> hardening_ratio = getParam<std::vector<Real>>("hardening_ratio");
    if (initial_shear_modulus.size() != _layer_ids.size() ||
        friction_coefficient.size() != _layer_ids.size() ||
        hardening_ratio.size() != _layer_ids.size())
      mooseError("Error in ADComputeISoilStress. initial_shear_modulus, friction_coefficient, "
                 "hardening_ratio and p_ref must be "
                 "of the same size as layer_ids.");
    if (MastodonUtils::isNegativeOrZero(initial_shear_modulus) ||
        MastodonUtils::isNegativeOrZero(friction_coefficient) ||
        MastodonUtils::isNegativeOrZero(hardening_ratio))
      mooseError("Error in ADComputeISoilStress. Positive values have to be provided "
                 "for `initial_shear_modulus`, "
                 "`friction_coefficient` and `hardening_ratio`");
    ISoilUtils::computeCoulombBackbone(backbone_stress,
                                       backbone_strain,
                                       _layer_ids,
                                       initial_shear_modulus,
                                       friction_coefficient,
                                       hardening_ratio,
                                       _p_ref,
                                       "ADComputeISoilStress");
  }
  else
    mooseError("Error in ADComputeISoilStress. The parameter soil_type is invalid.");
  // Deconstructing the backbone curves for all the soil layers into
  // elastic-perfectly-plastic components. Each backbone curve is split up into
  // a set of youngs modulus and yield stress pairs.
  _youngs.resize(_layer_ids.size());
  _yield_stress.resize(_layer_ids.size());
  computeSoilLayerProperties(_youngs,
                             _yield_stress, // *** CALCULATES YIELD STRAIN NOT YIELD STRESS ***
                             backbone_stress,
                             backbone_strain,
                             _layer_ids,
                             _poissons_ratio,
                             "ADComputeISoilStress");
  _stress_model.resize(_youngs[0].size());
  _stress_model_old.resize(_youngs[0].size());
  _base_models.resize(_youngs[0].size());
  for (std::size_t i = 0; i < _youngs[0].size(); i++)
  {
    _base_models[i] = Moose::stringify(i);
    _stress_model[i] = &declareADProperty<RankTwoTensor>(_base_models[i] + "_stress_model");
    _stress_model_old[i] =
        &getMaterialPropertyOldByName<RankTwoTensor>(_base_models[i] + "_stress_model");
  }

  const std::vector<FunctionName> & fcn_names(
      getParam<std::vector<FunctionName>>("initial_soil_stress"));
  const unsigned num = fcn_names.size();

  if (!(num == 0 || num == LIBMESH_DIM * LIBMESH_DIM))
    mooseError("Either zero or ",
               LIBMESH_DIM * LIBMESH_DIM,
               " initial soil stress functions must be provided.  You supplied ",
               num,
               "\n");

  _initial_soil_stress.resize(num);
  for (unsigned i = 0; i < num; ++i)
    _initial_soil_stress[i] = &getFunctionByName(fcn_names[i]);

  _stress_new.zero();
  _individual_stress_increment.zero();
  _deviatoric_trial_stress.zero();
}

void
ADComputeISoilStress::initQpStatefulProperties()
{
  ADComputeStressBase::initQpStatefulProperties();
  if (_initial_soil_stress_provided)
  {
    for (unsigned i = 0; i < LIBMESH_DIM; ++i)
      for (unsigned j = 0; j < LIBMESH_DIM; ++j)
        _stress[_qp](i, j) = _initial_soil_stress[i * LIBMESH_DIM + j]->value(_t, _q_point[_qp]);
  }
  for (std::size_t i = 0; i < _base_models.size(); i++)
    (*_stress_model[i])[_qp].zero();

  // Determine the current id for the soil. The variable which is a Real must be
  // converted to a unsigned int for lookup, so first
  // it is rounded to avoid Real values that are just below the desired value.
  _current_id = static_cast<unsigned int>(std::round(_soil_layer_variable[_qp]));
  // Get the position of the current id in the layer_ids array
  _pos = find(_layer_ids.begin(), _layer_ids.end(), _current_id) - _layer_ids.begin();

  using std::sqrt, std::pow;
  if (_wave_speed_calculation)
  {
    ADReal initial_youngs = 0.0;
    for (std::size_t i = 0; i < _base_models.size(); i++)
      initial_youngs += _youngs[_pos][i];

    // shear wave speed is sqrt(shear_modulus/density)
    (*_shear_wave_speed)[_qp] =
        sqrt(initial_youngs / (2.0 * (1.0 + _poissons_ratio[_pos])) / (*_density)[_qp]);

    // P wave speed is sqrt(P wave modulus/density)
    (*_P_wave_speed)[_qp] =
        sqrt(initial_youngs * (1.0 - _poissons_ratio[_pos]) / (1.0 + _poissons_ratio[_pos]) /
             (1.0 - 2.0 * _poissons_ratio[_pos]) / (*_density)[_qp]);
  }

  // determine the lateral and vertical stresses
  ADReal residual_vertical = _stress[_qp](2, 2);
  ADReal residual_xx = _stress[_qp](0, 0);
  ADReal residual_yy = _stress[_qp](1, 1);
  ADReal mean_stress = _stress[_qp].trace() / (-3.0);

  if (_pressure_dependency)
  {
    if (!MooseUtils::absoluteFuzzyEqual(mean_stress - _p0, 0.0))
      _stiffness_pressure_correction = pow((mean_stress - _p0) / _p_ref[_pos], _b_exp);
    else
      _stiffness_pressure_correction = 0.0;

    if (!MooseUtils::absoluteFuzzyEqual(
            _a0 + _a1 * (mean_stress - _p0) + _a2 * (mean_stress - _p0) * (mean_stress - _p0), 0.0))
      _strength_pressure_correction =
          sqrt(_a0 + _a1 * (mean_stress - _p0) + _a2 * (mean_stress - _p0) * (mean_stress - _p0)) /
          sqrt(_a0 + _a1 * (_p_ref[_pos]) + _a2 * (_p_ref[_pos]) * (_p_ref[_pos]));
    else
      _strength_pressure_correction = 0.0;
  }

  // Calculate the K0 consistent stress distribution
  ADRankTwoTensor dev_model;
  for (std::size_t i = 0; i < _base_models.size(); i++)
  {
    ADReal _mean_pressure = 0.0;
    if (residual_vertical != 0.0)
    {
      ADReal sum_youngs = 0.0;

      for (std::size_t j = i; j < _base_models.size(); j++)
        sum_youngs += _youngs[_pos][j];

      (*_stress_model[i])[_qp](2, 2) = residual_vertical * _youngs[_pos][i] / sum_youngs;
      (*_stress_model[i])[_qp](0, 0) = residual_xx * _youngs[_pos][i] / sum_youngs;
      (*_stress_model[i])[_qp](1, 1) = residual_yy * _youngs[_pos][i] / sum_youngs;
      dev_model = ((*_stress_model[i])[_qp]).deviatoric() / _youngs[_pos][i];
      _mean_pressure = (*_stress_model[i])[_qp].trace() / 3.0;
      ADReal J2_model = dev_model.doubleContraction(dev_model);
      ADReal dev_stress_model = 0.0;
      if (!MooseUtils::absoluteFuzzyEqual(J2_model, 0.0))
        dev_stress_model = sqrt(3.0 / 2.0 * J2_model);
      if (dev_stress_model > _yield_stress[_pos][i] * _strength_pressure_correction)
        dev_model *= (_yield_stress[_pos][i] * _strength_pressure_correction) / dev_stress_model;

      (*_stress_model[i])[_qp] = dev_model * _youngs[_pos][i]; // stress_model contains only the
                                                               // deviatoric part of the stress
    }
    residual_vertical = residual_vertical - (*_stress_model[i])[_qp](2, 2) - _mean_pressure;
    residual_xx = residual_xx - (*_stress_model[i])[_qp](0, 0) - _mean_pressure;
    residual_yy = residual_yy - (*_stress_model[i])[_qp](1, 1) - _mean_pressure;
  }
}

void
ADComputeISoilStress::computeQpStress()
{
  // Nothing to update during the first time step, return immediately
  if (_t_step == 0)
    return;

  _stress_new.zero();
  computeStress();
  _stress[_qp] = _rotation_increment[_qp] * _stress_new * _rotation_increment[_qp].transpose();
}

void
ADComputeISoilStress::computeStress()
{
  if (_t_step == 0)
    return;

  using std::round, std::pow, std::sqrt;

  // Determine the current id for the soil. The variable which is a Real must be
  // converted to a unsigned int for lookup, so first
  // it is rounded to avoid Real values that are just below the desired value.
  _current_id = static_cast<unsigned int>(round(_soil_layer_variable[_qp]));

  // Get the position of the current id in the layer_ids array
  _pos = find(_layer_ids.begin(), _layer_ids.end(), _current_id) - _layer_ids.begin();

  _individual_stress_increment.zero();

  _deviatoric_trial_stress.zero();

  // current pressure calculation
  ADReal mean_stress = _stress_old[_qp].trace() / (-3.0);

  if (mean_stress < _p0)
    mean_stress = 0.0;

  if (_pressure_dependency)
  {
    if (!MooseUtils::absoluteFuzzyEqual(mean_stress - _p0, 0.0))
      _stiffness_pressure_correction = pow((mean_stress - _p0) / _p_ref[_pos], _b_exp);
    else
      _stiffness_pressure_correction = 0.0;

    if (!MooseUtils::absoluteFuzzyEqual(
            _a0 + _a1 * (mean_stress - _p0) + _a2 * (mean_stress - _p0) * (mean_stress - _p0), 0.0))
      _strength_pressure_correction =
          sqrt(_a0 + _a1 * (mean_stress - _p0) + _a2 * (mean_stress - _p0) * (mean_stress - _p0)) /
          sqrt(_a0 + _a1 * (_p_ref[_pos]) + _a2 * (_p_ref[_pos]) * (_p_ref[_pos]));
    else
      _strength_pressure_correction = 0.0;
  }

  _mean_pressure = 0.0;

  // compute trial stress increment - note that _elasticity_tensor here
  // assumes youngs modulus = 1

  ADReal lame_1 = _elasticity_tensor[_qp](0, 0, 1, 1);
  ADReal lame_2 = (_elasticity_tensor[_qp](0, 0, 0, 0) - lame_1) / 2;
  ADReal initial_youngs = (lame_2 * (3 * lame_1 + 2 * lame_2)) / (lame_1 + lame_2);

  _individual_stress_increment =
      _elasticity_tensor[_qp] * (_strain_increment[_qp]) / initial_youngs;

  _mean_pressure_tmp = _individual_stress_increment.trace() / 3.0 * _stiffness_pressure_correction;

  _deviatoric_trial_stress_tmp =
      _individual_stress_increment.deviatoric() * _stiffness_pressure_correction;

  _dev_trial_stress_squared = 0.0;

  _effective_trial_stress = 0.0;

  _yield_condition = 0.0;

  for (std::size_t i = 0; i < _base_models.size(); i++)
  {

    // calculate pressure for each element
    _mean_pressure += _mean_pressure_tmp * _youngs[_pos][i];

    // compute the deviatoric trial stress normalized by non pressure dependent
    // youngs modulus.
    _deviatoric_trial_stress =
        _deviatoric_trial_stress_tmp + (*_stress_model_old[i])[_qp] / (_youngs[_pos][i]);

    // compute the effective trial stress
    _dev_trial_stress_squared =
        _deviatoric_trial_stress.doubleContraction(_deviatoric_trial_stress);

    if (!MooseUtils::absoluteFuzzyEqual(_dev_trial_stress_squared, 0.0))
      _effective_trial_stress = sqrt(3.0 / 2.0 * _dev_trial_stress_squared);

    // check yield condition and calculate plastic strain
    _yield_condition =
        _effective_trial_stress - _yield_stress[_pos][i] * _strength_pressure_correction;

    if (_yield_condition > 0.0)
      _deviatoric_trial_stress *=
          _yield_stress[_pos][i] * _strength_pressure_correction / _effective_trial_stress;

    (*_stress_model[i])[_qp] = _youngs[_pos][i] * (_deviatoric_trial_stress);

    _stress_new += (*_stress_model[i])[_qp];
  }
  _stress_new(0, 0) += _mean_pressure - mean_stress;
  _stress_new(1, 1) += _mean_pressure - mean_stress;
  _stress_new(2, 2) += _mean_pressure - mean_stress;
}

void
ADComputeISoilStress::computeSoilLayerProperties(
    std::vector<std::vector<ADReal>> & youngs,
    std::vector<std::vector<ADReal>> & yield_stress, // *** YIELD STRAIN, NOT YIELD STRESS***
    const std::vector<std::vector<Real>> & backbone_stress,
    const std::vector<std::vector<Real>> & backbone_strain,
    const std::vector<unsigned int> & layer_ids,
    const std::vector<Real> & poissons_ratio,
    const std::string & name)
{
  for (std::size_t k = 0; k < layer_ids.size(); k++)
  {
    unsigned int number = backbone_strain[k].size();

    // Calculating the Youngs modulus for each of the n curves for soil layer k
    std::vector<std::vector<ADReal>> A(number);
    std::vector<std::vector<ADReal>> InvA(number);

    for (std::size_t i = 0; i < number; i++)
    {
      A[i].resize(number);
      for (std::size_t j = 0; j < number; j++)
      {
        if (i <= j)
          A[i][j] = backbone_strain[k][i];
        else
          A[i][j] = backbone_strain[k][j];
      }
    }

    // create upper triangular matrix and modified stress
    youngs[k].resize(number);
    std::vector<ADReal> G0_component(number);
    yield_stress[k].resize(number);
    std::vector<ADReal> modified_backbone_stress(number);
    InvA = A;
    for (std::size_t i = 1; i < number; i++)
    {
      for (std::size_t j = 0; j < number; j++)
      {
        InvA[i][j] = A[i][j] - A[i - 1][j];
        modified_backbone_stress[i] = backbone_stress[k][i] - backbone_stress[k][i - 1];
      }
    }
    modified_backbone_stress[0] = backbone_stress[k][0];

    // backward substitution
    G0_component[number - 1] = modified_backbone_stress[number - 1] / InvA[number - 1][number - 1];
    yield_stress[k][number - 1] = backbone_strain[k][number - 1];
    int i = number - 2;
    while (i >= 0)
    {
      G0_component[i] = 0.0;
      for (std::size_t j = i + 1; j < number; j++)
        G0_component[i] += InvA[i][j] * G0_component[j];

      G0_component[i] = (modified_backbone_stress[i] - G0_component[i]) / InvA[i][i];
      yield_stress[k][i] = backbone_strain[k][i];
      i = i - 1;
    }

    // scaling
    for (std::size_t i = 0; i < number; i++)
    {
      using std::sqrt;
      youngs[k][i] = G0_component[i] * 2.0 * (1.0 + poissons_ratio[k]);
      yield_stress[k][i] = yield_stress[k][i] * sqrt(3.0) / (2.0 * (1.0 + poissons_ratio[k]));
    }
  }
  for (std::size_t k = 0; k < layer_ids.size(); k++)
  {
    for (std::size_t j = 0; j < backbone_strain[k].size(); j++)
    {
      if (youngs[k][j] == 0)
        mooseError("In " + name + "Found two segments in the backbone curve with the same slope. ",
                   "Please recheck input backbone curves.");
    }
  }
}
