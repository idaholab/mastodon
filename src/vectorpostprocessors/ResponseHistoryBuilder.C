// MOOSE includes
#include "PostprocessorInterface.h"
#include "ResponseHistoryBuilder.h"

// libMesh includes
#include "libmesh/parallel_algebra.h"

// C++ includes
#include <unistd.h>

registerMooseObject("MastodonApp", ResponseHistoryBuilder);

template <>
InputParameters
validParams<ResponseHistoryBuilder>()
{
  InputParameters params = validParams<NodalVectorPostprocessor>();
  params.addParam<std::vector<dof_id_type>>(
      "nodes", "Node number(s) at which the response history is needed.");

  // Force the object to only execute once per node even if it has multiple boundary ids
  params.set<bool>("unique_node_execute") = true;
  params.suppressParameter<bool>("unique_node_execute");

  params.set<ExecFlagEnum>("execute_on") = {EXEC_INITIAL, EXEC_TIMESTEP_END};
  params.suppressParameter<ExecFlagEnum>("execute_on");

  params.addRequiredCoupledVar("variables",
                               "Variable name for which the response history is requested.");
  params.addClassDescription("Calculates response histories for a given node and variable(s).");
  return params;
}

ResponseHistoryBuilder::ResponseHistoryBuilder(const InputParameters & parameters)
  : NodalVectorPostprocessor(parameters), _history_time(declareVector("time"))
{
  // Retrieving variable names from input
  const std::vector<VariableName> & var_names = getParam<std::vector<VariableName>>("variables");

  // Set that will store the union of node ids from all the boundaries or requested nodes
  std::set<dof_id_type> history_nodes;

  if (parameters.isParamValid("boundary") && parameters.isParamValid("nodes"))
    mooseError("Error in VectorPostprocessor, '",
               name(),
               "'. Please specify either of boundary or node, but not both.");

  if (!parameters.isParamValid("boundary") && !parameters.isParamValid("nodes"))
    mooseError("Error in VectorPostprocessor, '",
               name(),
               "'. Please provide either boundary or node for response history output.");

  if (parameters.isParamValid("boundary"))
  {
    // Retrieving boundaries and making a set of all the nodes in these boundaries
    std::set<BoundaryID> bnd_ids =
        boundaryIDs(); // Set containing the boundary IDs input by the user
    for (auto id : boundaryIDs())
      history_nodes.insert(_mesh.getNodeList(id).begin(), _mesh.getNodeList(id).end());
  }

  if (parameters.isParamValid("nodes"))
  {
    // Retrieving vector of nodes from input and storing them in a set, in case
    // there are repetetions
    std::vector<dof_id_type> vec = getParam<std::vector<dof_id_type>>("nodes");
    history_nodes.insert(vec.begin(), vec.end());
  }

  // Resizing _history to the number of nodes * number of variables
  _history.resize(var_names.size() * history_nodes.size());

  // Declaring _history vectors and creating map from node_id to the location of
  // the corresponding VPP in _history
  std::size_t count = 0;
  for (dof_id_type node_id : history_nodes)
  {
    for (std::size_t i = 0; i < var_names.size(); ++i)
      _history[count * var_names.size() + i] =
          &declareVector("node_" + Moose::stringify(node_id) + "_" + var_names[i]);
    _node_map[node_id] = count;
    count++;
  }

  // Coupling variables
  for (std::size_t i = 0; i < var_names.size(); ++i)
    _variables.push_back(&coupledValue("variables", i));
}

void
ResponseHistoryBuilder::initialize()
{
  _current_data.clear();
  _current_data.resize(_history.size());
}

void
ResponseHistoryBuilder::finalize()
{
  // Data to be added to the current vectors
  std::vector<Real> data(_history.size());

  if (n_processors() > 1)
  {
    // On each processor _current_data is sized for the number of history vectors (N).The allgather
    // puts these vectors together on the root processor in a single vector. Therefore, if there
    // are two processors (A and B) then the _current_data for each processors are:
    //    A = [A_0, A_1, ..., A_N]
    //    B = [B_0, B_1, ..., B_N]
    // After the allgather the _current_data on the root processor becomes:
    //    _current_data = [A_0, A_1, ..., A_N, B_0, B_1, ..., B_N]
    _communicator.allgather(_current_data, true);

    // The values for _current_data are zero everywhere except on the processor on which it was
    // computed and there should never be repeated values because "unique_node_execute" is true and
    // the execute method sets the data by index so any repeated calls would overwrite a previous
    // calculation. Therefore, the data that needs to be added is simply the sum of all the vector
    // across processors.
    const std::size_t N = _history.size();
    for (dof_id_type rank = 0; rank < n_processors(); ++rank)
      for (std::size_t i = 0; i < _history.size(); ++i)
        data[i] += _current_data[rank * N + i];
  }

  else
    data = _current_data;

  // Update the history vectors with the new data
  for (std::size_t i = 0; i < _history.size(); ++i)
    _history[i]->push_back(data[i]);

  // Update the time vector
  _history_time.push_back(_t);
}

void
ResponseHistoryBuilder::threadJoin(const UserObject & uo)
{
  // As detailed in the finalize() method the _current_data are zero everywhere except on the
  // process and thread where it was computed. Thus, adding the values from the other threads
  // updates the root thread correctly.
  const ResponseHistoryBuilder & builder = static_cast<const ResponseHistoryBuilder &>(uo);
  for (std::size_t i = 0; i < _history.size(); ++i)
    _current_data[i] += builder._current_data[i];
}

void
ResponseHistoryBuilder::execute()
{
  // finding location of the vector in _history corresponding to _current_node
  if (_node_map.find(_current_node->id()) != _node_map.end())
  {
    // The index of the data within the _history vector for the current node
    std::size_t loc = _node_map[_current_node->id()];
    for (std::size_t i = 0; i < _variables.size(); ++i)
      _current_data[loc * _variables.size() + i] = (*_variables[i])[0];
  }
}
