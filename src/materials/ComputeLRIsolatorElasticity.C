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

// MASTODON includes
#include "ComputeLRIsolatorElasticity.h"

// MOOSE includes
#include "MooseMesh.h"
#include "Assembly.h"
#include "NonlinearSystem.h"
#include "MooseVariable.h"
#include "MathUtils.h"

// libmesh includes
#include "libmesh/quadrature.h"
#include "libmesh/utility.h"

registerMooseObject("MastodonApp", ComputeLRIsolatorElasticity);

template <>
InputParameters
validParams<ComputeLRIsolatorElasticity>()
{
  InputParameters params = validParams<Material>();
  params.addClassDescription(
      "Compute the forces and the stiffness matrix for an LR isolator element.");
  // Switches
  params.addParam<bool>("cavitation", false, "Switch for modeling cavitation and post-cavitation.");
  params.addParam<bool>("buckling_load_variation",
                        false,
                        "Switch for modeling buckling load variation during the analysis.");
  params.addParam<bool>(
      "horizontal_stiffness_variation",
      false,
      "Switch for modeling variation of horizontal stiffness during the analysis.");
  params.addParam<bool>("vertical_stiffness_variation",
                        false,
                        "Switch for modeling variation of vertical stiffness during the analysis.");
  params.addParam<bool>(
      "strength_degradation",
      false,
      "Switch for modeling strength degradation due to lead core heating."); // Strength degradation
                                                                             // due to heating
  // Material properties
  params.addRequiredParam<Real>("fy", "Yield stress of the bearing.");
  params.addRequiredParam<Real>("alpha", "Yield displacement of the bearing.");
  params.addRequiredParam<Real>("G_rubber", "Shear modulus of rubber.");
  params.addRequiredParam<Real>("K_rubber", "Bulk modulus of rubber.");
  params.addRequiredParam<Real>("D1", "Diameter of lead core.");
  params.addRequiredParam<Real>("D2", "Outer diameter of the bearing.");
  params.addRequiredParam<Real>("ts", "Thickness of a single steel shim.");
  params.addRequiredParam<Real>("tr", "Thickness of a single rubber layer.");
  params.addRequiredParam<Real>("n", "Number of rubber layers.");
  params.addRequiredParam<Real>("tc", "Thickness of the rubber cover of the bearing.");
  params.addRequiredParam<Real>("gamma", "Gamma parameter of Newmark algorithm.");
  params.addRequiredParam<Real>("beta", "Beta parameter of Newmark algorithm.");
  params.addParam<Real>("rho_lead", 11200.0, "Density of lead. Defaults to 11200 kg/m3.");
  params.addParam<Real>(
      "c_lead", 130.0, "Specific heat capacity of lead. Defaults to 130.0 N-m/kg oC.");
  params.addParam<Real>(
      "k_steel", 50.0, "Thermal conductivity of steel. Defaults to 50.0 W/(m-oC).");
  params.addParam<Real>(
      "a_steel", 1.41e-05, "Thermal diffusivity of steel. Defaults to 1.41e-05 m2/s.");
  params.addParam<Real>("kc", 20.0, "Cavitation parameter.");
  params.addParam<Real>("phi_m", 0.75, "Damage index.");
  params.addParam<Real>("ac", 1.0, "Strength degradation parameter.");
  params.addParam<Real>("cd", 0.0, "Viscous damping parameter.");
  params.set<MooseEnum>("constant_on") = "ELEMENT"; // _qp = 0
  return params;
}

