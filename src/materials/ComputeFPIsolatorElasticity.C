// MASTODON includes
#include "ComputeFPIsolatorElasticity.h"

// MOOSE includes
#include "MooseMesh.h"
#include "Assembly.h"
#include "NonlinearSystem.h"
#include "MooseVariable.h"
#include "MathUtils.h"

// libmesh includes
#include "libmesh/quadrature.h"
#include "libmesh/utility.h"

registerMooseObject("MastodonApp", ComputeFPIsolatorElasticity);

template <>
InputParameters
validParams<ComputeFPIsolatorElasticity>()
{
  InputParameters params = validParams<Material>();
  params.addClassDescription("Compute the forces and the stiffness matrix for a single concave "
                             "Friction Pendulum isolator element.");
  // Switches
  params.addRequiredParam<bool>(
      "pressure_dependent",
      "Switch for modeling friction dependence on the instantaneous pressure.");
  params.addRequiredParam<bool>(
      "temperature_dependent",
      "Switch for modeling friction dependence on the instantaneous temperature.");
  params.addRequiredParam<bool>(
      "velocity_dependent",
      "Switch for modeling friction dependence on the instantaneous sliding velocity.");
  // Material properties
  params.addRequiredRangeCheckedParam<Real>(
      "mu_ref", "mu_ref>0.0", "Reference co-efficient of friction.");
  params.addRequiredRangeCheckedParam<Real>("p_ref", "p_ref>0.0", "Reference axial pressure.");
  params.addRequiredRangeCheckedParam<Real>(
      "diffusivity", "diffusivity>0.0", "Thermal diffusivity of steel.");
  params.addRequiredRangeCheckedParam<Real>(
      "conductivity", "conductivity>0.0", "Thermal conductivity of steel.");
  params.addRequiredRangeCheckedParam<Real>("a", "a>0.0", "Rate parameter.");
  params.addRequiredRangeCheckedParam<Real>(
      "r_eff", "r_eff>0.0", "Effective radius of curvature of sliding surface.");
  params.addRequiredRangeCheckedParam<Real>(
      "r_contact", "r_contact>0.0", "Radius of contact area at sliding surface.");
  params.addRequiredRangeCheckedParam<Real>(
      "uy", "uy>0.0", "Yield displacement of the bearing in shear.");
  params.addRequiredRangeCheckedParam<Real>(
      "gamma", "gamma>0.0", "Gamma parameter of Newmark algorithm.");
  params.addRequiredRangeCheckedParam<Real>(
      "beta", "beta>0.0", "Beta parameter of Newmark algorithm.");
  params.addRequiredRangeCheckedParam<Real>("unit",
                                            "8.0>unit>0.0",
                                            "Tag for conversion in the pressure factor computation "
                                            "when different unit systems are used. Enter "
                                            "1.0 for N m s C;  "
                                            "2.0 for kN m s C;  "
                                            "3.0 for N mm s C;  "
                                            "4.0 for kN mm s C;  "
                                            "5.0 for lb in s C;  "
                                            "6.0 for kip in s C;  "
                                            "7.0 for lb ft s C;  "
                                            "8.0 for kip ft s C. ");
  params.addParam<Real>(
      "k_x",
      10e13,
      "Stiffness of the bearing in translation along x axis. Defaults to 10e13 (SI units)");
  params.addParam<Real>(
      "k_xx",
      10e13,
      "Stiffness of the bearing in rotation about x axis. Defaults to 10e13 (SI units) ");
  params.addParam<Real>(
      "k_yy",
      10e13,
      "Stiffness of the bearing in rotation about y axis. Defaults to 10e13 (SI units) ");
  params.addParam<Real>(
      "k_zz",
      10e13,
      "Stiffness of the bearing in rotation about z axis. Defaults to 10e13 (SI units) ");
  params.addParam<Real>(
      "tol", 1e-8, "Convergence tolerance to satisfy equilibrium of element. Defaults to 1e-8");
  params.addParam<Real>(
      "maxiter", 100, "Maximum number of iterations for equilibrium. Defaults to 100.");
  params.set<MooseEnum>("constant_on") = "ELEMENT";
  return params;
}

