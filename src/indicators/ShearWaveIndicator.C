// libMesh includes
#include "libmesh/numeric_vector.h"

// MOOSE includes
#include "Assembly.h"
#include "MooseVariable.h"
#include "SystemBase.h"

// Mastodon includes
#include "ShearWaveIndicator.h"

registerMooseObject("MastodonApp", ShearWaveIndicator);

template <>
InputParameters
validParams<ShearWaveIndicator>()
{
  InputParameters params = validParams<Indicator>();
  params.addParam<MaterialPropertyName>("shear_wave_speed",
                                        "shear_wave_speed",
                                        "The name of the material properties "
                                        "(type Real) that computes the shear "
                                        "wave velocity.");
  params.addRequiredParam<Real>("cutoff_frequency", "The cutoff frequency in Hertz.");
  params.addClassDescription("Computes the minimum element size based on the shear wave speed.");
  return params;
}

ShearWaveIndicator::ShearWaveIndicator(const InputParameters & parameters)
  : Indicator(parameters),
    _shear_wave_speed(getMaterialProperty<Real>("shear_wave_speed")),
    _cutoff_frequency(getParam<Real>("cutoff_frequency")),
    _qrule(_assembly.qRule()),
    _indicator_var(_sys.getFieldVariable<Real>(_tid, name()))
{
}

void
ShearWaveIndicator::computeIndicator()
{
  _minimum_element_size.resize(_qrule->n_points());
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
    _minimum_element_size[_qp] = 1.0 / _cutoff_frequency * _shear_wave_speed[_qp];

  Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
  _solution.set(_indicator_var.nodalDofIndex(),
                *std::min_element(_minimum_element_size.begin(), _minimum_element_size.end()));
}