ComputeLRIsolatorElasticity::ComputeLRIsolatorElasticity(const InputParameters & parameters)
  : Material(parameters),
    _cavitation(getParam<bool>("cavitation")),
    _buckling_load_variation(getParam<bool>("buckling_load_variation")),
    _horizontal_stiffness_variation(getParam<bool>("horizontal_stiffness_variation")),
    _vertical_stiffness_variation(getParam<bool>("vertical_stiffness_variation")),
    _strength_degradation(getParam<bool>("strength_degradation")),
    _fy(getParam<Real>("fy")),
    _alpha(getParam<Real>("alpha")),
    _Gr(getParam<Real>("G_rubber")),
    _Kr(getParam<Real>("K_rubber")),
    _d1(getParam<Real>("D1")),
    _d2(getParam<Real>("D2")),
    _ts(getParam<Real>("ts")),
    _tr(getParam<Real>("tr")),
    _n(getParam<Real>("n")),
    _tc(getParam<Real>("tc")),
    _gamma(getParam<Real>("gamma")),
    _beta(getParam<Real>("beta")),
    _rhoL(getParam<Real>("rho_lead")), // qL in opensees
    _cL(getParam<Real>("c_lead")),
    _kS(getParam<Real>("k_steel")),
    _aS(getParam<Real>("a_steel")),
    _kc(getParam<Real>("kc")),
    _phi_m(getParam<Real>("phi_m")),
    _ac(getParam<Real>("ac")),
    _cd(getParam<Real>("cd")),
    _sD(0.5),
    _basic_def(getMaterialPropertyByName<ColumnMajorMatrix>("deformations")),
    _basic_def_old(getMaterialPropertyByName<ColumnMajorMatrix>("old_deformations")),
    _basic_vel(getMaterialPropertyByName<ColumnMajorMatrix>("deformation_rates")),
    _basic_vel_old(getMaterialPropertyByName<ColumnMajorMatrix>("old_deformation_rates")),
    _Fb(declareProperty<ColumnMajorMatrix>("basic_forces")),
    _Fb_old(getMaterialPropertyOld<ColumnMajorMatrix>("basic_forces")),
    _Fl(declareProperty<ColumnMajorMatrix>("local_forces")),
    _Fg(declareProperty<ColumnMajorMatrix>("global_forces")),
    _Kb(declareProperty<ColumnMajorMatrix>("basic_stiffness_matrix")),
    _Kl(declareProperty<ColumnMajorMatrix>("local_stiffness_matrix")),
    _Kg(declareProperty<ColumnMajorMatrix>("global_stiffness_matrix")),
    _total_gl(getMaterialPropertyByName<ColumnMajorMatrix>("total_global_to_local_transformation")),
    _total_lb(getMaterialPropertyByName<ColumnMajorMatrix>("total_local_to_basic_transformation")),
    _length(getMaterialPropertyByName<Real>("initial_isolator_length")),
    _pi(libMesh::pi),
    _TL_trial(0.0),
    _TLC(0.0),
    _z(declareProperty<ColumnMajorMatrix>("hysteresis_parameter")),
    _z_old(getMaterialPropertyOld<ColumnMajorMatrix>("hysteresis_parameter")),
    _umax(declareProperty<ColumnMajorMatrix>("max_tensile_deformation")),
    _umax_old(getMaterialPropertyOld<ColumnMajorMatrix>("max_tensile_deformation")),
    _ucn(declareProperty<ColumnMajorMatrix>("initial_cavitation_deformation")),
    _ucn_old(getMaterialPropertyOld<ColumnMajorMatrix>("initial_cavitation_deformation")),
    _Fcrmin(declareProperty<ColumnMajorMatrix>("initial_buckling_load")),
    _Fcrmin_old(getMaterialPropertyOld<ColumnMajorMatrix>("initial_buckling_load"))

