#include "FaultTreeUtils.h"
#include "MastodonUtils.h"

typedef FTAUtils::FaultTree::_node ft_node;

FTAUtils::FaultTree::FaultTree(std::string file_name, std::string root)
{
  FTAUtils::Parser parser = FTAUtils::Parser(file_name, FTAUtils::Parser::FORMAT_CSV);
  buildTree(parser);

  // Override root node if not default
  if (root != "")
  {
    // ASSERT
    if (!(getNode(root)))
    {
      fprintf(stderr,
              "[ASSERT] In File: %s, Line: %d => Root node "
              "requested (%s) not found in heirarchy. "
              "Please the check file %s.\n",
              __FILE__,
              __LINE__,
              root.c_str(),
              file_name.c_str());
      abort();
    }

    this->_root = root;
  }

  computeMinimumCutSets();
}

FTAUtils::FaultTree::FaultTree(set_string & sets_link, std::map<std::string, ft_node *> _node_base)
{
  std::string root = "";
  _node_d_b = _node_base;

  // Clearing up sets vector for safety
  rmSets();

  // Add root to set
  std::set<std::string> root_set, sub_sets;
  root_set.insert("TOP"); // "TOP", size: 1

  _sets.insert(root_set);

  // Start with root node to expand on heirarchy
  cutSetsExpand(getNode("TOP"));

  // Check if first row is empty
  // NOTE: Since its std::set, only first row could
  // be empty
  if (_sets.begin()->size() == 0)
  {
    _sets.erase(_sets.begin());
  }

  removeSubsets();

  sets_link = _sets;
}

FTAUtils::FaultTree::~FaultTree() {}

std::map<std::string, ft_node *>
FTAUtils::FaultTree::buildTree(FTAUtils::Parser parser)
{
  std::vector<std::string> line;
  while (true)
  {
    line = parser.yieldLine();

    // Stop if no new line to process
    if (line.size() == 0)
      break;

    // Stash name, operator
    _node * node = new _node(line[0], str2Operator(line[1]));

    // Add children
    for (int i = 2; i < line.size(); i++)
      node->_child.push_back(line[i]);

    // Stash the first entry as ROOT of tree
    if (_node_d_b.size() == 0)
      _root = line[0];

    // Add the newly created node to node lookup hashmap
    _node_d_b[line[0]] = node;
  }

  return _node_d_b;
}

FTAUtils::FaultTree::_operator_t
FTAUtils::FaultTree::str2Operator(std::string op)
{
  std::string op_s = FTAUtils::str2Upper(op, true);
  // ASSERT
  if (_opDict.count(op_s) == 0)
  {
    fprintf(stderr,
            "[ASSERT] In File: %s, Line: %d => "
            "Illegal Operator found: %s.\n",
            __FILE__,
            __LINE__,
            op.c_str());
    abort();
  }
  return _opDict[op_s];
}

set_string
FTAUtils::FaultTree::computeMinimumCutSets()
{
  // Clearing up sets vector for safety
  rmSets();

  // Add root to set
  std::set<std::string> root_set, sub_sets;
  root_set.insert(getRoot());

  _sets.insert(root_set);

  // Start with root node to expand on heirarchy
  cutSetsExpand(getNode(getRoot()));

  // Check if first row is empty
  // NOTE: Since its std::set, only first row could
  // be empty
  if (_sets.begin()->size() == 0)
  {
    _sets.erase(_sets.begin());
  }

  removeSubsets();

  return _sets;
}

void
FTAUtils::FaultTree::removeSubsets()
{
  std::set<uint64_t> rm_its;

  uint64_t index1 = 0;
  for (set_string::iterator row1_it = _sets.begin(); row1_it != _sets.end(); ++row1_it)
  {
    uint64_t index2 = index1 + 1;
    for (set_string::iterator row2_it = next(row1_it, 1); row2_it != _sets.end(); ++row2_it)
    {
      bool row1_is_subset =
          includes(row2_it->begin(), row2_it->end(), row1_it->begin(), row1_it->end());
      bool row2_is_subset =
          includes(row1_it->begin(), row1_it->end(), row2_it->begin(), row2_it->end());
      // Remove row1 if row2 is a subset of row1 by marking it
      if (row2_is_subset)
        rm_its.insert(index1);
      // Remove row2 if row1 is a subset of row1 by marking it
      else if (row1_is_subset)
        rm_its.insert(index2);
      index2++;
    }
    index1++;
  }

  uint64_t index = 0;
  // Remove rows marked to be removed
  for (std::set<uint64_t>::iterator it = rm_its.begin(); it != rm_its.end(); ++it)
  {
    _sets.erase(next(_sets.begin(), *it - index));
    index++;
  }
}

