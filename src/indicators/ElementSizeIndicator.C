// libMesh includes
#include "libmesh/numeric_vector.h"

// MOOSE includes
#include "Assembly.h"
#include "MooseVariable.h"
#include "SystemBase.h"

// Mastodon includes
#include "ElementSizeIndicator.h"

registerMooseObject("MastodonApp", ElementSizeIndicator);

InputParameters
ElementSizeIndicator::validParams()
{
  InputParameters params = Indicator::validParams();
  params.addRequiredParam<MaterialPropertyName>(
      "element_size",
      "The name of the material that will control the mesh refinement.");
  params.addClassDescription("Computes the element size based on the provided material value.");
  return params;
}

ElementSizeIndicator::ElementSizeIndicator(const InputParameters & parameters)
  : Indicator(parameters),
    // _element_size(getMaterialProperty<Real>("element_size")),
    _element_size(getMaterialProperty<Real>(getParam<MaterialPropertyName>("element_size"))),
    _qrule(_assembly.qRule()),
    _indicator_var(_sys.getFieldVariable<Real>(_tid, name()))
{
}

void
ElementSizeIndicator::computeIndicator()
{
  _minimum_element_size.resize(_qrule->n_points());
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
    _minimum_element_size[_qp] = _element_size[_qp];

  Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
  _solution.set(_indicator_var.nodalDofIndex(),
                *std::min_element(_minimum_element_size.begin(), _minimum_element_size.end()));
}
