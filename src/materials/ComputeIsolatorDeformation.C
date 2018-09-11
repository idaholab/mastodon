// MASTODON includes
#include "ComputeIsolatorDeformation.h"

// MOOSE includes
#include "MooseMesh.h"
#include "Assembly.h"
#include "NonlinearSystem.h"
#include "MooseVariable.h"
#include "ColumnMajorMatrix.h"

// libmesh includes
#include "libmesh/quadrature.h"
#include "libmesh/utility.h"

registerMooseObject("MastodonApp", ComputeIsolatorDeformation);

template <>
InputParameters
validParams<ComputeIsolatorDeformation>()
{
  InputParameters params = validParams<Material>();
  params.addClassDescription("Compute the deformations rotations in a two-noded isolator element.");
  params.addRequiredCoupledVar(
      "rotations", "The rotation variables appropriate for the simulation geometry and coordinate system.");
  params.addRequiredCoupledVar(
      "displacements",
      "The displacement variables appropriate for the simulation geometry and coordinate system.");
  params.addRequiredCoupledVar("velocities", "Translational velocity variables.");
  params.addRequiredCoupledVar("accelerations", " acceleration variables.");
  params.addRequiredParam<RealGradient>("y_orientation",
                                        "Orientation of the y direction along "
                                        "with Ky is provided. This should be "
                                        "perpendicular to the axis of the isolator.");
  params.addParam<MaterialPropertyName>("sd_ratio", 0.5, "Shear distance ratio.");
  params.set<MooseEnum>("constant_on") = "ELEMENT"; // sets _qp to 0. Material properties are assumed to be constant throughout the length of the element.
  return params;
}

ComputeIsolatorDeformation::ComputeIsolatorDeformation(const InputParameters & parameters)
  : Material(parameters),
    _nrot(coupledComponents("rotations")),
    _ndisp(coupledComponents("displacements")),
    _nvel(coupledComponents("velocities")),
    _naccel(coupledComponents("accelerations")),
    _rot_num(3),
    _disp_num(3),
    _vel_num(3),
    _accel_num(3),
    _sD(getMaterialProperty<Real>("sd_ratio")),
    _basic_disp(declareProperty<ColumnMajorMatrix>("deformations")),
    _basic_disp_old(declareProperty<ColumnMajorMatrix>("old deformations")),
    _basic_vel(declareProperty<ColumnMajorMatrix>("deformation_rates")),
    _basic_vel_old(declareProperty<ColumnMajorMatrix>("old deformations rates")),
    _basic_accel(declareProperty<ColumnMajorMatrix>("velocity rates")),
    _basic_accel_old(declareProperty<ColumnMajorMatrix>("old velocity rates")),
    _original_gl(declareProperty<ColumnMajorMatrix>("original_global_to_local_transformation")),
    _total_gl(declareProperty<ColumnMajorMatrix>("total_global_to_local_transformation")),
    _total_lb(declareProperty<ColumnMajorMatrix>("total_local_to_basic_transformation")),
    _length(declareProperty<Real>("initial_isolator_length"))
{

  // Checking for consistency between length of the provided displacements and rotations vector
  if (_ndisp != _nrot)
    mooseError("ComputeIsolatorDeformation: The number of variables supplied in 'displacements' "
               "and 'rotations' input parameters must be equal.");

  // Fetch coupled variables and gradients (as stateful properties if necessary)
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    MooseVariable * disp_variable = getVar("displacements", i);
    _disp_num[i] = disp_variable->number(); // Displacement variable numbers in MOOSE

    MooseVariable * vel_variable = getVar("velocities", i);
    _vel_num[i] = vel_variable->number();

    MooseVariable * rot_variable = getVar("rotations", i);
    _rot_num[i] = rot_variable->number(); // Rotation variable numbers in MOOSE

    MooseVariable * accel_variable = getVar("accelerations", i);
    _accel_num[i] = accel_variable->number(); // acceleration variable numbers in MOOSE
  }

}

