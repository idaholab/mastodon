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

#include "PostprocessorInterface.h"
#include "ResponseHistoryBuilder.h"

template <>
InputParameters
validParams<ResponseHistoryBuilder>()
{
  InputParameters params = validParams<NodalVectorPostprocessor>();
  params.addRequiredParam<unsigned int>("node",
                                        "Node number at which the response history is requested.");
  params.addRequiredCoupledVar("variables",
                               "Variable name for which the response history is requested.");
  params.addClassDescription("Calculates response histories for a given node and variable(s).");

  return params;
}

ResponseHistoryBuilder::ResponseHistoryBuilder(const InputParameters & parameters)
  : NodalVectorPostprocessor(parameters),
    _history_time(declareVector("00_time")),
    _cur_time(std::numeric_limits<Real>::min()),
    _node(getParam<unsigned int>("node"))

{
  const std::vector<VariableName> & names = getParam<std::vector<VariableName>>("variables");
  for (unsigned int i = 0; i < names.size(); ++i)
  {
    _history.push_back(&declareVector(names[i]));
    _variables.push_back(&coupledValue("variables", i));
  }
}

void
ResponseHistoryBuilder::initialize()
{
}

void
ResponseHistoryBuilder::threadJoin(const UserObject & uo)
{
}

void
ResponseHistoryBuilder::execute()
{
  if (_t != _cur_time)
  {
    _history_time.push_back(_t);
    _cur_time = _t;
  }
  if (_current_node->id() == _node)
  {
    for (unsigned int i = 0; i < _history.size(); ++i)
      _history[i]->push_back((*_variables[i])[0]);
  }
}
