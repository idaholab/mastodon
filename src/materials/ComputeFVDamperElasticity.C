// MASTODON includes
#include "ComputeFVDamperElasticity.h"

// MOOSE includes
#include "MooseMesh.h"
#include "Assembly.h"
#include "NonlinearSystem.h"
#include "MooseVariable.h"
#include "ColumnMajorMatrix.h"
#include "MathUtils.h"

// libmesh includes
#include "libmesh/quadrature.h"
#include "libmesh/utility.h"

registerMooseObject("MastodonApp", ComputeFVDamperElasticity);

template <>
InputParameters
validParams<ComputeFVDamperElasticity>()
{
  InputParameters params = validParams<Material>();
  params.addClassDescription("Compute the deformations, forces and the stiffness"
                             "matrix corresponding to a two-node nonlinear fluid"
                             "viscous damper element.");
  params.addRequiredCoupledVar(
      "displacements",
      "The displacement variables appropriate for the simulation of geometry and coordinate system.");
  params.addRequiredParam<RealGradient>(
       "y_orientation",
       "Orientation of the y direction along "
       "with Ky is provided. This should be "
       "perpendicular to the axis of the Damper.");

  // Input parameters
  params.addRequiredRangeCheckedParam<Real>(
       "Cd", "Cd > 0.0", "Damping co-efficient.");
  params.addRequiredRangeCheckedParam<Real>(
       "alpha", "alpha > 0.0", "Velocity exponent of the damper.");
  params.addRequiredRangeCheckedParam<Real>(
       "k", "k > 0.0", "Axial stiffness of the damper assembly.");
  params.addRequiredRangeCheckedParam<Real>(
       "gamma", "gamma > 0.0", "Gamma parameter of Newmark algorithm.");
  params.addRequiredRangeCheckedParam<Real>(
       "beta", "beta > 0.0", "Beta parameter of Newmark algorithm.");
  params.addParam<Real>(
       "Rel_tol",
       1e-6,
       "Relative tolerance for error in adaptive algorithm");
  params.addParam<Real>(
       "Abs_tol",
       1e-6,
       "Absolute tolerance for error in adaptive algorithm");
  params.set<MooseEnum>("constant_on") = "ELEMENT";
  return params;
}

ComputeFVDamperElasticity::ComputeFVDamperElasticity(const InputParameters & parameters)
  : Material(parameters),
    _ndisp(coupledComponents("displacements")),
    _disp_num(3),
    _length(declareProperty<Real>("initial_Damper_length")),
    _Cd(getParam<Real>("Cd")),
    _alpha(getParam<Real>("alpha")),
    _k(getParam<Real>("k")),
    _gamma(getParam<Real>("gamma")),
    _beta(getParam<Real>("beta")),
    _Rel_tol(getParam<Real>("Rel_tol")),
    _Abs_tol(getParam<Real>("Abs_tol")),
    _basic_def(declareProperty<Real>("basic_deformation")),
    _basic_def_old(getMaterialPropertyOld<Real>("basic_deformation")),
    _vel(declareProperty<Real>("velocity")),
    _vel_old(getMaterialPropertyOld<Real>("velocity")),
    _Fb(declareProperty<Real>("basic_force")),
    _Fb_old(getMaterialPropertyOld<Real>("basic_force")),
    _Fl(declareProperty<ColumnMajorMatrix>("local_forces")),
    _Fg(declareProperty<ColumnMajorMatrix>("global_forces")),
    _Kb(declareProperty<Real>("basic_stiffness")),
    _Kl(declareProperty<ColumnMajorMatrix>("local_stiffness_matrix")),
    _Kg(declareProperty<ColumnMajorMatrix>("global_stiffness_matrix")),
    _total_gl(declareProperty<ColumnMajorMatrix>("total_global_to_local_transformation"))

{
  // Fetch coupled variables (as stateful properties if necessary)
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    MooseVariable * disp_variable = getVar("displacements", i);
    _disp_num[i] = disp_variable->number(); // Displacement variable numbers in MOOSE
  }
}

void
ComputeFVDamperElasticity::initQpStatefulProperties()
{
  _vel[_qp] = 0;
}