ComputeFPIsolatorElasticity::ComputeFPIsolatorElasticity(const InputParameters & parameters)
  : Material(parameters),
    _pressure_dependent(getParam<bool>("pressure_dependent")),
    _temperature_dependent(getParam<bool>("temperature_dependent")),
    _velocity_dependent(getParam<bool>("velocity_dependent")),
    _mu_ref(getParam<Real>("mu_ref")),
    _p_ref(getParam<Real>("p_ref")),
    _diffusivity(getParam<Real>("diffusivity")),
    _conductivity(getParam<Real>("conductivity")),
    _a(getParam<Real>("a")),
    _r_eff(getParam<Real>("r_eff")),
    _r_contact(getParam<Real>("r_contact")),
    _uy(getParam<Real>("uy")),
    _unit(getParam<Real>("unit")),
    _gamma(getParam<Real>("gamma")),
    _beta(getParam<Real>("beta")),
    _k_x(getParam<Real>("k_x")),
    _k_xx(getParam<Real>("k_xx")),
    _k_yy(getParam<Real>("k_yy")),
    _k_zz(getParam<Real>("k_zz")),
    _tol(getParam<Real>("tol")),
    _maxiter(getParam<Real>("maxiter")),
    _sD(0.5),
    _local_def(getMaterialPropertyByName<ColumnMajorMatrix>("local_deformations")),
    _basic_def(getMaterialPropertyByName<ColumnMajorMatrix>("deformations")),
    _basic_def_old(getMaterialPropertyByName<ColumnMajorMatrix>("old_deformations")),
    _basic_vel(getMaterialPropertyByName<ColumnMajorMatrix>("deformation_rates")),
    _basic_vel_old(getMaterialPropertyByName<ColumnMajorMatrix>("old_deformation_rates")),
    _Fb(declareProperty<ColumnMajorMatrix>("basic_forces")),
    _Fl(declareProperty<ColumnMajorMatrix>("local_forces")),
    _Fg(declareProperty<ColumnMajorMatrix>("global_forces")),
    _Kb(declareProperty<ColumnMajorMatrix>("basic_stiffness_matrix")),
    _Kl(declareProperty<ColumnMajorMatrix>("local_stiffness_matrix")),
    _Kg(declareProperty<ColumnMajorMatrix>("global_stiffness_matrix")),
    _total_gl(getMaterialPropertyByName<ColumnMajorMatrix>("total_global_to_local_transformation")),
    _total_lb(getMaterialPropertyByName<ColumnMajorMatrix>("total_local_to_basic_transformation")),
    _length(getMaterialPropertyByName<Real>("initial_isolator_length")),
    _pi(libMesh::pi),
    _ubPlastic(declareProperty<RealVectorValue>("plastic displacements in basic system")),
    _ubPlastic_old(getMaterialPropertyOld<RealVectorValue>("plastic displacements in basic system"))

{
  // Calculation of lateral stiffness of elastic component
  _mass_slider = _p_ref * _pi * _r_contact * _r_contact / 9.81; // Mass of the slider
  _k0 = _mass_slider * 9.81 * _mu_ref / _uy; // Initial elastic stiffness in lateral direction

  // Other parameters
  _kpF = 1;
  _ktF = 1;
  _kvF = 1;
  _temperature_center = 0;
  _heatflux_center = 0;
  _mu_adj = _mu_ref;
  _disp_currentstep = 0;
  _vel_currentstep = 0;
  _vec_time.push_back(0);
  _vec_heatflux.push_back(0);
}

void
ComputeFPIsolatorElasticity::initQpStatefulProperties()
{
  _ubPlastic[_qp].zero();
}

void
ComputeFPIsolatorElasticity::computeQpProperties()
{
  initializeFPIsolator();

  // Computing shear forces and stiffness terms
  computeShear();

  // Compute forces in other directions
  _Fb[_qp](0) = _Kb[_qp](0, 0) * _basic_def[_qp](0); // translation in basic x direction
  _Fb[_qp](3) = _Kb[_qp](3, 3) * _basic_def[_qp](3); // rotation along basic x direction
  _Fb[_qp](4) = _Kb[_qp](4, 4) * _basic_def[_qp](4); // rotation along basic y direction
  _Fb[_qp](5) = _Kb[_qp](5, 5) * _basic_def[_qp](5); // rotation along basic z direction

  // Finalize forces and stiffness matrix and convert them into global co-ordinate system
  finalize();
}