{
  // Bearing material and geometric parameters
  _A = (_pi / 4.0) * ((_d2 + _tc) * (_d2 + _tc) - _d1 * _d1); // Bonded rubber area of bearing
  Real S = (_d2 * _d2 - _d1 * _d1) / (4 * _d2 * _tr); // Shape factor for case with lead core
  _Tr = _n * _tr;                                     // height of rubber in the bearing
  _h = _Tr + (_n - 1) * _ts;                          // height of rubber + shims
  Real F;                                             // Dimension modification factor
  if (_d1 == 0)                                       // If no lead core, i.e., elastomeric bearing
    F = 1.0;
  else
  {
    Real r = _d2 / _d1; // Outer to inner diameter ratio
    F = (r * r + 1) / ((r - 1) * (r - 1)) +
        (1 + r) / ((1 - r) * log(r)); // Dimension modification factor
  }
  Real Ec = 1.0 / ((1.0 / (6 * _Gr * S * S * F)) +
                   (4.0 / 3.0) * (1.0 / _Kr)); // Compressive modulus of elastomeric bearing
  Real I = (_pi / 64.0) * (pow((_d2 + _tc), 4) - pow(_d1, 4)); // Moment of inertia of bearing
  _rg = sqrt(I / _A);                                          // Radius of gyration

  // Bearing shear behavior parameters
  if (_alpha < 0.0 || _alpha >= 1.0)
    mooseError("In ComputeLRIsolatorElasticity block, ",
               name(),
               ". Parameter, '_alpha' should be >= 0.0 and < 1.0.");
  _qYield0 = _fy * (1 - _alpha);
  _qYield = _qYield0;               // This yield stress changes with time
  _ke = _Gr * _A / _Tr;             // Stiffness of elastic component (due to rubber)
  _k0 = (1.0 / _alpha - 1.0) * _ke; // Initial stiffness of hysteretic component (due to lead)

  // Axial parameters: compression
  Real Erot = Ec / 3.0;                        // Rotation modulus of bearing
  Real As = _A * _h / _Tr;                     // Adjusted shear area of bearing
  Real Is = I * _h / _Tr;                      // Adjusted moment of intertia of bearing
  Real Pe = _pi * _pi * Erot * Is / (_h * _h); // Euler buckling load of bearing
  _kv0 = _A * Ec / _Tr;        // Pre-cavitation tensile stiffness at zero lateral displacement
  _kv = _kv0;                  // Pre-cavitation stiffness initialized to that at zero displacement
  _Fcr = -sqrt(Pe * _Gr * As); // Critical buckling load in compression
  _Fcrn = _Fcr;                // Current value of critical buckling load
  _ucr = _Fcr / _kv0;          // Initial value of critical buckling deformation
  _ucrn = _ucr;                // Current value of critical buckling deformation

  // Axial parameters: tension
  _Fc = 3.0 * _Gr * _A; // Force that initiates cavitation
  _Fcn = _Fc;           // Initial value of cavitation force (will be updated each time step)
  _uc = _Fc / _kv0;     // Deformation at which cavitation is first initiated
  _Fmax = _Fc;          // Initial value of maximum tensile force (will be updated each time step)

  _dzdu.reshape(2, 2);
  _dzdu.zero();
  _ubC.zero();
  _tC = _t;
}

void
ComputeLRIsolatorElasticity::initQpStatefulProperties()
{
  _z[_qp].reshape(2, 1);
  _z[_qp].zero();
  _umax[_qp].reshape(1, 1);
  _umax[_qp](0) = _uc;
  _ucn[_qp].reshape(1, 1);
  _ucn[_qp](0) = _uc;
  _Fcrmin[_qp].reshape(1, 1);
  _Fcrmin[_qp](0) = _Fcr;
}

