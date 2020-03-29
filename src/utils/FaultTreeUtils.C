#include "FaultTreeUtils.h"
#include "MastodonUtils.h"

typedef FTAUtils::FaultTree::_node ft_node;

FTAUtils::FaultTree::FaultTree(std::string file_name, std::string root)
{
  FTAUtils::Parser parser = FTAUtils::Parser(file_name, FTAUtils::Parser::FORMAT_CSV);
  buildTree(parser);

  if (root != "")
  {
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

  rmSets();

  std::set<std::string> root_set, sub_sets;
  root_set.insert("TOP");

  _sets.insert(root_set);

  cutSetsExpand(getNode("TOP"));


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

    if (line.size() == 0)
      break;

    _node * node = new _node(line[0], str2Operator(line[1]));

    for (int i = 2; i < line.size(); i++)
      node->_child.push_back(line[i]);

    if (_node_d_b.size() == 0)
      _root = line[0];

    _node_d_b[line[0]] = node;
  }

  return _node_d_b;
}

FTAUtils::FaultTree::_operator_t
FTAUtils::FaultTree::str2Operator(std::string op)
{
  std::string op_s = FTAUtils::str2Upper(op, true);
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
  rmSets();

  std::set<std::string> root_set, sub_sets;
  root_set.insert(getRoot());

  _sets.insert(root_set);

  cutSetsExpand(getNode(getRoot()));


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
      if (row2_is_subset)
        rm_its.insert(index1);
      else if (row1_is_subset)
        rm_its.insert(index2);
      index2++;
    }
    index1++;
  }

  uint64_t index = 0;
  for (std::set<uint64_t>::iterator it = rm_its.begin(); it != rm_its.end(); ++it)
  {
    _sets.erase(next(_sets.begin(), *it - index));
    index++;
  }
}

void
FTAUtils::FaultTree::cutSetsExpand(_node * node)
{
  if (!node)
  {
    fprintf(stderr,
            "[ASSERT] In File: %s, Line: %d => "
            "NULL node encountered.\n",
            __FILE__,
            __LINE__);
    abort();
  }

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


  set_string mk_rows;

  for (set_string::iterator row_it = _sets.begin(); row_it != _sets.end(); ++row_it)
  {
    std::set<std::string> row(*row_it);
    std::set<std::string>::iterator it = find(row.begin(), row.end(), node->_name);
    if (it != row.end())
    {
      row.erase(it);


      switch (node->_op)
      {
        case AND:
          for (uint64_t c_id = 0; c_id < node->_child.size(); c_id++)
          {
            row.insert(node->_child[c_id]);
          }
          break;

        case OR:
          for (uint64_t c_id = 0; c_id < node->_child.size(); c_id++)
          {
            std::set<std::string> row_set(row);
            row_set.insert(node->_child[c_id]);
            mk_rows.insert(row_set);
          }

          row.clear();
          break;

        default:
        {
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


    _sets.erase(row_it);
    _sets.insert(row);
  }

  for (set_string::iterator it = mk_rows.begin(); it != mk_rows.end(); ++it)
  {
    std::set<std::string> row(*it);
    _sets.insert(*it);
  }

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

    if (line.size() == 0)
      break;
    lines.push_back(line);
  }
  return lines;
}

std::vector<std::string>
FTAUtils::Parser::yieldLine()
{
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

  int i = 0;

  if (x >= data[size - 2][0])
  {
    i = size - 2;
  }
  else
  {
    while (x > data[i + 1][0])
      i++;
  }

  double xL = data[i][0], yL = data[i][1], xR = data[i + 1][0], yR = data[i + 1][1];

  if (!extrapolate)
  {
    if (x < xL)
      yR = yL;
    if (x > xR)
      yL = yR;
  }

  double dydx = (yR - yL) / (xR - xL);

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
