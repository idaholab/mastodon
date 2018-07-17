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

#ifndef LINEARSPRING_H
#define LINEARSPRING_H

#include "Material.h"
#include "RankTwoTensor.h"

// Forward Declarations
class LinearSpring;

template <>
InputParameters validParams<LinearSpring>();

/**
 * LinearSpring material simulates a linear spring with a diagonal stiffness
 * matrix, including rotational stiffnesses.
 */
class LinearSpring : public Material
{
public:
  LinearSpring(const InputParameters & parameters);

protected:
  /// Compute properties for each qp
  virtual void computeQpProperties() override;

  /// Computes the total rotation matrix accounting for rigid body rotations.
  /// Relevant for large rotation problems. Currently approximated as the
  /// initial rotation (t=0) matrix and the exact calculation of total rotation
  /// is not implemented.
  void computeTotalRotation();

  /// Computes the displacement and rotation deformations of the spring element
  void computeDeformations();

  /// Computes the spring forces in global coordinate system
  void computeForces();

  /// Computes stiffness matrix relating the forces and displacements in the
  /// global coordinate system
  void computeStiffnessMatrix();

  /// Number of coupled rotational variables
  unsigned int _nrot;

  /// Number of coupled displacement variables
  unsigned int _ndisp;

  /// Variable numbers corresponding to the rotational variables
  std::vector<unsigned int> _rot_num;

  /// Variable numbers corresponding to the displacement variables
  std::vector<unsigned int> _disp_num;

  /// Deformations in the spring calculated at a time t in the spring local coordinates
  MaterialProperty<RealVectorValue> & _deformations;

  /// Rotations in the spring calculated at a time t in the spring local coordinates
  MaterialProperty<RealVectorValue> & _rotations;

  /// Axial stiffness of the spring (local x direction)
  const VariableValue & _kx;

  /// Shear stiffness of the spring in local y direction
  const VariableValue & _ky;

  /// Shear stiffness of the spring in local z direction
  const VariableValue & _kz;

  /// Torsional stiffness of the spring (local x direction)
  const VariableValue & _krx;

  /// Rotational stiffness of the spring in the local y direction
  const VariableValue & _kry;

  /// Rotational stiffness of the spring in the local z direction
  const VariableValue & _krz;

  /// Global displacement at node 0 of the spring element
  RealVectorValue _global_disp0; // node 0

  /// Global displacement at node 1 of the spring element
  RealVectorValue _global_disp1; // node 1

  /// Global rotation at node 0 of the spring element
  RealVectorValue _global_rot0; // node 0

  /// Global rotation at node 1 of the spring element
  RealVectorValue _global_rot1; // node 1

  /// Local displacement at node 0 of the spring element
  RealVectorValue _local_disp0; // node 0

  /// Local displacement at node 1 of the spring element
  RealVectorValue _local_disp1; // node 1

  /// Local rotation at node 0 of the spring element
  RealVectorValue _local_rot0; // node 0

  /// Local rotation at node 1 of the spring element
  RealVectorValue _local_rot1; // node 1

  /// Spring forces in the local coordinate system
  RealVectorValue _spring_forces_local;

  // Spring moments in the local coordinate system
  RealVectorValue _spring_moments_local;

  /// Spring displacement stiffness matrix in the local coordinate system
  RankTwoTensor _kdd_local;

  /// Spring rotational stiffness matrix in the local coordinate system
  RankTwoTensor _krr_local;

  /// Spring forces in the global coordinate system
  MaterialProperty<RealVectorValue> & _spring_forces_global;

  /// Spring moments in the global coordinate system
  MaterialProperty<RealVectorValue> & _spring_moments_global;

  /// Spring displacement stiffness matrix in the global coordinate system
  MaterialProperty<RankTwoTensor> & _kdd;

  /// Spring rotational stiffness matrix in the global coordinate system
  MaterialProperty<RankTwoTensor> & _krr;

  /// Rotational transformation from global coordinate system to spring local configuration at t = 0
  RankTwoTensor _original_global_to_local_rotation;

  /// Rotational transformation from global coordinate system to spring local configuration at any time
  MaterialProperty<RankTwoTensor> & _total_global_to_local_rotation;
};

#endif // LINEARSPRING_H
