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
  virtual void computeProperties() override;
  virtual void initQpStatefulProperties() override;

  /// Computes the displacement and rotation strain increments
  void computeQpDeformation();

  // /// Computes the stiffness matrices
  // void computeJacobian();

  /// Number of coupled rotational variables
  unsigned int _nrot;

  /// Number of coupled displacement variables
  unsigned int _ndisp;

  /// Variable numbers corresponding to the rotational variables
  std::vector<unsigned int> _rot_num;

  /// Variable numbers corresponding to the displacement variables
  std::vector<unsigned int> _disp_num;

  /// Rotational transformation from global coordinate system to intiial beam local configuration
  MaterialProperty<RankTwoTensor> & _original_global_to_local_rotation;

  /// Deformations in the spring calculated at a time t in the spring local coordinates
  MaterialProperty<RealVectorValue> & _deformations;

  /// Deformations in the spring calculated at a time t in the spring local coordinates
  MaterialProperty<RealVectorValue> & _rotations;
};

#endif // COMPUTESPRINGDEFORMATION_H
