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

#ifndef COMPUTEFVDAMPERELASTICITY_H
#define COMPUTEFVDAMPERELASTICITY_H

#include "Material.h"
#include "ColumnMajorMatrix.h"

/**
 * ComputeFVDamperElasticity calculates the deformations,forces and stiffness matrix
 * of a damper element.
 **/

class ComputeFVDamperElasticity : public Material
{
public:
  static InputParameters validParams();
  ComputeFVDamperElasticity(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  void initQpStatefulProperties() override;

  /// compute transformation matrix for the damper element
  void computeTransformationMatrix();

  /// compute global deformations of the damper element
  void computeDeformation();

  /// initilize properties
  void initializeFVdamper();

  /// computes the basic forces and stiffness (along axis direction)
  void computeAxial();

  /// convert basic forces and stiffness into global co-ordinate system
  void finalize();

  /// function to obtain numerical solution of the damper element
  void
  dormandPrince(Real _TVel, Real _TVel_new, Real _TFd, Real h, Real & yt, Real & eps, Real & error);

  /// function that returns the force rate
  Real fdot(Real v, Real fd);

  /// number of coupled displacement variables
  unsigned int _ndisp;

  /// variable numbers corresponding to the displacement variables
  std::vector<unsigned int> _disp_num;

  /// initial length of the damper
  MaterialProperty<Real> & _length;

  // damper material, response and geomtric parameters
  //
  /// damping co-efficient
  const Real & _cd;

  /// velocity exponent of the damper
  const Real & _alpha;

  /// axial stiffness of the damper assembly
  const Real & _k;

  /// gamma parameter of Newmark algorithm
  const Real & _gamma;

  /// beta parameter of Newmark algorithm
  const Real & _beta;

  /// relative tolerance for error in adaptive algorithm
  const Real & _rel_tol;

  /// absolute tolerance for error in adaptive algorithm
  const Real & _abs_tol;

  /// displacements in the damper basic system, namely, deformations
  MaterialProperty<Real> & _basic_def;

  /// displacements in the damper basic system, namely, old deformations
  const MaterialProperty<Real> & _basic_def_old;

  /// velocity in current step
  MaterialProperty<Real> & _vel;

  /// velocity in previous step
  const MaterialProperty<Real> & _vel_old;

  /// damper forces in the basic coordinate system
  MaterialProperty<Real> & _Fb;

  /// damper forces of the previous step in the basic coordinate system
  const MaterialProperty<Real> & _Fb_old;

  /// damper forces in the local coordinate system
  MaterialProperty<ColumnMajorMatrix> & _Fl;

  /// damper forces in the global coordinate system
  MaterialProperty<ColumnMajorMatrix> & _Fg;

  /// damper stiffness in the basic coordinate system
  MaterialProperty<Real> & _Kb;

  /// damper stiffness in the local coordinate system
  MaterialProperty<ColumnMajorMatrix> & _Kl;

  /// damper stiffness in the global coordinate system
  MaterialProperty<ColumnMajorMatrix> & _Kg;

  /// transformation matrix from global coordinate system to damper local configuration at any time
  MaterialProperty<ColumnMajorMatrix> & _total_gl;
};

#endif // COMPUTEFVDAMPERELASTICITY_H