void
ComputeLRIsolatorElasticity::initializeLRIsolator()
{
  Real I = (_pi / 64.0) * (pow((_d2 + _tc), 4) - pow(_d1, 4)); // Moment of inertia of bearing
  Real Is = I * _h / _Tr; // Adjusted moment of intertia of bearing
  Real Er = 3.0 * _Gr;    // Elastic modulus of rubber (assuming nu = 0.5)

  // Initializing stiffness matrices
  _Kb[_qp].reshape(6, 6);
  _Kb[_qp].identity();
  _Kb[_qp](0, 0) = _kv0;
  _Kb[_qp](1, 1) = _k0 + _ke;
  _Kb[_qp](2, 2) = _k0 + _ke;
  _Kb[_qp](3, 3) = _Gr * (2 * Is) / _h; // torsional stiffness
  _Kb[_qp](4, 4) = Er * Is / _h;        // rotational stiffness
  _Kb[_qp](5, 5) = Er * Is / _h;        // rotational stiffness

  // Initializing forces
  _Fb[_qp].reshape(6, 1); // forces in the basic system
  _Fb[_qp].zero();
  _Fl[_qp].reshape(12, 1); // forces in local system (6+6 = 12dofs)
  _Fl[_qp].zero();
  _Fg[_qp] = _Fl[_qp]; // forces in global system

  _Fb[_qp] = _Kb[_qp] * _basic_def[_qp];
}

void
ComputeLRIsolatorElasticity::computeQpProperties()
{
  initializeLRIsolator();

  // Compute axial forces and stiffness terms
  computeAxial();

  // Computing shear forces and stiffness terms
  computeShear();

  // Compute rotational components
  // basic x direction
  _Fb[_qp](3) = _Kb[_qp](3, 3) * _basic_def[_qp](3);
  // basic y direction
  _Fb[_qp](4) = _Kb[_qp](4, 4) * _basic_def[_qp](4);
  // basic z direction
  _Fb[_qp](5) = _Kb[_qp](5, 5) * _basic_def[_qp](5);

  // Finalize forces and stiffness matrix and convert them into global co-ordinate system
  finalize();
}