void
ComputeFPIsolatorElasticity::initializeFPIsolator()
{
  // Initialize stiffness matrices
  _Kb[_qp].reshape(6, 6);
  _Kb[_qp].identity();
  _Kb[_qp](0, 0) = _k_x;  // axial stiffness along x axis
  _Kb[_qp](1, 1) = _k0;   // elastic lateral stiffness along y axis
  _Kb[_qp](2, 2) = _k0;   // elastic lateral stiffness along z axis
  _Kb[_qp](3, 3) = _k_xx; // torsional stiffness about x axis
  _Kb[_qp](4, 4) = _k_yy; // rotational stiffness about y axis
  _Kb[_qp](5, 5) = _k_zz; // rotational stiffness about z axis

  // Initialize forces
  _Fb[_qp].reshape(6, 1); // forces in the basic system
  _Fb[_qp].zero();
  _Fl[_qp].reshape(12, 1); // forces in local system (6+6 = 12dofs)
  _Fl[_qp].zero();
  _Fg[_qp] = _Fl[_qp]; // forces in global system

  _Fb[_qp] = _Kb[_qp] * _basic_def[_qp];
}

void
ComputeFPIsolatorElasticity::computeShear()
{
  if (_t != _vec_time[_vec_time.size() - 1])
  {
    // Update last element of time vector with current analysis time
    _vec_time.push_back(_t);
    _vec_heatflux.push_back(0);
  }

  // Compute resultant displacement in horizontal direction
  Real resultantU = sqrt(_basic_def[_qp](1, 0) * _basic_def[_qp](1, 0) +
                         _basic_def[_qp](2, 0) * _basic_def[_qp](2, 0));

  // Calculate radii in basic y- and z- direction
  Real r_y = sqrt(pow(_r_eff, 2) - pow(_basic_def[_qp](1, 0), 2));
  Real r_z = sqrt(pow(_r_eff, 2) - pow(_basic_def[_qp](2, 0), 2));

  // Calculate velocities based on current deformations according to Newmark formulation
  Real vel1 = _basic_vel_old[_qp](1, 0) +
              (_gamma / _beta) * (((_basic_def[_qp](1, 0) - _basic_def_old[_qp](1, 0)) / _dt) -
                                  (_basic_vel_old[_qp](1, 0)));
  Real vel2 = _basic_vel_old[_qp](2, 0) +
              (_gamma / _beta) * (((_basic_def[_qp](2, 0) - _basic_def_old[_qp](2, 0)) / _dt) -
                                  (_basic_vel_old[_qp](2, 0)));

  // Calculate absolute velocity
  Real velAbs =
      sqrt(pow(vel1 / r_y * _basic_def[_qp](1, 0) + vel2 / r_z * _basic_def[_qp](2, 0), 2) +
           pow(vel1, 2) + pow(vel2, 2));

  // Check for uplift
  Real is_uplift = 0.0;
  Real kFactUplift = 1.0E-6;
  if (_Fb[_qp](0, 0) >= 0.0)
  {
    _Fb[_qp](0, 0) = -0.0001 * _Fb[_qp](0, 0);
    is_uplift = 1.0;
  }

  // Unit conversion for pressure to be used in the pressure factor computation
  Real p_Unit_Convert; // To convert to MPa
  if (_unit == 1)
  {
    p_Unit_Convert = 0.000001;
  }
  if (_unit == 2)
  {
    p_Unit_Convert = 0.001;
  }
  if (_unit == 3)
  {
    p_Unit_Convert = 1.0;
  }
  if (_unit == 4)
  {
    p_Unit_Convert = 1000.0;
  }
  if (_unit == 5)
  {
    p_Unit_Convert = 0.006894;
  }
  if (_unit == 6)
  {
    p_Unit_Convert = 6.894;
  }
  if (_unit == 7)
  {
    p_Unit_Convert = 0.00004788;
  }
  if (_unit == 8)
  {
    p_Unit_Convert = 0.04788;
  }

  // Calculate normal force
  Real N = -_Fb[_qp](0, 0);

  // Calculate instantaneous pressure
  Real Inst_Pressure = fabs(N) / (_pi * _r_contact * _r_contact);

  // Calculate displacement increment in the current step
  _disp_currentstep = sqrt(pow((_basic_def[_qp](1, 0) - _basic_def_old[_qp](1, 0)), 2) +
                           pow((_basic_def[_qp](2, 0) - _basic_def_old[_qp](2, 0)), 2));

  // Calculate sliding velocity
  _vel_currentstep = _disp_currentstep / (_vec_time[_t_step] - _vec_time[_t_step - 1]);

  if (velAbs > 0.0)
    _vel_currentstep = velAbs;

  // Calculate pressure factor
  if (_pressure_dependent)
    _kpF = pow(0.7, ((Inst_Pressure - _p_ref) * p_Unit_Convert / 50.0));

  // Calculate temperature factor
  if (_temperature_dependent)
  {
    // Calculate flux
    if (resultantU < (_r_contact * sqrt(_pi / 4)))
      _vec_heatflux[_t_step] =
          _mu_adj * (fabs(N) / (_pi * _r_contact * _r_contact)) * _vel_currentstep;

    else
      _vec_heatflux[_t_step] = 0.0;

    _heatflux_center = _vec_heatflux[_t_step];

    // Temperature at the sliding surface
    Real temperature_change = 0.0;

    // Evaluate the temperature integral
    if (_t_step >= 1 && resultantU > 0.0)
    {
      Real dt_analysis = 0.000001;
      Real tau = 0.000001;

      for (int jTemp = 1; jTemp <= _t_step; jTemp++)
      {
        dt_analysis = _vec_time[_t_step] - _vec_time[_t_step - 1];
        tau = _vec_time[jTemp];

        temperature_change =
            temperature_change +
            sqrt(_diffusivity / _pi) * _vec_heatflux[_t_step - jTemp] * dt_analysis /
                (_conductivity * sqrt(tau)); // Solving the integral for temperature increment
      }

      _temperature_center =
          20.0 + temperature_change; // Temperature at the center of the sliding surface

      _ktF =
          0.789 * (pow(0.7, (_temperature_center / 50.0)) + 0.4); // Update the temperature factor
    }
  }

  // Calculate velocity factor
  if (_velocity_dependent)
    _kvF = 1 - 0.5 * exp(-_a * _vel_currentstep);

  // Calculate adjusted co-efficient of friction
  _mu_adj = _mu_ref * _kpF * _ktF * _kvF;

  // Calculate shear forces and stiffness in basic y- and z-direction
  int iter = 0;
  _qbOld.zero();

  do
  {
    // Save shear forces of last iteration
    _qbOld(0) = _Fb[_qp](1, 0);
    _qbOld(1) = _Fb[_qp](2, 0);

    // Yield force
    Real qYield = _mu_adj * N;

    // Calculate stiffness of elastic components
    Real k2y = N / r_y;
    Real k2z = N / r_z;

    // Calculate initial stiffness of hysteretic components
    Real k0y = _k0 - k2y;
    Real k0z = _k0 - k2z;

    // Account for uplift
    kFactUplift = 1.0E-6;
    if (fabs(is_uplift - 1.0) <= 0.00001)
    {
      k0y = kFactUplift * k0y;
      k0z = kFactUplift * k0z;
    }

    // Trial shear forces of hysteretic component
    _qTrial.zero();
    _qTrial(0) = k0y * (_basic_def[_qp](1, 0) - _ubPlastic_old[_qp](0));
    _qTrial(1) = k0z * (_basic_def[_qp](2, 0) - _ubPlastic_old[_qp](1));

    // Compute yield criterion of hysteretic component
    Real qTrialNorm = _qTrial.norm();
    Real Y = qTrialNorm - qYield;

    // Elastic step -> no updates required
    if (Y <= 0.0)
    {
      // Set shear forces
      _Fb[_qp](1, 0) = _qTrial(0) - N * _local_def[_qp](5, 0) + k2y * _basic_def[_qp](1, 0);
      _Fb[_qp](2, 0) = _qTrial(1) + N * _local_def[_qp](4, 0) + k2z * _basic_def[_qp](2, 0);

      // Set tangent stiffnesses
      _Kb[_qp](1, 1) = _Kb[_qp](2, 2) = _k0;
      _Kb[_qp](1, 2) = _Kb[_qp](2, 1) = 0.0;

      // Account for uplift
      if (fabs(is_uplift - 1.0) <= 0.00001)
      {
        _Kb[_qp](1, 1) = kFactUplift * _k0;
        _Kb[_qp](2, 2) = kFactUplift * _k0;
      }

      _ubPlastic[_qp](0) = _ubPlastic_old[_qp](0);
      _ubPlastic[_qp](1) = _ubPlastic_old[_qp](1);
    }

    // Plastic step -> return mapping
    else
    {
      // Compute consistency parameters
      Real dGammaY = Y / k0y;
      Real dGammaZ = Y / k0z;

      // Update plastic displacements
      _ubPlastic[_qp](0) = _ubPlastic_old[_qp](0) + dGammaY * _qTrial(0) / qTrialNorm;
      _ubPlastic[_qp](1) = _ubPlastic_old[_qp](1) + dGammaZ * _qTrial(1) / qTrialNorm;

      // Set shear forces
      _Fb[_qp](1, 0) = qYield * _qTrial(0) / qTrialNorm - N * _local_def[_qp](5, 0) +
                       k2y * _basic_def[_qp](1, 0);
      _Fb[_qp](2, 0) = qYield * _qTrial(1) / qTrialNorm + N * _local_def[_qp](4, 0) +
                       k2z * _basic_def[_qp](2, 0);

      // Account for uplift
      Real k0Plastic = _k0;

      if (fabs(is_uplift - 1.0) <= 0.00001)
        k0Plastic = kFactUplift * _k0;

      // Set tangent stiffness
      Real D = pow(qTrialNorm, 3);

      _Kb[_qp](1, 1) = k0Plastic * (1.0 - (qYield * _qTrial(1) * _qTrial(1) / D)) + k2y;
      _Kb[_qp](1, 2) = -qYield * k0Plastic * _qTrial(0) * _qTrial(1) / D;
      _Kb[_qp](2, 1) = _Kb[_qp](1, 2);
      _Kb[_qp](2, 2) = k0Plastic * (1.0 - (qYield * _qTrial(0) * _qTrial(0) / D)) + k2z;
    }

    iter++;

  } while (sqrt(pow(_Fb[_qp](1, 0) - _qbOld(0), 2) + pow(_Fb[_qp](2, 0) - _qbOld(1), 2) >= _tol) &&
           (iter <= _maxiter));

  // Issue warning if iteration did not converge
  if (iter >= _maxiter)
  {
    mooseError("Error: ComputeFPIsolatorElasticity::computeshearforce() - did not find the shear "
               "force after ",
               iter,
               " iterations and norm: ",
               sqrt(pow(_Fb[_qp](1, 0) - _qbOld(0), 2) + pow(_Fb[_qp](2, 0) - _qbOld(1), 2)),
               ".\n");
  }
}

