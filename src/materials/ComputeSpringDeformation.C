// MASTODON includes
#include "ComputeSpringDeformation.h"

// MOOSE includes
#include "MooseMesh.h"
#include "Assembly.h"
#include "NonlinearSystem.h"
#include "MooseVariable.h"
#include "RankTwoTensor.h"

// libmesh includes
#include "libmesh/quadrature.h"
#include "libmesh/utility.h"

registerMooseObject("MastodonApp", ComputeSpringDeformation);

template <>
InputParameters
validParams<ComputeSpringDeformation>()
{
  InputParameters params = validParams<Material>();
  params.addClassDescription("Compute the deformations rotations in a two-noded spring element.");
  params.addRequiredCoupledVar(
      "rotations", "The rotation variables appropriate for the simulation geometry and coordinate system.");
  params.addRequiredCoupledVar(
      "displacements",
      "The displacement variables appropriate for the simulation geometry and coordinate system.");
  params.addRequiredParam<RealGradient>("y_orientation",
                                        "Orientation of the y direction along "
                                        "with Ky is provided. This should be "
                                        "perpendicular to the axis of the spring.");
  return params;
}

ComputeSpringDeformation::ComputeSpringDeformation(const InputParameters & parameters)
  : Material(parameters),
    _nrot(coupledComponents("rotations")),
    _ndisp(coupledComponents("displacements")),
    _rot_num(3),
    _disp_num(3),
    _deformations(declareProperty<RealVectorValue>("deformations")),
    _rotations(declareProperty<RealVectorValue>("rotations")),
    _original_global_to_local_rotation(declareProperty<RankTwoTensor>("original_global_to_local_rotation")),
    _total_global_to_local_rotation(declareProperty<RankTwoTensor>("total_global_to_local_rotation"))
{
  // Checking for consistency between mesh dimension and length of the provided displacements vector
  if (_ndisp != _mesh.dimension() || _nrot != _mesh.dimension())
    mooseError("The number of variables supplied in 'displacements' and 'rotations' must match the "
               "mesh dimension. \n Mesh dimension = ", _mesh.dimension(),
               "\nNumber of displacement variables = ", _ndisp,
               "\nNumber of rotation variables = ", _nrot, "\n");

  // Fetch coupled variables and gradients (as stateful properties if necessary)
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    MooseVariable * disp_variable = getVar("displacements", i);
    _disp_num[i] = disp_variable->number(); // Displacement variable numbers in MOOSE

    MooseVariable * rot_variable = getVar("rotations", i);
    _rot_num[i] = rot_variable->number(); // Rotation variable numbers in MOOSE
  }
}

void
ComputeSpringDeformation::initQpStatefulProperties()
{
  // Compute initial orientation and length of the spring in global coordinate system
  // Fetch the two nodes of the link element
  std::vector<Node *> node;
  for (unsigned int i = 0; i < 2; ++i)
    node.push_back(_current_elem->get_node(i));
  RealGradient x_orientation;
  for (unsigned int i = 0; i < _ndisp; ++i)
    x_orientation(i) = (*node[1])(i) - (*node[0])(i);
  // _original_length[_qp] = x_orientation.norm();
  x_orientation /= x_orientation.norm(); // Normalizing with length to get orientation

  // Get y orientation of the spring in global coordinate system
  RealGradient y_orientation = getParam<RealGradient>("y_orientation");
  // Real dot = x_orientation(0) * y_orientation(0) + x_orientation(1) * y_orientation(1) +
  //            x_orientation(2) * y_orientation(2);
  Real dot = x_orientation * y_orientation;

  // Check if x and y orientations are perpendicular
  if (abs(dot) > 1e-4)
    mooseError("Error in ComputeSpringDeformation: y_orientation should be perpendicular to "
               "the axis of the beam.");

  // Calculate z orientation in the global coordinate system as a cross product of the x and y orientations
  RealGradient z_orientation = x_orientation.cross(y_orientation);
  // z_orientation(0) = (x_orientation(1) * y_orientation(2) - x_orientation(2) * y_orientation(1));
  // z_orientation(1) = (x_orientation(2) * y_orientation(0) - x_orientation(0) * y_orientation(2));
  // z_orientation(2) = (x_orientation(0) * y_orientation(1) - x_orientation(1) * y_orientation(0));

  // Calculate the rotation matrix from global to spring local configuration at t = 0
  _original_global_to_local_rotation[_qp](0, 0) = x_orientation(0);
  _original_global_to_local_rotation[_qp](0, 1) = x_orientation(1);
  _original_global_to_local_rotation[_qp](1, 0) = y_orientation(0);
  _original_global_to_local_rotation[_qp](1, 1) = y_orientation(1);
  _original_global_to_local_rotation[_qp](0, 2) = x_orientation(2);
  _original_global_to_local_rotation[_qp](1, 2) = y_orientation(2);
  _original_global_to_local_rotation[_qp](2, 0) = z_orientation(0);
  _original_global_to_local_rotation[_qp](2, 1) = z_orientation(1);
  _original_global_to_local_rotation[_qp](2, 2) = z_orientation(2);
}