void
ComputeLRIsolatorElasticity::computeAxial()
{
  Real uh = sqrt(_basic_def_old[_qp](1, 0) * _basic_def_old[_qp](1, 0) +
                 _basic_def_old[_qp](2, 0) * _basic_def_old[_qp](2, 0));

  // Updating Vertical stiffness based on previous step horizontal displacements
  if (_vertical_stiffness_variation)
  {
    _kv = _kv0 * (1.0 / (1.0 + (3.0 / (_pi * _pi)) * (uh / _rg) * (uh / _rg)));
    if (uh > 0.0)
      _uc = _Fc / _kv;
  }

  // Updating current value of critical Buckling load bassed on previous step horizontal
  // displacements
  if (_buckling_load_variation)
  {
    if (uh / _d2 > 1.0)
      mooseError("Horizontal displacement is greater than the outer diameter of the isolator!\n");
    Real delta = 2.0 * acos(uh / _d2); // delta is undefined for uh/_d2 > 1.0
    Real _Ar = ((_d2 + _tc) * (_d2 + _tc) - _d1 * _d1) / 4.0 *
               (delta - sin(delta)); // _Ar does not include lead core
    if (_Ar / _A < 0.2 || uh / _d2 >= 1.0)
      _Fcrn = 0.2 * _Fcr;
    else
      _Fcrn = _Fcr * _Ar / _A;
    if (_Fcrn > _Fcrmin_old[_qp](0))
      _Fcrmin[_qp](0) = _Fcrn;
  }
  _ucrn = _Fcrn / _kv;

  // Calculating force and stiffness in basic x-direction
  // Post buckling phase
  if (_basic_def[_qp](0) <= _ucrn)
  {
    if (_buckling_load_variation)
    {
      _Kb[_qp](0, 0) = _kv / 1000.0;
      _Fb[_qp](0, 0) = _Fcrmin[_qp](0) + _Kb[_qp](0, 0) * (_basic_def[_qp](0) - _ucrn);
    }
    else
    {
      _Kb[_qp](0, 0) = _kv;
      _Fb[_qp](0, 0) = _Kb[_qp](0, 0) * _basic_def[_qp](0, 0);
    }
  }

  if (_basic_def[_qp](0, 0) > _ucrn)
  {
    if (_cavitation) // simulating cavitation effects
    {
      // Linear Elastic Phase
      if (_basic_def[_qp](0, 0) <= _ucn_old[_qp](0))
      {
        _Kb[_qp](0, 0) = _kv;
        _Fb[_qp](0, 0) = _Kb[_qp](0, 0) * _basic_def[_qp](0, 0);
        _umax[_qp](0) = _umax_old[_qp](0);
        _Fmax = _Fc * (1.0 + (1.0 / (_Tr * _kc)) * (1.0 - exp(-_kc * (_umax[_qp](0) - _uc))));
        _Fcn = _Fc * (1.0 - _phi_m * (1.0 - exp(-_ac * (_umax[_qp](0) - _uc) / _uc)));
        _ucn[_qp](0) = _Fcn / _kv;
      }
      // Cavitation un-loading phase
      else if (_basic_def[_qp](0, 0) <= _umax_old[_qp](0))
      {
        _umax[_qp](0) = _umax_old[_qp](0);
        _Fmax = _Fc * (1.0 + (1.0 / (_Tr * _kc)) * (1.0 - exp(-_kc * (_umax[_qp](0) - _uc))));
        _Fcn = _Fc * (1.0 - _phi_m * (1.0 - exp(-_ac * (_umax[_qp](0) - _uc) / _uc)));
        _ucn[_qp](0) = _Fcn / _kv; // cavitation deformation
        _Kb[_qp](0, 0) = (_Fmax - _Fcn) / (_umax[_qp](0) - _ucn[_qp](0));
        _Fb[_qp](0, 0) = _Fcn + (_Fmax - _Fcn) / (_umax[_qp](0) - _ucn[_qp](0)) *
                                    (_basic_def[_qp](0, 0) - _ucn[_qp](0));
      }
      // Cavitation reloading phase
      else
      {
        _Kb[_qp](0, 0) = (_Fc / _Tr) * exp(-_kc * (_basic_def[_qp](0, 0) - _uc));
        _Fb[_qp](0, 0) =
            _Fc * (1.0 + (1.0 / (_Tr * _kc)) * (1.0 - exp(-_kc * (_basic_def[_qp](0, 0) - _uc))));

        // Updating umax, Fmax, Fcn, ucn
        _umax[_qp](0) = _basic_def[_qp](0, 0);
        _Fmax = _Fc * (1.0 + (1.0 / (_Tr * _kc)) * (1.0 - exp(-_kc * (_umax[_qp](0) - _uc))));
        _Fcn = _Fc * (1.0 - _phi_m * (1.0 - exp(-_ac * (_umax[_qp](0) - _uc) / _uc)));
        _ucn[_qp](0) = _Fcn / _kv;
      }
    }
    else // cavitation effects not simulated
    {
      _Kb[_qp](0, 0) = _kv;
      _Fb[_qp](0, 0) = _Kb[_qp](0, 0) * _basic_def[_qp](0, 0);
    }
  }
}

