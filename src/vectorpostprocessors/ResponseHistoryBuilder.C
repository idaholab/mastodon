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
  params.addParam<std::vector<dof_id_type>>("nodes", "Node number(s) at which the response history is needed.");
  params.set<ExecFlagEnum>("execute_on") = {EXEC_INITIAL, EXEC_TIMESTEP_END};
  params.addRequiredCoupledVar("variables", "Variable name for which the response history is requested.");
  params.addClassDescription("Calculates response histories for a given node and variable(s).");
  return params;
}

ResponseHistoryBuilder::ResponseHistoryBuilder(const InputParameters & parameters)
  : NodalVectorPostprocessor(parameters),
    _history_time(declareVector("time"))
{
  // Retrieving variable names from input
  const std::vector<VariableName> & var_names = getParam<std::vector<VariableName>>("variables");

   // Set that will store the union of node ids from all the boundaries or requested nodes
  std::set<dof_id_type> history_nodes;

  if (parameters.isParamValid("boundary") && parameters.isParamValid("nodes"))
    mooseError("Error in VectorPostprocessor, '", name(), "'. Please specify either of boundary or node, but not both.");

  if (!parameters.isParamValid("boundary") && !parameters.isParamValid("nodes"))
    mooseError("Error in VectorPostprocessor, '", name(), "'. Please provide either boundary or node for response history output.");

  if (parameters.isParamValid("boundary"))
  {
    // Retrieving boundaries and making a set of all the nodes in these boundaries
    std::set<BoundaryID> bnd_ids = boundaryIDs(); // Set containing the boundary IDs input by the user
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
      _history[count * var_names.size() + i] = &declareVector("node_" + Moose::stringify(node_id) + "_" + var_names[i]);
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
  std::cout << "*********** Executing initialize\n";
  _node_rank = DofObject::invalid_processor_id;
}

void
ResponseHistoryBuilder::finalize()
{
  // When running in parallel the data must be transferred from the processor that owns the node that
  // is being utilized to all other processors.
  if (n_processors() > 1)
  {
    // Create a vector for storing the data from the last time execution that should be appended
    // to all vectors on the other processors. This is the storage that will be used with broadcast.
    std::vector<Real> data(_history.size() + 1);
    // std::cout << "DATASIZE\t" << data.size() << std::endl;
    // On the processor with the data, pack the data to be broadcast
    if (_node_rank == processor_id())
    {
      std::cout << "*********** Executing finalize 1 \n";
      data[0] = _history_time.back();
      std::cout << "*********** Executing finalize 2 \n";
      for (size_t i = 0; i < _history.size(); ++i)
        {
          std::cout << "*********** Executing finalize 3 \n";
          std::cout << "HISTORY is: \n" << Moose::stringify(*_history[i]) << std::endl;
          data[i + 1] = _history[i]->back();
          std::cout << "*********** Executing finalize 4 \n";
        }
    }

    // Communicate which processor owns the data, use int because libMesh specializations do not
    // include bool types.
    std::vector<int> recieve;
    int send = processor_id() == _node_rank;
    std::cout << "*********** Executing finalize 4.5 \n";
    _communicator.allgather(send, recieve);
    std::cout << "*********** Executing finalize 5 \n";

    // Determine the "root id" from which the data is to be broadcast, then broadcast
    int root_id = std::distance(recieve.begin(), std::find(recieve.begin(), recieve.end(), 1));
    _communicator.broadcast(data, root_id);
    std::cout << "*********** Executing finalize 6 \n";

    // Update the data on processors that don't already have it
    if (_node_rank != processor_id())
    {
      _history_time.push_back(data[0]);
      std::cout << "*********** Executing finalize 7 \n";
      for (size_t i = 0; i < _history.size(); ++i)
        _history[i]->push_back(data[i + 1]);
    }
    std::cout << "*********** Executing finalize 8 \n";
  }
}

void
ResponseHistoryBuilder::threadJoin(const UserObject & uo)
{
  const ResponseHistoryBuilder & builder = static_cast<const ResponseHistoryBuilder &>(uo);

  // Prior to the execute() call the _node_rank is set to an invalid value. When execute() occurs
  // data is only added to the vectors for a single node. This happens on one processor and one
  // thread. When the data is added the _node_rank is also set to be the processor id of the node.
  // Therefore, if the object passed in has a valid _node_rank then this is the object that has the
  // most updated data, so add its most recent data to this object. Also, if the node happens to be
  // stored on thread 0 then this loop will never do anything, which is fine, because the purpose
  // of this method is to get the data to the 0 thread copy.
  if (builder._node_rank != DofObject::invalid_processor_id)
  {
    _node_rank = builder._node_rank; // required for MPI communcation in finalize
    _history_time.push_back(builder._history_time.back());
    for (size_t i = 0; i < _history.size(); ++i)
      _history[i]->push_back(builder._history[i]->back());
  }
}

void
ResponseHistoryBuilder::execute()
{
  std::cout << "EXECUTING CURRENT NODE !!!!!!!\t" << _current_node->id() << std::endl;
  _node_rank = _current_node->processor_id();

  // building time vector. Need to do this only once per timestep
  _history_time.resize(_t_step + 1);
  _history_time[_t_step] = _t;

  // finding location of the vector in _history corresponding to _current_node
  if (_node_map.find(_current_node->id()) != _node_map.end())
  {
    // std::cout << "*****************************\n";
    std::size_t loc = _node_map[_current_node->id()];
    for (std::size_t i = 0; i < _variables.size(); ++i)
    {
      // Resizing corresponding VPP value to current number of time num_steps
      // Doing this to avoid the usage of pushback() because execute() may
      // go through the same node more than once.
      _history[loc * _variables.size() + i]->resize(_t_step + 1);
      // Assigning the value to the vector, instead of using pushback()
      (*_history[loc * _variables.size() + i])[_t_step] = (*_variables[i])[0];
    }
  }
  std::cout << "HISTORY IN EXECUTE is: \n" << Moose::stringify(*_history[0]) << std::endl;
}
