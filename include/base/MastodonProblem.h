// MOOSE includes
#include "FEProblem.h"

// Forward Declarations
class MastodonProblem;

template<>
InputParameters validParams<MastodonProblem>();

/**
 * Provides auto adapting to the initial adaptivity function. This allows the mesh to refine
 * to a minimum element size.
 */
class MastodonProblem : public FEProblem
{
public:
  MastodonProblem(const InputParameters & parameters);

  /**
   * Run the initial adaptivity until there is nothing remaining to adapt.
   */
  virtual void initialAdaptMesh() override;

};