void
ComputeFPIsolatorElasticity::addPDeltaEffects()
{
  // Add P-Delta moment stiffness terms
  Real Ls = (1 - _sD) * _length[_qp];

  _Kl[_qp](5, 1) -= _Fb[_qp](0, 0);
  _Kl[_qp](5, 7) += _Fb[_qp](0, 0);
  _Kl[_qp](5, 11) -= _Fb[_qp](0, 0) * Ls;
  _Kl[_qp](11, 11) -= _Fb[_qp](0, 0) * Ls;
  _Kl[_qp](4, 2) += _Fb[_qp](0, 0);
  _Kl[_qp](4, 8) -= _Fb[_qp](0, 0);
  _Kl[_qp](4, 10) -= _Fb[_qp](0, 0) * Ls;
  _Kl[_qp](10, 10) -= _Fb[_qp](0, 0) * Ls;

  // Add V-Delta torsion stiffness terms
  _Kl[_qp](3, 1) += _Fb[_qp](2, 2);
  _Kl[_qp](3, 2) -= _Fb[_qp](1, 1);
  _Kl[_qp](3, 7) -= _Fb[_qp](2, 2);
  _Kl[_qp](3, 8) += _Fb[_qp](1, 1);
  _Kl[_qp](3, 10) += _Fb[_qp](1, 1) * Ls;
  _Kl[_qp](3, 11) += _Fb[_qp](2, 2) * Ls;
  _Kl[_qp](9, 10) -= _Fb[_qp](1, 1) * Ls;
  _Kl[_qp](9, 11) -= _Fb[_qp](2, 2) * Ls;
}

void
ComputeFPIsolatorElasticity::finalize()
{
  // Convert forces from basic to local to global coordinate system
  _Fl[_qp] = _total_lb[_qp].transpose() * _Fb[_qp]; // local forces
  _Fg[_qp] = _total_gl[_qp].transpose() * _Fl[_qp]; // global forces

  // Convert stiffness matrix from basic to local coordinate system
  _Kl[_qp] = _total_lb[_qp].transpose() * _Kb[_qp] * _total_lb[_qp];

  // add P-∆ and V-∆ effects to local stiffness matrix
  addPDeltaEffects();

  // Converting stiffness matrix from local to global coordinate system
  _Kg[_qp] = _total_gl[_qp].transpose() * _Kl[_qp] * _total_gl[_qp];
}
