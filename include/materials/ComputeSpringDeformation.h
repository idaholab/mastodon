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

#ifndef COMPUTESPRINGDEFORMATION_H
#define COMPUTESPRINGDEFORMATION_H

#include "Material.h"
#include "RankTwoTensor.h"
/**
 * ComputeSpringDeformation calculates the deformations and rotations in a
 * spring element.
 */

// Forward Declarations
class ComputeSpringDeformation;

template <>
InputParameters validParams<ComputeSpringDeformation>();

class ComputeSpringDeformation : public Material
{
public:
  ComputeSpringDeformation(const InputParameters & parameters);

protected:
  /// Initialize properties
  virtual void initQpStatefulProperties() override;

  /// Compute properties for each qp
  virtual void computeProperties() override;

  /// Computes the total rotation matrix accounting for rigid body rotations.
  /// Relevant for large rotation problems. Currently not implemented.
  void computeTotalRotation();

  /// Computes the displacement and rotation deformations of the spring element
  void computeQpDeformation();

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

  /// Deformations in the spring calculated at a time t in the spring local coordinates
  MaterialProperty<RealVectorValue> & _rotations;

  /// Rotational transformation from global coordinate system to spring local configuration at t = 0
  MaterialProperty<RankTwoTensor> & _original_global_to_local_rotation;

  /// Rotational transformation from global coordinate system to spring local configuration at any time
  MaterialProperty<RankTwoTensor> & _total_global_to_local_rotation;
};

#endif // COMPUTESPRINGDEFORMATION_H