void
ComputeLRIsolatorElasticity::computeShear()
{
  // update horizontal stiffness based on axial load and critical buckling of previous step
  if (_horizontal_stiffness_variation)
  {
    _ke = (_Gr * _A / _Tr) * (1.0 - pow(_Fb_old[_qp](0) / _Fcrn, 2));

    if (_ke < 0)
    {
      _ke = 0.01 * (_Gr * _A / _Tr); // a fraction of _ke to avoid convergence issues
    }
  }

  // current temperature of the lead core
  Real vel1 = _basic_vel_old[_qp](1, 0) +
              (_gamma / _beta) * (((_basic_def[_qp](1, 0) - _basic_def_old[_qp](1, 0)) / _dt) -
                                  (_basic_vel_old[_qp](1, 0)));
  Real vel2 = _basic_vel_old[_qp](2, 0) +
              (_gamma / _beta) * (((_basic_def[_qp](2, 0) - _basic_def_old[_qp](2, 0)) / _dt) -
                                  (_basic_vel_old[_qp](2, 0)));
  Real vel = sqrt(vel1 * vel1 + vel2 * vel2);

  _TL_trial = calculateCurrentTemperature(_qYield, _TLC, vel);

  // calculating shear forces and stiffnesses in basic y and z directions
  // get displacement increments (trial-committed)

  ColumnMajorMatrix delta_ub = _basic_def[_qp] - _basic_def_old[_qp];

  if (std::sqrt(delta_ub(1) * delta_ub(1) + delta_ub(2) * delta_ub(2)) >= 0.0)
  {
    // yield displacement
    double uy = _qYield / _k0;

    // calculate hysteretic evolution parameter, z, using the Newton-Raphson method
    unsigned int iter = 0;
    unsigned int maxIter = 100;
    Real tol = 1E-9;
    Real beta = 0.1; // beta and gamma are as per Nagarajaiah(1991), which is opposite to from Park
                     // et al.(1986)
    Real gamma = 0.9;
    double tmp1, tmp2, tmp3;
    ColumnMajorMatrix f(2, 1), delta_z(2, 1), Df(2, 2);
    do
    {
      tmp1 = beta + gamma * MathUtils::sign(_z[_qp](0) * delta_ub(1));
      tmp2 = beta + gamma * MathUtils::sign(_z[_qp](1) * delta_ub(2));
      tmp3 = _z[_qp](0) * delta_ub(1) * tmp1 + _z[_qp](1) * delta_ub(2) * tmp2;

      // function and derivative
      f(0) = _z[_qp](0) - _z_old[_qp](0) - 1.0 / uy * (delta_ub(1) - _z[_qp](0) * tmp3);
      f(1) = _z[_qp](1) - _z_old[_qp](1) - 1.0 / uy * (delta_ub(2) - _z[_qp](1) * tmp3);

      Df(0, 0) = 1.0 + (1.0 / uy) *
                           (2 * _z[_qp](0) * delta_ub(1) * tmp1 + _z[_qp](1) * delta_ub(2) * tmp2);
      Df(1, 0) = (tmp1 / uy) * _z[_qp](1) * delta_ub(1);
      Df(0, 1) = (tmp2 / uy) * _z[_qp](0) * delta_ub(2);
      Df(1, 1) = 1.0 + (1.0 / uy) *
                           (_z[_qp](0) * delta_ub(1) * tmp1 + 2 * _z[_qp](1) * delta_ub(2) * tmp2);

      // issue warning if the diagonal elements of the derivative Df is zero
      if (MooseUtils::absoluteFuzzyLessEqual(Df(0, 0), 0.0) ||
          MooseUtils::absoluteFuzzyLessEqual(Df(1, 1), 0.0))
        mooseError("Error in ComputeLRIsolatorElasticity block, ",
                   name(),
                   ". Zero Jacobian in Newton-Raphson scheme while solving ",
                   "for the hysteretic evolution parameter, z.\n");
      // advance one step
      delta_z(0) =
          (f(0) * Df(1, 1) - f(1) * Df(0, 1)) / (Df(0, 0) * Df(1, 1) - Df(0, 1) * Df(1, 0));
      delta_z(1) =
          (f(0) * Df(1, 0) - f(1) * Df(0, 0)) / (Df(0, 1) * Df(1, 0) - Df(0, 0) * Df(1, 1));
      _z[_qp] = _z[_qp] - delta_z;
      iter++;
    } while ((delta_z.norm() >= tol) && (iter < maxIter));

    // Error if Newton-Raphson scheme did not converge
    if (iter >= maxIter)
      mooseError("Error in block, ",
                 name(),
                 ". Could not solve for hysteresis",
                 " evolution parameter, z, after ",
                 iter,
                 " iterations and",
                 " achieving a norm of ",
                 delta_z.norm(),
                 ".\n");

    // calculate derivative of hysteretic evolution parameter
    Real du1du2, du2du1;
    if (delta_ub(1) * delta_ub(2) == 0)
    {
      du1du2 = 0.0;
      du2du1 = 0.0;
    }
    else
    {
      du1du2 = delta_ub(1) / delta_ub(2);
      du2du1 = delta_ub(2) / delta_ub(1);
    }
    _dzdu(0, 0) =
        (1.0 / uy) * (1.0 - _z[_qp](0) * (_z[_qp](0) * tmp1 + _z[_qp](1) * tmp2 * du2du1));
    _dzdu(0, 1) =
        (1.0 / uy) * (du1du2 - _z[_qp](0) * (_z[_qp](0) * tmp1 * du1du2 + _z[_qp](1) * tmp2));
    _dzdu(1, 0) =
        (1.0 / uy) * (du2du1 - _z[_qp](1) * (_z[_qp](0) * tmp1 + _z[_qp](1) * tmp2 * du2du1));
    _dzdu(1, 1) =
        (1.0 / uy) * (1.0 - _z[_qp](1) * (_z[_qp](0) * tmp1 * du1du2 + _z[_qp](1) * tmp2));

    Real dt = _t - _tC;
    if (dt == 0)
      dt = _dt;

    // set shear force
    _Fb[_qp](1, 0) = _cd * vel1 + _qYield * _z[_qp](0) + _ke * _basic_def[_qp](1, 0);
    _Fb[_qp](2, 0) = _cd * vel2 + _qYield * _z[_qp](1) + _ke * _basic_def[_qp](2, 0);

    // set tangent stiffness
    _Kb[_qp](1, 1) = (_gamma / _beta) * _cd / _dt + _qYield * _dzdu(0, 0) + _ke;
    _Kb[_qp](1, 2) = _qYield * _dzdu(0, 1);
    _Kb[_qp](2, 1) = _qYield * _dzdu(1, 0);
    _Kb[_qp](2, 2) = (_gamma / _beta) * _cd / _dt + _qYield * _dzdu(1, 1) + _ke;

    if (_Kb[_qp](1, 2) * _Kb[_qp](2, 1) - _Kb[_qp](1, 1) * _Kb[_qp](2, 2) == 0)
      mooseError("Error in block, ",
                 name(),
                 ". Jacobian for isolator is zero due to off diagonal shear elements. Please check "
                 "again.\n");
  }
}

