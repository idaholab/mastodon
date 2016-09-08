// Mastodon includes
#include "MastodonProblem.h"

template<>
InputParameters validParams<MastodonProblem>()
{
  InputParameters params = validParams<FEProblem>();
  params.addClassDescription("A problem to perform adaptivity until the mesh no longer is changing.");
  return params;
}

MastodonProblem::MastodonProblem(const InputParameters & parameters) :
    FEProblem(parameters)
{
}

void
MastodonProblem::initialAdaptMesh()
{
  // The max number of iterations
  unsigned int n_max = adaptivity().getInitialSteps();
  if (n_max == 0)
  {
    mooseWarning("The initial adaptivity steps was not set, thus no initial adaptivity was performed. To enable Mastodon automatic intial adaptivity set the 'initial_steps' parameter in the Adaptivity block.");
    return;
  }

  // Keep track of the iterations
  unsigned int count = 1;

  // The adaptivity flag
  bool flag = true;

  // Perform the adaptivity
  while (flag && (count <= n_max))
  {
    _console << "Mastodon adapting mesh: " << count << "\n";

    computeIndicators();
    computeMarkers();

    flag = _adaptivity.initialAdaptMesh();
    if (flag)
      meshChanged();

    count++;

    projectSolution();
  }
}