void
ComputeSpringDeformation::computeProperties()
{
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
    computeQpDeformation();
}

void
ComputeSpringDeformation::computeTotalRotation()
{
  // Currently this forumation is limited to small deformations in the spring,
  // namely, it is assumed that there are no rigid body rotations in the spring,
  // and that the total rotation matrix from global to local coordinates
  // (calculated below) remains the same as the one at t = 0 throughout the
  // duration of the simulation.
  _total_global_to_local_rotation[_qp] = _original_global_to_local_rotation[_qp];
}

void
ComputeSpringDeformation::computeQpDeformation()
{
  // fetch the two end nodes for _current_elem
  std::vector<Node *> node;
  for (unsigned int i = 0; i < 2; ++i)
    node.push_back(_current_elem->get_node(i));

  // Fetch the solution for the two end nodes at time t
  NonlinearSystemBase & nonlinear_sys = _fe_problem.getNonlinearSystemBase();
  const NumericVector<Number> & sol = *nonlinear_sys.currentSolution();

  // Calculating global displacements and rotations at the end nodes
  RealVectorValue global_disp0(_mesh.dimension(), 0.0); // node 0
  RealVectorValue global_disp1(_mesh.dimension(), 0.0); // node 1
  RealVectorValue global_rot0(_mesh.dimension(), 0.0); // node 0
  RealVectorValue global_rot1(_mesh.dimension(), 0.0); // node 1
  for (unsigned int i = 0; i < _ndisp; ++i)
  {
    global_disp0(i) = sol(node[0]->dof_number(nonlinear_sys.number(), _disp_num[i], 0));
    global_disp1(i) = sol(node[1]->dof_number(nonlinear_sys.number(), _disp_num[i], 0));
    global_rot0(i) = sol(node[0]->dof_number(nonlinear_sys.number(), _rot_num[i], 0));
    global_rot1(i) = sol(node[1]->dof_number(nonlinear_sys.number(), _rot_num[i], 0));
  }

  // Convert spring nodal displacements and rotations from global coordinate system to local coordinate system
  // First, compute total rotation
  computeTotalRotation();
  RealVectorValue local_disp0(_total_global_to_local_rotation[0] * global_disp0);
  RealVectorValue local_disp1(_total_global_to_local_rotation[0] * global_disp1);
  RealVectorValue local_rot0(_total_global_to_local_rotation[0] * global_rot0);
  RealVectorValue local_rot1(_total_global_to_local_rotation[0] * global_rot1);

  // Calculating spring deformations and rotations. Deformations and rotations
  // are assumed to be constant through the length of the spring.
  _deformations[_qp] = local_disp1 - local_disp0;
  _rotations[_qp] = local_rot1 - local_rot0;
}