void
ComputeIsolatorDeformation::computeQpProperties()
{
  // Compute initial orientation and length of the isolator in global coordinate system
  // Fetch the two nodes of the link element
  std::vector<Node *> node;
  for (unsigned int i = 0; i < 2; ++i)
    node.push_back(_current_elem->get_node(i));
  RealGradient x_orientation;
  for (unsigned int i = 0; i < _ndisp; ++i)
    x_orientation(i) = (*node[1])(i) - (*node[0])(i);
  _length[_qp] = x_orientation.norm();
  if (_length[_qp] == 0.0)
    mooseError("Error in isolator material block, ", name(), ". Isolator element cannot be of zero length.");
  x_orientation /= _length[_qp]; // Normalizing with length to get orientation

  // Get y orientation of the isolator in global coordinate system
  RealGradient y_orientation = getParam<RealGradient>("y_orientation");
  // Real dot = x_orientation(0) * y_orientation(0) + x_orientation(1) * y_orientation(1) +
  //            x_orientation(2) * y_orientation(2);
  Real dot = x_orientation * y_orientation;

  // Check if x and y orientations are perpendicular
  if (abs(dot) > 1e-4)
    mooseError("Error in isolator block, ", name(), ". y_orientation should be perpendicular to "
               "the axis of the isolator.");

  // Calculate z orientation in the global coordinate system as a cross product of the x and y orientations
  RealGradient z_orientation = x_orientation.cross(y_orientation);

  // Create rotation matrix from global to local coordinate system
  _original_gl[_qp].reshape(12, 12);
  _original_gl[_qp].zero();
  _original_gl[_qp](0, 0) = _original_gl[_qp](3, 3) = _original_gl[_qp](6, 6) = _original_gl[_qp](9, 9)   = x_orientation(0);
  _original_gl[_qp](0, 1) = _original_gl[_qp](3, 4) = _original_gl[_qp](6, 7) = _original_gl[_qp](9, 10)  = x_orientation(1);
  _original_gl[_qp](0, 2) = _original_gl[_qp](3, 5) = _original_gl[_qp](6, 8) = _original_gl[_qp](9, 11)  = x_orientation(2);
  _original_gl[_qp](1, 0) = _original_gl[_qp](4, 3) = _original_gl[_qp](7, 6) = _original_gl[_qp](10, 9)  = y_orientation(0);
  _original_gl[_qp](1, 1) = _original_gl[_qp](4, 4) = _original_gl[_qp](7, 7) = _original_gl[_qp](10, 10) = y_orientation(1);
  _original_gl[_qp](1, 2) = _original_gl[_qp](4, 5) = _original_gl[_qp](7, 8) = _original_gl[_qp](10, 11) = y_orientation(2);
  _original_gl[_qp](2, 0) = _original_gl[_qp](5, 3) = _original_gl[_qp](8, 6) = _original_gl[_qp](11, 9)  = z_orientation(0);
  _original_gl[_qp](2, 1) = _original_gl[_qp](5, 4) = _original_gl[_qp](8, 7) = _original_gl[_qp](11, 10) = z_orientation(1);
  _original_gl[_qp](2, 2) = _original_gl[_qp](5, 5) = _original_gl[_qp](8, 8) = _original_gl[_qp](11, 11) = z_orientation(2);

  // Create rotation matrix from local to basic system (linear)
  _total_lb[_qp].reshape(6, 12);
  _total_lb[_qp].zero();
  _total_lb[_qp](0,0) = _total_lb[_qp](1,1) = _total_lb[_qp](2,2) = _total_lb[_qp](3,3) = _total_lb[_qp](4,4) = _total_lb[_qp](5,5) = -1.0;
  _total_lb[_qp](0,6) = _total_lb[_qp](1,7) = _total_lb[_qp](2,8) = _total_lb[_qp](3,9) = _total_lb[_qp](4,10) = _total_lb[_qp](5,11) = 1.0;
  _total_lb[_qp](1,5) = -_sD[0] * _length[_qp];
  _total_lb[_qp](1,11) = -(1.0 - _sD[0]) * _length[_qp];
  _total_lb[_qp](2,4) = -_total_lb[_qp](1,5);
  _total_lb[_qp](2,10) = -_total_lb[_qp](1,11);

  computeTotalRotation();
  computeDeformation();
}

void
ComputeIsolatorDeformation::computeTotalRotation()
{
  std::size_t _qp = 0;
  // Currently this formulation is limited to small deformations in the isolator,
  // namely, it is assumed that there are no rigid body rotations in the isolator,
  // and that the total rotation matrix from global to local coordinates
  // (calculated below) remains the same as the one at t = 0 throughout the
  // duration of the simulation.
  _total_gl[_qp] = _original_gl[_qp];
}

