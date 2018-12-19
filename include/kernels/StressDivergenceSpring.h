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

#ifndef STRESSDIVERGENCESPRING_H
#define STRESSDIVERGENCESPRING_H

#include "Kernel.h"

// Forward Declarations
class StressDivergenceSpring;
template <typename>
class RankTwoTensorTempl;
typedef RankTwoTensorTempl<Real> RankTwoTensor;

template <>
InputParameters validParams<StressDivergenceSpring>();

class StressDivergenceSpring : public Kernel
{
public:
  StressDivergenceSpring(const InputParameters & parameters);
  virtual void computeResidual() override;
  virtual void computeJacobian() override;
  virtual void computeOffDiagJacobian(MooseVariableFEBase & jvar) override;
  using Kernel::computeOffDiagJacobian;

protected:
  virtual Real computeQpResidual() override { return 0.0; }

  /// Direction along which force/moment is calculated
  const unsigned int _component;

  /// Number of coupled displacement variables
  unsigned int _ndisp;

  /// Variable numbers corresponding to displacement variables
  std::vector<unsigned int> _disp_var;

  /// Number of coupled rotational variables
  unsigned int _nrot;

  /// Variable numbers corresponding to rotational variables
  std::vector<unsigned int> _rot_var;

  /// Spring forces
  const MaterialProperty<RealVectorValue> & _spring_forces_global;

  /// Spring moments
  const MaterialProperty<RealVectorValue> & _spring_moments_global;

  /// Displacement stiffness matrix
  const MaterialProperty<RankTwoTensor> & _kdd;

  /// Rotation stiffness matrix
  const MaterialProperty<RankTwoTensor> & _krr;

  /// Rotation stiffness matrix
  const MaterialProperty<RankTwoTensor> & _total_global_to_local_rotation;
};

#endif // STRESSDIVERGENCESPRING_H
