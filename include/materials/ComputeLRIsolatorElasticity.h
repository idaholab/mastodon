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

#ifndef COMPUTELRISOLATORELASTICITY_H
#define COMPUTELRISOLATORELASTICITY_H

#include "Material.h"
#include "ColumnMajorMatrix.h"

/**
 * ComputeLRIsolatorElasticity calculates the forces and stiffness matrix
 * of an LR isolator element.
 */

// Forward Declarations
class ComputeLRIsolatorElasticity;

template <>
InputParameters validParams<ComputeLRIsolatorElasticity>();

class ComputeLRIsolatorElasticity : public Material
{
public:
  ComputeLRIsolatorElasticity(const InputParameters & parameters);

protected:
  /// Compute stiffness matrix and force in the basic system
  virtual void computeQpProperties() override;

  void initQpStatefulProperties() override;

  /// Initilize properties
  void initializeLRIsolator();

  /// Computes the forces and stiffness matrix elements in the axial direction
  void computeAxial();

  /// Computes the forces and stiffness matrix elements in shear
  void computeShear();

  /// Computes the current temperature in the lead core
  Real calculateCurrentTemperature(Real _qYield, Real _TLC, Real vel);

  /// Changes the stiffness matrix in the local coordinate system by adding
  /// P-Delta effects
  void addPDeltaEffects();

  /// Finalize the forces and stiffnesses and convert into global co-ordinate system
  void finalize();

  /// Switch for modeling cavitation and post-cavitation effects
  const bool & _cavitation;

  /// Switch for simulating buckling load variation during the analysis
  const bool & _buckling_load_variation;

  /// Switch for modeling horizontal stiffness variation during the analysis
  const bool & _horizontal_stiffness_variation;

  /// Switch for modeling vertical stiffness variation during the analysis
  const bool & _vertical_stiffness_variation;

  /// Switch for modeling strength degradation during the analysis
  const bool & _strength_degradation;

  //
  // Isolator material, response and geomtric parameters
  //
  /// Yield stress of the bearing
  const Real & _fy;

  /// Yield displacement of the bearing
  const Real & _alpha;

  /// Shear modulus of rubber
  const Real & _Gr;

  /// Bulk modulus of rubber
  const Real & _Kr;

  /// Diameter of the lead core
  const Real & _d1;

  /// Outer diameter of the bearing
  const Real & _d2;

  /// Thickness of each steel shim
  const Real & _ts;

  /// Thickness of each rubber layer
  const Real & _tr;

  /// Total number of rubber layers
  const Real & _n;

  /// Thickness of rubber cover
  const Real & _tc;

  /// Gamma parameter of Newmark algorithm
  const Real & _gamma;

  /// Beta parameter of Newmark algorithm
  const Real & _beta;

  /// Mass density of lead
  const Real & _rhoL;

  /// Specific heat capacity of lead
  const Real & _cL;

  /// Thermal conductivity of steel
  const Real & _kS;

  /// Thermal diffusivity of steel
  const Real & _aS;

  /// Cavitation parameter
  const Real & _kc;

  /// Damage index
  const Real & _phi_m;

  /// Strength degradation parameter
  const Real & _ac;

  /// Viscous damping parameter
  const Real & _cd;

  /// Shear distance ratio
  Real _sD;

  /// Isolator deformations in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_def;

  /// Isolator old deformations in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_def_old;

  /// Isolator deformation rates in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_vel;

  /// Isolator old deformations rates in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_vel_old;

  /// Isolator accelerations in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_accel;

  /// Isolator old accelerations in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _basic_accel_old;

  /// Isolator forces (including moments) in the basic system
  MaterialProperty<ColumnMajorMatrix> & _Fb;

  /// Isolator old forces (including moments) in the basic system
  const MaterialProperty<ColumnMajorMatrix> & _Fb_old;

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

  /// Bonded rubber area of the bearing
  Real _A;

  /// Total rubber thickness
  Real _Tr;

  /// Total thickness of rubber and steel shims
  Real _h;

  /// Radius of gyration
  Real _rg;

  //
  // Bearing shear parameters
  //
  /// Initial yield stress of the bearing
  Real _qYield0;

  /// Instantaneous yield stress of the bearing
  Real _qYield;

  /// Elastic shear stiffness of bearing (from rubber)
  Real _ke;

  /// Initial hysteretic shear stiffness of the bearing (from lead)
  Real _k0;

  //
  // Bearing axial parameters: compression
  //
  /// Pre-cavitation stiffness of the bearing at zero lateral displacement
  Real _kv0;

  /// Adjusted pre-cavitation stiffness during the analysis
  Real _kv;

  /// Initial Critical buckling load in compression
  Real _Fcr;

  /// Critical buckling load (current)
  Real _Fcrn;

  /// Initial value of deformation corresponding to critical buckling load
  Real _ucr;

  /// Current value of deformation corresponding to critical buckling load
  Real _ucrn;

  //
  // Bearing axial parameters: tension
  //
  /// Initial value of tensile force that initiates cavitation
  Real _Fc;

  /// Current value of tensile force that initiates cavitation
  Real _Fcn;

  /// Initial value of deformation at which, cavitation is initiated
  Real _uc;

  /// Maximum tensile force (will be updated each time step)
  Real _Fmax;

  /// Trial lead temperature
  Real _TL_trial;

  /// Committed lead temperature
  Real _TLC;

  /// Hysteresis parameters
  MaterialProperty<ColumnMajorMatrix> & _z;

  /// old hysteresis parameters
  const MaterialProperty<ColumnMajorMatrix> & _z_old;

  /// Maximum tensile deformation (will be updated each time step)
  MaterialProperty<ColumnMajorMatrix> & _umax;

  /// Maximum tensile deformation in the previous step
  const MaterialProperty<ColumnMajorMatrix> & _umax_old;

  /// current value of deformation that initiates cavitation (will be updated each time step)
  MaterialProperty<ColumnMajorMatrix> & _ucn;

  /// current value of deformation that initiates cavitation in previous step (will be updated each time step)
  const MaterialProperty<ColumnMajorMatrix> & _ucn_old;

  /// Initial value of critical buckling load during loading
  MaterialProperty<ColumnMajorMatrix> & _Fcrmin;

  /// Initial value of critical buckling load during loading in previous step
  const MaterialProperty<ColumnMajorMatrix> & _Fcrmin_old;

  /// Tangent matrix of hysteretic evolution parameters
  ColumnMajorMatrix _dzdu;

  /// Committed displacements in the basic system
  ColumnMajorMatrix _ubC;

  /// Time at which variables are committed
  Real _tC;
};

#endif // COMPUTELRISOLATORELASTICITY_H
