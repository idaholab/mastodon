// Mastodon includes
#include "LayerInterfaceIndicator.h"

// libMesh includes
#include "libmesh/quadrature.h"

template<>
InputParameters validParams<LayerInterfaceIndicator>()
{
  InputParameters params = validParams<InternalSideIndicator>();
  params.addClassDescription("Computes the difference in the element layer ids between neighbors and sets the error to 1.0 if different.");
  return params;
}


LayerInterfaceIndicator::LayerInterfaceIndicator(const InputParameters & parameters) :
    InternalSideIndicator(parameters)
{
}


void
LayerInterfaceIndicator::computeIndicator()
{
  Real sum = 0;
  for (_qp=0; _qp<_qrule->n_points(); _qp++)
    sum += std::abs(_u[_qp] - _u_neighbor[_qp]);

  if (sum > 0)
  {
    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    _solution.set(_field_var.nodalDofIndex(), 1.0);
  }
}
