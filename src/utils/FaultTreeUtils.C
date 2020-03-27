#include "FaultTreeUtils.h"
#include "MastodonUtils.h"
// #include "DelimitedFileReader.h"

/*
 * Constructor for fault tree class
 * NOTE: Default value of root is the first node in file
 */
/*!public*/
FTAUtils::FaultTree::FaultTree(std::string file_name, std::string root)
/*!endpublic*/
{
  FTAUtils::Parser parser = FTAUtils::Parser(file_name, FTAUtils::Parser::FORMAT_CSV);
  buildTree(parser);

  // Override root node if not default
  if (root != "")
  {
    ASSERT(getNode(root),
           "Root node requested (%s) not found in heirarchy. Please the check "
           "file %s",
           root.c_str(),
           file_name.c_str());
    this->_root = root;
  }

  computeMinimumCutSets();
}

/*!public*/
FTAUtils::FaultTree::FaultTree(std::set<std::set<std::string>> & sets_link,
                               std::map<std::string, FTAUtils::FaultTree::_node *> _node_base)
/*!endpublic*/
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

/*
 * Destructor
 */
/*!public*/
FTAUtils::FaultTree::~FaultTree()
/*!endpublic*/ {}

/*
 * Builds m-ary fault tree
 */
/*!public*/
std::map<std::string, FTAUtils::FaultTree::_node *>
FTAUtils::FaultTree::buildTree(FTAUtils::Parser parser)
/*!endpublic*/
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

/*
 * Translates string to opeartor
 */
/*!private*/
FTAUtils::FaultTree::_operator_t
FTAUtils::FaultTree::str2Operator(std::string op)
/*!endprivate*/
{
  std::string op_s = FTAUtils::str2Upper(op, true);
  ASSERT(_opDict.count(op_s) != 0, "Illegal Operator found: %s", op.c_str());

  return _opDict[op_s];
}

/*
 * Computes minimum cut sets based on MOCUS Algorithm
 */
/*!public*/
std::set<std::set<std::string>>
FTAUtils::FaultTree::computeMinimumCutSets()
/*!endpublic*/
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

