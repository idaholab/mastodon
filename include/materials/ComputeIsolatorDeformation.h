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

#ifndef COMPUTEISOLATORDEFORMATION_H
#define COMPUTEISOLATORDEFORMATION_H

#include "Material.h"
#include "RankTwoTensor.h"
/**
 * ComputeIsolatorDeformation calculates the deformations and rotations in a
 * isolator element.
 */

// Forward Declarations
class ComputeIsolatorDeformation;

template <>
InputParameters validParams<ComputeIsolatorDeformation>();

class ComputeIsolatorDeformation : public Material
{
public:
  ComputeIsolatorDeformation(const InputParameters & parameters);

protected:
  /// Compute properties for each qp (_qp = 0)
  virtual void computeQpProperties() override;

  /// Computes the total rotation matrix accounting for rigid body rotations.
  /// Relevant for large rotation problems. Currently not implemented.
  void computeTotalRotation();

  /// Computes the displacement and rotational deformations of the isolator
  void computeDeformation();

  /// Number of coupled rotational variables
  unsigned int _nrot;

  /// Number of coupled displacement variables
  unsigned int _ndisp;

  /// Number of coupled velocity variables
  unsigned int _nvel;

  /// Variable numbers corresponding to the rotational variables
  std::vector<unsigned int> _rot_num;

  /// Variable numbers corresponding to the displacement variables
  std::vector<unsigned int> _disp_num;

  /// Variable numbers corresponding to the velocity variables
  std::vector<unsigned int> _vel_num;

  /// Shear distance ratio
  const MaterialProperty<Real> & _sD;

  /// Displacements in the isolator basic system, namely, deformations
  MaterialProperty<ColumnMajorMatrix> & _basic_disp;

  /// Displacements in the isolator basic system, namely, old deformations
  MaterialProperty<ColumnMajorMatrix> & _basic_disp_old;

  /// Velocities in the isolator basic system, namely, deformation rates
  MaterialProperty<ColumnMajorMatrix> & _basic_vel;

  /// Velocities in the isolator basic system, namely, old deformation rates
  MaterialProperty<ColumnMajorMatrix> & _basic_vel_old;

  /// Transformation matrix from global coordinate system to isolator local configuration at t=0
  MaterialProperty<ColumnMajorMatrix> & _original_gl;

  /// Transformation matrix from global coordinate system to isolator local configuration at any time
  MaterialProperty<ColumnMajorMatrix> & _total_gl;

  /// Transformation matrix from isolator local coordinate system to basic configuration at any time
  MaterialProperty<ColumnMajorMatrix> & _total_lb;

  /// Initial length of the isolator
  MaterialProperty<Real> & _length;
};

#endif // COMPUTEISOLATORDEFORMATION_H