void
ComputeIsolatorDeformation::computeDeformation()
{
  // fetch the two end nodes for _current_elem
  std::vector<Node *> node;
  for (unsigned int i = 0; i < 2; ++i)
    node.push_back(_current_elem->get_node(i));

  // Fetch the solution for the two end nodes at time t
  NonlinearSystemBase & nonlinear_sys = _fe_problem.getNonlinearSystemBase();
  const NumericVector<Number> & sol = *nonlinear_sys.currentSolution();
  const NumericVector<Number> & sol_old = nonlinear_sys.solutionOld();

  AuxiliarySystem & aux = _fe_problem.getAuxiliarySystem();
  const NumericVector<Number> & aux_sol = *aux.currentSolution();
  const NumericVector<Number> & aux_sol_old = aux.solutionOld();

  // Calculating global displacements (including rotations) and velocities (rotational velocity terms are zero)
  // 12 x 1 matrix with first six rows corresponding to node 0 dofs and next six to node 1 dofs
  ColumnMajorMatrix global_disp(12, 1);
  ColumnMajorMatrix global_vel(12, 1);
  ColumnMajorMatrix global_disp_old(12, 1);
  ColumnMajorMatrix global_vel_old(12, 1);
  ColumnMajorMatrix global_accel(12, 1);
  ColumnMajorMatrix global_accel_old(12, 1);
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    global_disp(i) = sol(node[0]->dof_number(nonlinear_sys.number(), _disp_num[i], 0)); // node 0 displacements
    global_disp(i + 3) = sol(node[0]->dof_number(nonlinear_sys.number(), _rot_num[i], 0)); // node 0 rotations
    global_disp(i + 6) = sol(node[1]->dof_number(nonlinear_sys.number(), _disp_num[i], 0)); // node 1 displacements
    global_disp(i + 9) = sol(node[1]->dof_number(nonlinear_sys.number(), _rot_num[i], 0));// node 1 rotations
    global_disp_old(i) = sol_old(node[0]->dof_number(nonlinear_sys.number(), _disp_num[i], 0)); // node 0 displacements
    global_disp_old(i + 3) = sol_old(node[0]->dof_number(nonlinear_sys.number(), _rot_num[i], 0)); // node 0 rotations
    global_disp_old(i + 6) = sol_old(node[1]->dof_number(nonlinear_sys.number(), _disp_num[i], 0)); // node 1 displacements
    global_disp_old(i + 9) = sol_old(node[1]->dof_number(nonlinear_sys.number(), _rot_num[i], 0));// node 1 rotations
    global_vel(i) = aux_sol(node[0]->dof_number(aux.number(), _vel_num[i], 0)); // node 0 velocities; rotational velocity terms remain zero
    global_vel(i + 6) = aux_sol_old(node[1]->dof_number(aux.number(), _vel_num[i], 0)); // node 1 velocities; rotational velocity terms remain zero
    global_vel_old(i) = aux_sol_old(node[0]->dof_number(aux.number(), _vel_num[i], 0)); // node 0 velocities; rotational velocity terms remain zero
    global_vel_old(i + 6) = aux_sol_old(node[1]->dof_number(aux.number(), _vel_num[i], 0)); // node 1 velocities; rotational velocity terms remain zero
    global_accel(i) = aux_sol(node[0]->dof_number(aux.number(), _accel_num[i], 0)); // node 0 accelerations; rotational accelration terms remain zero
    global_accel(i + 6) = aux_sol_old(node[1]->dof_number(aux.number(), _accel_num[i], 0)); // node 1 accelerations; rotational accelerations terms remain zero
    global_accel_old(i) = aux_sol_old(node[0]->dof_number(aux.number(), _accel_num[i], 0)); // node 0 accelerations; rotational accelerations terms remain zero
    global_accel_old(i + 6) = aux_sol_old(node[1]->dof_number(aux.number(), _accel_num[i], 0)); // node 1 accelerations; rotational accelerations terms remain zero
    }

  // Converting global deformations and deformation rates to the isolator
  // basic system.
  _basic_disp[_qp].reshape(6, 1);
  _basic_vel[_qp].reshape(6, 1);
  _basic_disp_old[_qp].reshape(6, 1);
  _basic_vel_old[_qp].reshape(6, 1);
  _basic_accel[_qp].reshape(6, 1);
  _basic_accel_old[_qp].reshape(6, 1);

  _basic_disp[_qp] = _total_lb[_qp] * _total_gl[_qp] * global_disp;
  _basic_vel[_qp] = _total_lb[_qp] * _total_gl[_qp] * global_vel;
  _basic_disp_old[_qp] = _total_lb[_qp] * _total_gl[_qp] * global_disp_old;
  _basic_vel_old[_qp] = _total_lb[_qp] * _total_gl[_qp] * global_vel_old;
  _basic_accel[_qp] = _total_lb[_qp] * _total_gl[_qp] * global_accel;
  _basic_accel_old[_qp] = _total_lb[_qp] * _total_gl[_qp] * global_accel_old;
}
