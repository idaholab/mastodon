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

#ifndef COMPUTEELASTICITYISOLATOR_H
#define COMPUTEELASTICITYISOLATOR_H

#include "Material.h"

/**
 * ComputeElasticityIsolator ...................
 */

class ComputeElasticityIsolator;

template <>
InputParameters validParams<ComputeElasticityIsolator>();

class ComputeElasticityIsolator : public Material
{
public:
  ComputeElasticityIsolator(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;
  virtual void initQpStatefulProperties() override;

  /// Material stiffness vector that relates displacement strain increments to force increments
  MaterialProperty<RealVectorValue> & _material_stiffness;

  /// Material flexure vector that relates rotational strain increments to moment increments
  MaterialProperty<RealVectorValue> & _material_flexure;

  /// prefactor function to multiply the material stiffness and flexure vectors with
  Function * const _prefactor_function;

  /// Young's modulus of the beam material
  const VariableValue & _youngs_modulus;

  /// Shear modulus of the beam material
  const VariableValue & _shear_modulus;

  /// Shear coefficient for the beam cross-section
  const VariableValue & _shear_coefficient;
};

#endif // COMPUTEELASTICITYISOLATOR_H