void
ComputeLRIsolatorElasticity::addPDeltaEffects()
{
  // add geometric stiffness to local stiffness
  _Kl[_qp](5, 1) -= 0.5 * _Fb[_qp](0, 0);
  _Kl[_qp](5, 7) += 0.5 * _Fb[_qp](0, 0);
  _Kl[_qp](11, 1) -= 0.5 * _Fb[_qp](0, 0);
  _Kl[_qp](11, 7) += 0.5 * _Fb[_qp](0, 0);
  _Kl[_qp](4, 2) += 0.5 * _Fb[_qp](0, 0);
  _Kl[_qp](4, 8) -= 0.5 * _Fb[_qp](0, 0);
  _Kl[_qp](10, 2) += 0.5 * _Fb[_qp](0, 0);
  _Kl[_qp](10, 8) -= 0.5 * _Fb[_qp](0, 0);
  _Kl[_qp](5, 5) += 0.5 * _Fb[_qp](0, 0) * _sD * _length[_qp];
  _Kl[_qp](11, 5) -= 0.5 * _Fb[_qp](0, 0) * _sD * _length[_qp];
  _Kl[_qp](4, 4) += 0.5 * _Fb[_qp](0, 0) * _sD * _length[_qp];
  _Kl[_qp](10, 4) -= 0.5 * _Fb[_qp](0, 0) * _sD * _length[_qp];
  _Kl[_qp](5, 11) -= 0.5 * _Fb[_qp](0, 0) * (1.0 - _sD) * _length[_qp];
  _Kl[_qp](11, 11) += 0.5 * _Fb[_qp](0, 0) * (1.0 - _sD) * _length[_qp];
  _Kl[_qp](4, 10) -= 0.5 * _Fb[_qp](0, 0) * (1.0 - _sD) * _length[_qp];
  _Kl[_qp](10, 10) += 0.5 * _Fb[_qp](0, 0) * (1.0 - _sD) * _length[_qp];
}

