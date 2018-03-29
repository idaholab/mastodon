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

#ifndef COMPUTESPRINGELASTICITY_H
#define COMPUTESPRINGELASTICITY_H

#include "Material.h"
#include "RankTwoTensor.h"
/**
 * ComputeSpringElasticity calculates the forces and stiffness matrix
 * corresponding to a spring element.
 */

// Forward Declarations
class ComputeSpringElasticity;

template <>
InputParameters validParams<ComputeSpringElasticity>();

class ComputeSpringElasticity : public Material
{
public:
  ComputeSpringElasticity(const InputParameters & parameters);

protected:
  /// Initialize the properties
  virtual void initQpStatefulProperties() override;

  /// Computes the spring forces in global coordinate system
  void computeForces();

  /// Computes stiffness matrix relating the forces and displacements in the
  /// global coordinate system
  void computeStiffnessMatrix();

  /// Axial stiffness of the spring
  const VariableValue & _kx;

  /// Shear stiffness of the spring in local y direction
  const VariableValue & _ky;

  /// Shear stiffness of the spring in local z direction
  const VariableValue & _kz;

  /// Torsional stiffness of the spring
  const VariableValue & _krx;

  /// Rotational stiffness of the spring in the local y direction
  const VariableValue & _kry;

  /// Rotational stiffness of the spring in the local z direction
  const VariableValue & _krz;

  /// Deformations in the spring calculated at a time t in the spring local coordinates
  const MaterialProperty<RealVectorValue> & _deformations;

  /// Deformations in the spring calculated at a time t in the spring local coordinates
  const MaterialProperty<RealVectorValue> & _rotations;

  /// Spring forces in the global coordinate system
  MaterialProperty<RealVectorValue> & _spring_forces;

  /// Spring moments in the global coordinate system
  MaterialProperty<RealVectorValue> & _spring_moments;

  /// Spring displacement stiffness matrix in the global coordinate system
  MaterialProperty<RankTwoTensor> & _kdd;

  /// Spring rotational stiffness matrix in the global coordinate system
  MaterialProperty<RankTwoTensor> & _krr;

  /// Spring stiffness matrix relating displacement and rotation degrees of freedom
  // MaterialProperty<RankTwoTensor> & _kdr;

  /// Rotational transformation matrix from global coordinate system to spring
  /// local configuration at any time
  const MaterialProperty<RankTwoTensor> & _total_global_to_local_rotation;
};

#endif // COMPUTESPRINGELASTICITY_H
