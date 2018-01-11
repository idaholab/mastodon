// MOOSE includes
#include "PostprocessorInterface.h"
#include "ResponseHistoryBuilder.h"

// libMesh includes
#include "libmesh/parallel_algebra.h"

// C++ includes
#include <unistd.h>

template <>
InputParameters
validParams<ResponseHistoryBuilder>()
{
  InputParameters params = validParams<NodalVectorPostprocessor>();
  params.addRequiredParam<unsigned int>("node",
                                        "Node number at which the response history is requested.");
  params.set<ExecFlagEnum>("execute_on") = {EXEC_INITIAL, EXEC_TIMESTEP_END};
  params.addRequiredCoupledVar("variables",
                               "Variable name for which the response history is requested.");
  params.addClassDescription("Calculates response histories for a given node and variable(s).");

  return params;
}

ResponseHistoryBuilder::ResponseHistoryBuilder(const InputParameters & parameters)
  : NodalVectorPostprocessor(parameters),
    _history_time(declareVector("_time")),
    _node(getParam<unsigned int>("node"))

{
  const std::vector<VariableName> & names = getParam<std::vector<VariableName>>("variables");
  for (std::size_t i = 0; i < names.size(); ++i)
  {
    _history.push_back(&declareVector(names[i]));
    _variables.push_back(&coupledValue("variables", i));
  }
}

void
ResponseHistoryBuilder::initialize()
{
  _node_rank = DofObject::invalid_processor_id;
}

void
ResponseHistoryBuilder::finalize()
{
  // When running in parallel the data must be transfered from the processor that owns the node that
  // is being utilized to all other processors.
  if (n_processors() > 1)
  {
    // Create a vector for storing the data from the last time execution that should be appended
    // to all vectors on the other processors. This is the storage that will be used with broadcast.
    std::vector<Real> data(_history.size() + 1);

    // On the processor with the data, pack the data to be broadcast
    if (_node_rank == processor_id())
    {
      data[0] = _history_time.back();
      for (size_t i = 0; i < _history.size(); ++i)
        data[i + 1] = _history[i]->back();
    }

    // Communicate which processor owns the data, use int because libMesh specializations do not
    // include bool types.
    std::vector<int> recieve;
    int send = processor_id() == _node_rank;
    _communicator.allgather(send, recieve);

    // Determine the "root id" from which the data is to be broadcast, then broadcast
    int root_id = std::distance(recieve.begin(), std::find(recieve.begin(), recieve.end(), 1));
    _communicator.broadcast(data, root_id);

    // Update the data on processors that don't already have it
    if (_node_rank != processor_id())
    {
      _history_time.push_back(data[0]);
      for (size_t i = 0; i < _history.size(); ++i)
        _history[i]->push_back(data[i + 1]);
    }
  }
}

void
ResponseHistoryBuilder::threadJoin(const UserObject & uo)
{
  const ResponseHistoryBuilder & builder = static_cast<const ResponseHistoryBuilder &>(uo);

  // Prior to the execute() call the _node_rank is set to an invalid value. When execute() occurs
  // data is only added to the vectors for a single node. This happends on one processor and one
  // thread. When the data is added the _node_rank is also set to be the processor id of the node.
  // Therefor, if the object passed in has a valid _node_rank then this is the object that has the
  // most updated data, so add its most recent data to this object. Also, if the node happens to be
  // stored on thread 0 then this loop will never do anything, which is fine, because the purpose
  // of this method is to get the data to the 0 thread copy.
  if (builder._node_rank != DofObject::invalid_processor_id)
  {
    _node_rank = builder._node_rank; // required for MPI communcation in finailize
    _history_time.push_back(builder._history_time.back());
    for (size_t i = 0; i < _history.size(); ++i)
      _history[i]->push_back(builder._history[i]->back());
  }
}

void
ResponseHistoryBuilder::execute()
{
  if (_current_node->id() == _node)
  {
    _node_rank = _current_node->processor_id();
    _history_time.push_back(_t);
    for (size_t i = 0; i < _history.size(); ++i)
      _history[i]->push_back((*_variables[i])[0]);
  }
}