void
ComputeFVDamperElasticity::computeQpProperties()
{
  // Compute transformation matrices
  computeTransformationMatrix();

  // Compute the global displacement values
  computeDeformation();

  // Initialize the stiffness matrices and force vectors
  initializeFVdamper();

  // Compute axial forces and stiffness terms
  computeAxial();

  // Finalize forces and stiffness matrix and convert them into global co-ordinate system
  finalize();
}

void
ComputeFVDamperElasticity::computeTransformationMatrix()
{
  // Fetch the two nodes of the link element
  std::vector<Node *> node;
  for (unsigned int i = 0; i < 2; ++i)
    node.push_back(_current_elem->get_node(i));

  // Defining orientation of damper (direction cosines)
  RealGradient x_orientation;
  for (unsigned int i = 0; i < _ndisp; ++i)
    x_orientation(i) = (*node[1])(i) - (*node[0])(i);

  // Length of the damper element
  _length[_qp] = x_orientation.norm();

  if (_length[_qp] == 0.0)
    mooseError("Error in ComputeFVDamperElasticity material block, ",
               name(),
               ". Damper element cannot be of zero length.");

  // Normalizing the orientation vector
  x_orientation /= _length[_qp];

  // Get y orientation of the Damper in global coordinate system
  RealGradient y_orientation = getParam<RealGradient>("y_orientation");
  Real dot = x_orientation * y_orientation;

  // Check if x and y orientations are perpendicular
  if (abs(dot) > 1e-4)
    mooseError("Error in ComputeFVDamperElasticity material block, ",
               name(),
               ". y_orientation should be perpendicular to the axis of the Damper.");

  // Transformation matrix from global to local coordinate system
  _total_gl[_qp].reshape(2, 6);
  _total_gl[_qp].zero();
  _total_gl[_qp](0, 0) = _total_gl[_qp](1, 3) = x_orientation(0); //direction cosine in x
  _total_gl[_qp](0, 1) = _total_gl[_qp](1, 4) = x_orientation(1); //direction cosine in y
  _total_gl[_qp](0, 2) = _total_gl[_qp](1, 5) = x_orientation(2); //direction cosine in z
}

void
ComputeFVDamperElasticity::computeDeformation()
{
  // Fetch the two end nodes for _current_elem
  std::vector<Node *> node;
  for (unsigned int i = 0; i < 2; ++i)
    node.push_back(_current_elem->get_node(i));

  // Fetch the solution for the two end nodes at current time
  NonlinearSystemBase & nonlinear_sys = _fe_problem.getNonlinearSystemBase();
  const NumericVector<Number> & sol = *nonlinear_sys.currentSolution();
  const NumericVector<Number> & sol_old = nonlinear_sys.solutionOld();

  // Calculating global displacements 6 x 1 matrix with
  //first three rows corresponding to node 0 dofs and next three to node 1 dofs
  ColumnMajorMatrix global_disp(6, 1);
  ColumnMajorMatrix global_disp_old(6, 1);

  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    global_disp(i) = sol(node[0]->dof_number(nonlinear_sys.number(), _disp_num[i], 0)); // node 0 displacements
    global_disp(i + 3) = sol(node[1]->dof_number(nonlinear_sys.number(), _disp_num[i], 0)); // node 1 displacements
    global_disp_old(i) = sol_old(node[0]->dof_number(nonlinear_sys.number(), _disp_num[i], 0)); // node 0 displacements
    global_disp_old(i + 3) = sol_old(node[1]->dof_number(nonlinear_sys.number(), _disp_num[i], 0)); // node 1 displacements
  }

  _basic_def[_qp] = (_total_gl[_qp] * global_disp)(1,0)-(_total_gl[_qp] * global_disp)(0,0);
}

void
ComputeFVDamperElasticity::initializeFVdamper()
{
  // initialize basic force and stiffness terms
  _Kb[_qp] = 0;
  _Fb[_qp] = 0;

  // Initialize local force and stiffness terms
  _Kl[_qp].reshape(2,2);
  _Fl[_qp].reshape(2,1);
}