void
FTAUtils::FaultTree::cutSetsExpand(_node * node)
{
  // ASSERT
  if (!node)
  {
    fprintf(stderr,
            "[ASSERT] In File: %s, Line: %d => "
            "NULL node encountered.\n",
            __FILE__,
            __LINE__);
    abort();
  }

  // ASSERT
  if (node->_child.size() == 0)
  {
    fprintf(stderr,
            "[ASSERT] In File: %s, Line: %d => "
            "Empty child list found for node '%s'.\n",
            __FILE__,
            __LINE__,
            node->_name.c_str());
    abort();
  }

  // New rows which have to be appended at end. Adding them might
  // result in data hazards
  set_string mk_rows;

  // 1. Iterate through entire list and match for own node's name
  for (set_string::iterator row_it = _sets.begin(); row_it != _sets.end(); ++row_it)
  {
    std::set<std::string> row(*row_it);
    // Search for the element in row
    std::set<std::string>::iterator it = find(row.begin(), row.end(), node->_name);
    if (it != row.end())
    {
      // Erase self to add children
      row.erase(it);

      // 2. Replace self with children based on operation
      //    (i) . Replace self with children in same row if AND
      //    (ii). Replace self with child one per row if OR
      switch (node->_op)
      {
        case AND:
          // Append all children in same row
          for (uint64_t c_id = 0; c_id < node->_child.size(); c_id++)
          {
            row.insert(node->_child[c_id]);
          }
          break;

        case OR:
          // Replicate line and append one child per row
          for (uint64_t c_id = 0; c_id < node->_child.size(); c_id++)
          {
            std::set<std::string> row_set(row);
            row_set.insert(node->_child[c_id]);
            mk_rows.insert(row_set);
          }

          // Clear this row to be removed at end (postprocessing)
          row.clear();
          break;

        default:
        {
          // ASSERT
          fprintf(stderr,
                  "[ASSERT] In File: %s, Line: %d => "
                  "Unknown Operator found!.\n",
                  __FILE__,
                  __LINE__);
          abort();
        }
        break;
      }
    }

    // The following 2 lines might result in an empty row to be pushed.
    // If we gate insertion with count, we might have a data hazard due
    // to wrong increment of iterators.
    // A quick fix to this is to check for first row and delete it at end
    _sets.erase(row_it);
    _sets.insert(row);
  }

  // Add newly created rows
  for (set_string::iterator it = mk_rows.begin(); it != mk_rows.end(); ++it)
  {
    std::set<std::string> row(*it);
    _sets.insert(*it);
  }

  // 3. Recurse on each non leaf child
  for (uint64_t c_id = 0; c_id < node->_child.size(); c_id++)
  {
    _node * child = getNode(node->_child[c_id]);
    if (child)
      cutSetsExpand(child);
  }
}

void
FTAUtils::FaultTree::rmSets()
{
  for (set_string::iterator row_it = _sets.begin(); row_it != _sets.end(); ++row_it)
  {
    _sets.erase(row_it);
  }
}

ft_node *
FTAUtils::FaultTree::getNode(std::string name)
{
  return (_node_d_b.count(name) != 0) ? _node_d_b[name] : NULL;
}

std::string
FTAUtils::FaultTree::getRoot()
{
  return _root;
}

set_string
FTAUtils::FaultTree::getCutSets()
{
  return _sets;
}

FTAUtils::Parser::Parser(std::string fileName, FTAUtils::Parser::parseFormatT format)
{
  // Assertion on supported parsing formats
  if (format != FORMAT_CSV)
  {
    fprintf(stderr,
            "[ASSERT] In File: %s, Line: %d => "
            "Unsupported parse format.\n",
            __FILE__,
            __LINE__);
    abort();
  }

  fileP = new std::ifstream;
  fileP->open(fileName, std::ifstream::in);

  if (!fileP->is_open())
    throw FTAUtils::CException("Unable to open file.");
}

FTAUtils::Parser::~Parser() {}

vector_string
FTAUtils::Parser::yieldLines()
{
  vector_string lines;
  std::vector<std::string> line;
  while (true)
  {
    line = FTAUtils::Parser::yieldLine();

    // Stop if no new line to process
    if (line.size() == 0)
      break;
    lines.push_back(line);
  }
  return lines;
}

std::vector<std::string>
FTAUtils::Parser::yieldLine()
{
  // ASSERT
  if (!(fileP->is_open()))
  {
    fprintf(stderr,
            "[ASSERT] In File: %s, Line: %d => "
            "Unsupported parse format.\n",
            __FILE__,
            __LINE__);
    abort();
  }

  std::string buffer;
  std::vector<std::string> line;

  // Get a line that has something except \n
  if (getline(*fileP, buffer))
  {
    std::string token;
    std::istringstream iss(buffer);
    while (getline(iss, token, ','))
    {
      line.push_back(FTAUtils::trim(token));
    }
  }
  return line;
}

std::string
FTAUtils::trim(const std::string & str)
{
  std::string whiteSpace = " \t\n";

  size_t first = str.find_first_not_of(whiteSpace);
  if (std::string::npos == first)
  {
    return str;
  }
  size_t last = str.find_last_not_of(whiteSpace);
  return str.substr(first, (last - first + 1));
}

std::string
FTAUtils::str2Upper(const std::string & str_in, bool trim_input)
{
  std::string str = str_in;

  if (trim_input)
    str = trim(str_in);

  transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

double
FTAUtils::interpolate(vector_double data, double x, bool extrapolate)
{
  int size = data.size();

  // find left end of interval for interpolation
  int i = 0;

  // special case: beyond right end
  if (x >= data[size - 2][0])
  {
    i = size - 2;
  }
  else
  {
    while (x > data[i + 1][0])
      i++;
  }

  // points on either side (unless beyond ends)
  double xL = data[i][0], yL = data[i][1], xR = data[i + 1][0], yR = data[i + 1][1];

  // if beyond ends of array and not extrapolating
  if (!extrapolate)
  {
    if (x < xL)
      yR = yL;
    if (x > xR)
      yL = yR;
  }

  // gradient
  double dydx = (yR - yL) / (xR - xL);

  // linear interpolation
  return yL + dydx * (x - xL);
}

double
FTAUtils::normalCDF(double x)
{
  return std::erfc(-x / std::sqrt(2)) / 2;
}

double
FTAUtils::normalCDF(double x, double mu, double sigma)
{
  double y;

  y = (x - mu) / sigma;

  return FTAUtils::normalCDF(y);
}