/*!private*/
void
FTAUtils::FaultTree::removeSubsets()
/*!endprivate*/
{
  std::set<uint64_t> rm_its;

  uint64_t index1 = 0;
  for (std::set<std::set<std::string>>::iterator row1_it = _sets.begin(); row1_it != _sets.end();
       ++row1_it)
  {
    uint64_t index2 = index1 + 1;
    for (std::set<std::set<std::string>>::iterator row2_it = next(row1_it, 1);
         row2_it != _sets.end();
         ++row2_it)
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

/*
 * Recursive call function to flood fill sets by expanding them based on
 * operation ALGO:
 *       1. Iterate through entire list and match for own node's name
 *       2. Replace self with children based on operation
 *          (i) . Replace self with children in same row if AND
 *          (ii). Replace self with child one per row if OR
 *       3. Recurse on each non leaf child
 *
 * NOTE: 1. Updates "sets" variable
 *       2. Uses std::set 2d array, hence absorption and idempotence properties
 *          are implicit
 */
/*!private*/
void
FTAUtils::FaultTree::cutSetsExpand(_node * node)
/*!endprivate*/
{
  ASSERT(node, "NULL node encountered");
  ASSERT(node->_child.size() != 0, "Empty child list found for node '%s'", node->_name.c_str());

  // New rows which have to be appended at end. Adding them might
  // result in data hazards
  std::set<std::set<std::string>> mk_rows;

  // 1. Iterate through entire list and match for own node's name
  for (std::set<std::set<std::string>>::iterator row_it = _sets.begin(); row_it != _sets.end();
       ++row_it)
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
          ASSERT(false, "Unknown Operator found!");
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
  for (std::set<std::set<std::string>>::iterator it = mk_rows.begin(); it != mk_rows.end(); ++it)
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

/*!private*/
void
FTAUtils::FaultTree::rmSets()
/*!endprivate*/
{
  for (std::set<std::set<std::string>>::iterator row_it = _sets.begin(); row_it != _sets.end();
       ++row_it)
  {
    _sets.erase(row_it);
  }
}

/*!private*/
FTAUtils::FaultTree::_node *
FTAUtils::FaultTree::getNode(std::string name)
/*!endprivate*/ { return (_node_d_b.count(name) != 0) ? _node_d_b[name] : NULL; }

/*!public*/
std::string
FTAUtils::FaultTree::getRoot()
/*!endpublic*/ { return _root; }

/*!public*/
// void FTAUtils::FaultTree::printSets(std::set<std::set<std::string>> sets)
/*!endpublic*/
/*
{
  std::cout << "------------- SETS BEGIN ----------------- " << std::endl;
  for (std::set<std::set<std::string>>::iterator row = sets.begin(); row != sets.end(); ++row)
  {
    printRow(*row);
    std::cout << std::endl;
  }
  std::cout << "-------------- SETS END ------------------ " << std::endl;
}
*/

/*!public*/
// void
// FTAUtils::FaultTree::printSets()
/*!endpublic*/ 
// { printSets(_sets); }

/*!public*/
// void
// FTAUtils::FaultTree::printRow(std::set<std::string> row)
/*!endpublic*/
/*
{
  for (std::set<std::string>::iterator col = row.begin(); col != row.end(); ++col)
  {
    std::cout << *col << ", ";
  }
}
*/

/*
 * Function returns cut sets at the given point
 * NOTE: If MOCUS ran before this function call, min cut sets will be returned
 */
/*!public*/
std::set<std::set<std::string>>
FTAUtils::FaultTree::getCutSets()
/*!endpublic*/ { return _sets; }

// **************************Parser Definition**************************
/*
 * Constructor for parser class
 */
/*!public*/
FTAUtils::Parser::Parser(std::string fileName, FTAUtils::Parser::parseFormatT format)
/*!endpublic*/
{
  // Assertion on supported parsing formats
  ASSERT(format == FORMAT_CSV, "Unsupported parse format");
  fileP = new std::ifstream;
  fileP->open(fileName, std::ifstream::in);
  // ASSERT( fileP->is_open(), "Unable to open file: %s", fileName.c_str());

  if (!fileP->is_open())
    throw FTAUtils::CException("Unable to open file.");
}

/*
 * Destructor for parser class
 */
/*!public*/
FTAUtils::Parser::~Parser()
/*!endpublic*/ {}

/*
 * Yields all records, populates the standard structure and returns
 * This function acts as an abstract layer to hide different formats
 * that might be supported in future
 *
 * Returns: Array of strings
 */
/*!public*/
std::vector<std::vector<std::string>>
FTAUtils::Parser::yieldLines()
/*!endpublic*/
{
  std::vector<std::vector<std::string>> lines;
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

/*
 * Yields a single record, populates the standard structure and returns
 * This function acts as an abstract layer to hide different formats
 * that might be supported in future
 *
 * Returns: Array of strings
 */
/*!public*/
std::vector<std::string>
FTAUtils::Parser::yieldLine()
/*!endpublic*/
{
  ASSERT(fileP->is_open(), "Illegal call to yieldLine. File not open yet!");

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

/*
 * String trim method which removes all leading and lagging whitespace in a string
 */
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

// defined in Utils.cpp in FTA code

/*
 * Converts ASCII string to upper case
 */
std::string
FTAUtils::str2Upper(const std::string & str_in, bool trim_input)
{
  std::string str = str_in;

  if (trim_input)
    str = trim(str_in);

  transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

/*
 * Returns interpolated value at x from parallel arrays ( xData, yData )
 * Assumes that xData has at least two elements, is sorted and is strictly monotonic increasing
 * boolean argument extrapolate determines behaviour beyond ends of array (if needed)
 */
double
FTAUtils::interpolate(std::vector<std::vector<double>> data, double x, bool extrapolate)
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

// Phi(-∞, x) aka N(x)
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
