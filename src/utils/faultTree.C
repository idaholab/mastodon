#include "faultTree.h"

/*
 * Constructor for fault tree class
 * NOTE: Default value of root is the first node in file
 */
/*!public*/
FaultTree::FaultTree(string file_name, string root)
/*!endpublic*/
{
  ns::Parser parser = ns::Parser(file_name, ns::Parser::FORMAT_CSV);
  buildTree(parser);

  // Override root node if not default
  if (root != "") {
    ASSERT(getNode(root),
           "Root node requested (%s) not found in heirarchy. Please the check "
           "file %s",
           root.c_str(), file_name.c_str());
    this->_root = root;
  }

  computeMinimumCutSets();
}

/*
 * Destructor for parser class
 */
/*!public*/
FaultTree::~FaultTree()
/*!endpublic*/
{}

/*
 * Builds m-ary fault tree based on the set parser
 */
/*!private*/
void FaultTree::buildTree(ns::Parser parser)
/*!endprivate*/
{
  /*
  vector<string> line;
  while (true) {
    line = parser.yieldLine();

    // Stop if no new line to process
    if (line.size() == 0)
      break;

    // Stash name, operator
    _node *node = new _node(line[0], str2Operator(line[1]));

    // Add children
    for (int i = 2; i < line.size(); i++)
      node->_child.push_back(line[i]);

    // Stash the first entry as ROOT of tree
    if (_node_d_b.size() == 0)
      _root = line[0];

    // Add the newly created node to node lookup hashmap
    _node_d_b[line[0]] = node;
  }
  */
  return;
}

/*
 * Translates string to opeartor
 */
/*!private*/
FaultTree::_operator_t FaultTree::str2Operator(string op)
/*!endprivate*/
{
  string op_s = str2Upper(op, true);
  ASSERT(_opDict.count(op_s) != 0, "Illegal Operator found: %s", op.c_str());

  return _opDict[op_s];
}

/*
 * Computes minimum cut sets based on MOCUS Algorithm
 */
/*!private*/
void FaultTree::computeMinimumCutSets()
/*!endprivate*/
{
  // Clearing up sets vector for safety
  rmSets();

  // Add root to set
  set<string> root_set;
  root_set.insert(getRoot());
  _sets.insert(root_set);

  // Start with root node to expand on heirarchy
  cutSetsExpand(getNode(getRoot()));

  // Check if first row is empty
  // NOTE: Since its std::set, only first row could
  // be empty
  if (_sets.begin()->size() == 0) {
    _sets.erase(_sets.begin());
  }

  removeSubsets();
}

/*!private*/
void FaultTree::removeSubsets()
/*!endprivate*/
{
  set<uint64_t> rm_its;

  uint64_t index1 = 0;
  for (set<set<string>>::iterator row1_it = _sets.begin();
       row1_it != _sets.end(); ++row1_it) {
    uint64_t index2 = index1 + 1;
    for (set<set<string>>::iterator row2_it = next(row1_it, 1);
         row2_it != _sets.end(); ++row2_it) {
      bool row1_is_subset = includes(row2_it->begin(), row2_it->end(),
                                     row1_it->begin(), row1_it->end());
      bool row2_is_subset = includes(row1_it->begin(), row1_it->end(),
                                     row2_it->begin(), row2_it->end());
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
  for (set<uint64_t>::iterator it = rm_its.begin(); it != rm_its.end(); ++it) {
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
void FaultTree::cutSetsExpand(_node *node)
/*!endprivate*/
{
  ASSERT(node, "NULL node encountered");
  ASSERT(node->_child.size() != 0, "Empty child list found for node '%s'",
         node->_name.c_str());

  // New rows which have to be appended at end. Adding them might
  // result in data hazards
  set<set<string>> mk_rows;

  // 1. Iterate through entire list and match for own node's name
  for (set<set<string>>::iterator row_it = _sets.begin(); row_it != _sets.end();
       ++row_it) {
    set<string> row(*row_it);
    // Search for the element in row
    set<string>::iterator it = find(row.begin(), row.end(), node->_name);
    if (it != row.end()) {
      // Erase self to add children
      row.erase(it);

      // 2. Replace self with children based on operation
      //    (i) . Replace self with children in same row if AND
      //    (ii). Replace self with child one per row if OR
      switch (node->_op) {
      case AND:
        // Append all children in same row
        for (uint64_t c_id = 0; c_id < node->_child.size(); c_id++) {
          row.insert(node->_child[c_id]);
        }
        break;

      case OR:
        // Replicate line and append one child per row
        for (uint64_t c_id = 0; c_id < node->_child.size(); c_id++) {
          set<string> row_set(row);
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
  for (set<set<string>>::iterator it = mk_rows.begin(); it != mk_rows.end();
       ++it) {
    set<string> row(*it);
    _sets.insert(*it);
  }

  // 3. Recurse on each non leaf child
  for (uint64_t c_id = 0; c_id < node->_child.size(); c_id++) {
    _node *child = getNode(node->_child[c_id]);
    if (child)
      cutSetsExpand(child);
  }
}

/*!private*/
void FaultTree::rmSets()
/*!endprivate*/
{
  for (set<set<string>>::iterator row_it = _sets.begin(); row_it != _sets.end();
       ++row_it) {
    _sets.erase(row_it);
  }
}

/*!private*/
FaultTree::_node *FaultTree::getNode(string name)
/*!endprivate*/
{
  return (_node_d_b.count(name) != 0) ? _node_d_b[name] : NULL;
}

/*!public*/
string FaultTree::getRoot()
/*!endpublic*/
{
  return _root;
}

/*!public*/
void FaultTree::printSets(set<set<string>> sets)
/*!endpublic*/
{
  cout << "------------- SETS BEGIN ----------------- " << endl;
  for (set<set<string>>::iterator row = sets.begin(); row != sets.end();
       ++row) {
    printRow(*row);
    cout << endl;
  }
  cout << "-------------- SETS END ------------------ " << endl;
}

/*!public*/
void FaultTree::printSets()
/*!endpublic*/
{
  printSets(_sets);
}

/*!public*/
void FaultTree::printRow(set<string> row)
/*!endpublic*/
{
  for (set<string>::iterator col = row.begin(); col != row.end(); ++col) {
    cout << *col << ", ";
  }
}

/*
 * Function returns cut sets at the given point
 * NOTE: If MOCUS ran before this function call, min cut sets will be returned
 */
/*!public*/
set<set<string>> FaultTree::getCutSets()
/*!endpublic*/
{
  return _sets;
}
