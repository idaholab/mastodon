// libMesh includes
#include "libmesh/numeric_vector.h"

// Mastodon includes
#include "LayerInterfaceIndicator.h"

// libMesh includes
#include "libmesh/quadrature.h"

#include "MooseVariable.h"

template <>
InputParameters
validParams<LayerInterfaceIndicator>()
{
  InputParameters params = validParams<InternalSideIndicator>();
  params.addParam<Real>("tolerance",
                        1e-10,
                        "Differences in the layer_id between elements above "
                        "this tolerance will be marked for refinement.");
  params.addClassDescription("Computes the difference in the element layer ids "
                             "between neighbors and sets the error to 1.0 if "
                             "different.");
  return params;
}

LayerInterfaceIndicator::LayerInterfaceIndicator(const InputParameters & parameters)
  : InternalSideIndicator(parameters), _tolerance(getParam<Real>("tolerance"))
{
}

void
LayerInterfaceIndicator::computeIndicator()
{
  if ((std::abs(_u[0] - _u_neighbor[0])) > _tolerance)
  {
    Threads::spin_mutex::scoped_lock lock(Threads::spin_mtx);
    _solution.set(_field_var.nodalDofIndex(), 1.0);
  }
}