Real
ComputeLRIsolatorElasticity::calculateCurrentTemperature(Real _qYield, Real _TLC, Real vel)
{
  // lead core heating
  if (_t <= 0)
    return 0.0;

  if (_t < _tC)
    _tC = 0.0;
  Real a = _d1 / 2.0;
  Real dt = _t - _tC;
  // if (dt>1) dt = 0;
  Real a_lead = _pi * pow(a, 2);
  Real tau = (_aS * _t) / (pow(a, 2));
  Real F;
  if (tau < 0.6)
    F = 2.0 * sqrt(tau / _pi) -
        (tau / _pi) * (2.0 - (tau / 4.0) - pow(tau / 4.0, 2) - (15.0 / 4.0) * (pow(tau / 4.0, 3)));
  else
    F = 8.0 / (3.0 * _pi) - (1.0 / (2.0 * sqrt(_pi * tau))) *
                                (1.0 - (1.0 / (12.0 * tau)) + (1.0 / (6.0 * pow(4.0 * tau, 2))) -
                                 (1.0 / (12.0 * pow(4.0 * tau, 3))));
  Real deltaT1 =
      (dt / (_rhoL * _cL * _h)) *
      ((_qYield * vel) / a_lead -
       (_kS * _TLC / a) * (1.0 / F + 1.274 * ((_n - 1) * _ts / a) * pow(tau, -1.0 / 3.0)));
  if (deltaT1 <= 0.0)
    deltaT1 = 0.0;

  Real TL_trial1 = _TLC + deltaT1;
  Real tCurrent2 = _t + dt;
  tau = (_aS * tCurrent2) / (pow(a, 2));
  if (tau < 0.6)
    F = 2.0 * sqrt(tau / _pi) -
        (tau / _pi) * (2.0 - (tau / 4.0) - pow(tau / 4.0, 2) - (15.0 / 4.0) * (pow(tau / 4.0, 3)));
  else
    F = 8.0 / (3.0 * _pi) - (1.0 / (2.0 * sqrt(_pi * tau))) *
                                (1.0 - (1.0 / (12.0 * tau)) + (1.0 / (6.0 * pow(4.0 * tau, 2))) -
                                 (1.0 / (12.0 * pow(4.0 * tau, 3))));
  Real deltaT2 =
      (dt / (_rhoL * _cL * _h)) *
      ((_qYield * vel) / a_lead -
       (_kS * TL_trial1 / a) * (1.0 / F + 1.274 * ((_n - 1) * _ts / a) * pow(tau, -1.0 / 3.0)));
  if (deltaT2 <= 0.0)
    deltaT2 = 0.0;

  Real _TL_trial = _TLC + 0.5 * (deltaT1 + deltaT2);

  return _TL_trial;
}

void
ComputeLRIsolatorElasticity::finalize()

{
  // update lead core heating parameters
  _TLC = _TL_trial;
  _tC = _t;
  if (_strength_degradation)
    _qYield = _qYield0 * exp(-0.0069 * _TLC);

  // Converting forces from basic to local to global
  _Fl[_qp] = _total_lb[_qp].transpose() * _Fb[_qp]; // local forces
  _Fg[_qp] = _total_gl[_qp].transpose() * _Fl[_qp]; // global forces

  // Converting stiffness matrix from basic to local
  _Kl[_qp] = _total_lb[_qp].transpose() * _Kb[_qp] * _total_lb[_qp];

  // add P-∆ effects to local stiffness
  addPDeltaEffects();

  // Converting stiffness matrix from loacl to global
  _Kg[_qp] = _total_gl[_qp].transpose() * _Kl[_qp] * _total_gl[_qp];
}