void
ComputeFVDamperElasticity::computeAxial()
{
  _vel[_qp] = _vel_old[_qp] + (_gamma/_beta) *
                  (((_basic_def[_qp] - _basic_def_old[_qp])/_dt) - (_vel_old[_qp]));

  // adaptive iterative algorithm starts

  Real t_vel = _vel[_qp];
  Real accel = (_vel[_qp] - _vel_old[_qp])/_dt;
  Real t_fd = _Fb_old[_qp];
  Real s = 1;
  Real nit = 0;
  Real s_tot = 0;
  Real yt, eps, error;

  do
  {
    Real h = s * _dt;
    Real t_vel_new = t_vel + accel * h;

    dormandPrince(t_vel, t_vel_new, t_fd, h, yt ,eps, error);

    if (eps <=_Rel_tol || fabs(eps) <=_Abs_tol)
    {
      t_vel = t_vel_new;
      t_fd = yt;
      s_tot = s_tot + s;
    }
    else
    {
      nit = nit + 1;
      s = pow(2,-nit);
    }

  } while (s_tot < 1.0);

  // set axial force
  _Fb[_qp] = t_fd;

  // set axial stiffness
  if (_vel[_qp] != 0)
  {
    Real k_damp = _Cd * MathUtils::sign(_vel[_qp]) * _alpha *
                      pow(fabs(_vel[_qp]),_alpha - 1.0)   * (_gamma/_beta)/_dt;
    Real k_spring = _k;

    // springs in series connection
    _Kb[_qp] = k_damp * k_spring / (k_damp + k_spring);
  }
}

void
ComputeFVDamperElasticity::dormandPrince(Real t_vel, Real t_vel_new, Real t_fd, Real h, Real& yt, Real& eps, Real& error)
{
  Real k1 = fdot(t_vel, t_fd) * h;
  Real k2 = fdot((t_vel_new - t_vel) * (1.0 / 5.0) + t_vel,
                t_fd + (1.0 / 5.0) * k1) * h;
  Real k3 = fdot((t_vel_new - t_vel) * (3.0 / 10.0) + t_vel,
                t_fd + (3.0 / 40.0) * k1 + (9.0 / 40.0) * k2) * h;
  Real k4 = fdot((t_vel_new - t_vel) * (4.0 / 5.0) + t_vel,
                t_fd + (44.0 / 45.0) * k1 + (-56.0 / 15.0) * k2 + (32.0 / 9.0) * k3) * h;
  Real k5 = fdot((t_vel_new - t_vel) * (8.0 / 9.0) + t_vel,
                t_fd + (19372.0 / 6561.0) * k1 + (-25360.0 / 2187.0) * k2 + (64448.0 / 6561.0) * k3 +
                (-212.0 / 729.0) * k4) * h;
  Real k6 = fdot((t_vel_new - t_vel) * (1.0) + t_vel,
                t_fd + (9017.0 / 3168.0) * k1 + (-355.0 / 33.0) * k2 + (46732.0 / 5247.0) * k3 +
                (49.0 / 176.0) * k4 + (-5103.0 / 18656.0) * k5) * h;

  yt = t_fd + (35.0 / 384.0) * k1 + (500.0 / 1113.0) * k3 + (125.0 / 192.0) * k4 +
          (-2187.0 / 6784.0) * k5 + (11.0 / 84.0) * k6;

  Real k7 = fdot((t_vel_new - t_vel) * (1.0) + t_vel, yt) * h;

  error = (71.0 / 57600.0) * k1 + (-71.0 / 16695.0) * k3 + (71.0 / 1920.0) * k4 +
              (-17253.0 / 339200.) * k5  + (22.0 / 525.0) * k6 + (-1.0 / 40.0) * k7;

  eps = fabs(error);
}

Real
ComputeFVDamperElasticity::fdot(Real v, Real fd)
{
  return (v - (MathUtils::sign(fd) * pow(fabs(fd)/_Cd, 1.0/_alpha))) * _k;
}

void
ComputeFVDamperElasticity::finalize()
{
  // calculate local forces
  _Fl[_qp](0,0) = -_Fb[_qp]; // force at node 1
  _Fl[_qp](1,0) = _Fb[_qp]; // force at node 2

  // calculate global forces from local system
  _Fg[_qp] = _total_gl[_qp].transpose() * _Fl[_qp];

  // populate the local stiffness matrix
  for (unsigned int i =0; i < 2; ++i)
    for (unsigned int j =0; j<2; j++)
      _Kl[_qp](i,j) = (i==j ? 1 : -1) * _Kb[_qp];

  // convert local stiffness matrix to global coordinate system
  _Kg[_qp] = _total_gl[_qp].transpose() * _Kl[_qp] * _total_gl[_qp];
}
