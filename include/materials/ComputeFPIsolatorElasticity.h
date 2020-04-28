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

#ifndef COMPUTEFPISOLATORELASTICITY_H
#define COMPUTEFPISOLATORELASTICITY_H

#include "Material.h"
#include "ColumnMajorMatrix.h"

/**
 * ComputeFPIsolatorElasticity calculates the forces and stiffness matrix
 * of an FP isolator element.
 */

class ComputeFPIsolatorElasticity : public Material
{
public:
  static InputParameters validParams();
  ComputeFPIsolatorElasticity(const InputParameters & parameters);

protected:
  /// Compute stiffness matrix and force in the basic system
  virtual void computeQpProperties() override;

  void initQpStatefulProperties() override;

  /// Initialize properties
  void initializeFPIsolator();

  /// Computes the forces and stiffness matrix in shear
  void computeShear();

  /// Changes the stiffness matrix in the local coordinate system by adding P-Delta effects
  void addPDeltaEffects();

  /// Finalize the forces and stiffnesses and convert into global coordinate system
  void finalize();

  /// Switch for modeling friction dependence on the instantaneous pressure
  const bool & _pressure_dependent;

  /// Switch for modeling friction dependence on the instantaneous temperature
  const bool & _temperature_dependent;

  /// Switch for modeling friction dependence on the instantaneous sliding velocity
  const bool & _velocity_dependent;

  //
  // Isolator material, response and geometric parameters
  //

  /// Reference co-efficient of friction
  const Real & _mu_ref;

  /// Reference axial pressure
  const Real & _p_ref;

  /// Thermal diffusivity of steel
  const Real & _diffusivity;

  /// Thermal conductivity of steel
  const Real & _conductivity;

  /// Rate parameter
  const Real & _a;

  /// Effective radius of curvature of sliding surface
  const Real & _r_eff;

  /// Radius of contact area at sliding surface
  const Real & _r_contact;

  /// Yield displacement of the bearing in shear
  const Real & _uy;

  /// Unit conversion for pressure to be used in the pressure factor computation
  const unsigned int & _unit;

  /// Gamma parameter of Newmark algorithm
  const Real & _gamma;

  /// Beta parameter of Newmark algorithm
  const Real & _beta;

  /// Stiffness of the bearing in translation along x axis
  const Real & _k_x;

  /// Stiffness of the bearing in rotation about x axis
  const Real & _k_xx;

  /// Stiffness of the bearing in rotation about y axis
  const Real & _k_yy;

  /// Stiffness of the bearing in rotation about z axis
  const Real & _k_zz;

  /// Convergence tolerance to satisfy equilibrium of element
  const Real & _tol;

  /// Maximum number of iterations for equilibrium
  const Real & _maxiter;

  /// Shear distance ratio
  Real _sD;

  /// Isolator deformations in the local system
  const MaterialProperty<ColumnMajorMatrix> & _local_def;

  /// Isolator deformations in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_def;

  /// Isolator old deformations in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_def_old;

  /// Isolator deformation rates in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_vel;

  /// Isolator old deformation rates in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_vel_old;

  /// Isolator forces (including moments) in the basic system
  MaterialProperty<ColumnMajorMatrix> & _Fb;

  /// Isolator forces (including moments) in the local system
  MaterialProperty<ColumnMajorMatrix> & _Fl;

  /// Isolator forces (including moments) in the global system
  MaterialProperty<ColumnMajorMatrix> & _Fg;

  /// Isolator stiffness (including rotational stiffness) in the basic system
  MaterialProperty<ColumnMajorMatrix> & _Kb;

  /// Isolator stiffness (including rotational stiffness) in the local system
  MaterialProperty<ColumnMajorMatrix> & _Kl;

  /// Isolator stiffness (including rotational stiffness) in the global system
  MaterialProperty<ColumnMajorMatrix> & _Kg;

  /// Transformation matrix from global coordinate system to isolator local configuration at any time
  const MaterialProperty<ColumnMajorMatrix> & _total_gl;

  /// Transformation matrix from isolator local coordinate system to basic configuration at any time
  const MaterialProperty<ColumnMajorMatrix> & _total_lb;

  /// Length of the bearing at t = 0
  const MaterialProperty<Real> & _length;

  /// The number, pi
  const Real & _pi;

  //
  /// Other parameters
  //

  /// Mass of the slider
  Real _mass_slider;

  /// Lateral stiffness of bearing before sliding begins
  Real _k0;

  /// Pressure factor
  Real _kpF;

  /// Temperature factor
  Real _ktF;

  /// Velocity factor
  Real _kvF;

  /// Temperature at the center of sliding surface
  Real _temperature_center;

  /// Adjusted value of co-efficient of friction
  Real _mu_adj;

  /// Heat flux at the center of sliding surface
  Real _heatflux_center;

  /// Displacement increment in current step
  Real _disp_currentstep;

  /// Velocity increment in current step
  Real _vel_currentstep;

  /// Plastic displacements in basic system
  MaterialProperty<RealVectorValue> & _ubPlastic;

  /// Old plastic displacements in basic system
  const MaterialProperty<RealVectorValue> & _ubPlastic_old;

  /// Save old shear forces during the itearion
  RealVectorValue _qbOld;

  /// Trial shear forces of the hysteretic component
  RealVectorValue _qTrial;

  /// Vector to store analysis time at each step
  std::vector<Real> _vec_time;

  /// Vector to store heatflux at each step
  std::vector<Real> _vec_heatflux;
};

#endif // COMPUTEFPISOLATORELASTICITY_H
