#include "FaultTreeUtils.h"
#include "MastodonUtils.h"

FTAUtils::SetSetString
FTAUtils::FaultTree(std::string file_name, std::string root)
{

  MapNode curr_tree;
  SetSetString curr_sets, updated_sets;

  std::string curr_root; // current root

  curr_tree = buildTree(file_name, curr_root);
  // Override root node if not default
  // update root
  if (root != "")
  { // root == ""
    // ASSERT
    if (!(getNode(curr_tree, root)))
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
    curr_root = root;
  }

  updated_sets = computeMinimumCutSets(curr_tree, curr_root);

  return updated_sets;
}

FTAUtils::SetSetString
FTAUtils::FaultTree(std::vector<std::vector<std::string>> ftVector, std::string root)
{
  MapNode curr_tree;
  SetSetString curr_sets, updated_sets;

  std::string curr_root; // current root

  curr_tree = buildTreeFromVector(ftVector, curr_root); // _node_d_b; "TOP"

  // Override root node if not default
  // update root
  if (root != "")
  { // root == ""
    // ASSERT
    if (!(getNode(curr_tree, root)))
    {
      fprintf(stderr,
              "[ASSERT] In File:, Line: => Root node "
              "requested  not found in heirarchy. "
              "Please the check file.\n");
      abort();
    }
    curr_root = root;
  }

  updated_sets = computeMinimumCutSets(curr_tree, curr_root);

  return updated_sets;
}

/*
** Function for call from quantification
*/
FTAUtils::SetSetString
FTAUtils::FaultTree(std::string file_name, std::string root, FTAUtils::MapInt & beEventsDict)
{

  MapNode curr_tree;
  SetSetString curr_sets, updated_sets;

  std::string curr_root; // current root

  curr_tree = buildTree(file_name, curr_root, beEventsDict); // _node_d_b; "TOP"

  // Override root node if not default
  // update root
  if (root != "")
  { // root == ""
    // ASSERT
    if (!(getNode(curr_tree, root)))
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
    curr_root = root;
  }

  updated_sets = computeMinimumCutSets(curr_tree, curr_root);

  return updated_sets;
}

FTAUtils::SetSetString
FTAUtils::FaultTree(std::vector<std::vector<std::string>> ftVector,
                    std::string root,
                    FTAUtils::MapInt & beEventsDict)
{
  MapNode curr_tree;
  SetSetString curr_sets, updated_sets;

  std::string curr_root; // current root

  curr_tree = buildTreeFromVector(ftVector, curr_root, beEventsDict); // _node_d_b; "TOP"

  // Override root node if not default
  // update root
  if (root != "")
  { // root == ""
    // ASSERT
    if (!(getNode(curr_tree, root)))
    {
      fprintf(stderr,
              "[ASSERT] In File:, Line: => Root node "
              "requested  not found in heirarchy. "
              "Please the check file.\n");
      abort();
    }
    curr_root = root;
  }

  updated_sets = computeMinimumCutSets(curr_tree, curr_root);

  return updated_sets;
}
/**
 * initialize root, _node_d_b

FTAUtils::MapNode FTAUtils::buildTree(Parser parser, std::string &curr_root) {
  std::vector<std::string> line;
  MapNode node_d_b;
  while (true) {
    line = parser.yieldLine();

    // Stop if no new line to process
    if (line.size() == 0)
      break;

    // Stash name, operator
    _node *node = new _node(line[0], str2Operator(line[1]));


    // Add children
      for( int i = 2; i < line.size(); i++ )
      {

         node->_child.push_back( line[i] );
      }

    // Stash the first entry as ROOT of tree
    if (node_d_b.size() == 0)
      curr_root = line[0];

    // Add the newly created node to node lookup hashmap
    node_d_b[line[0]] = node;
  }

  return node_d_b;
}
*/

FTAUtils::MapNode
FTAUtils::buildTree(std::string fileName, std::string & curr_root)
{

  FTAUtils::VecVecString fileVector = FTAUtils::generateVectorsFromFile(fileName, "FORMAT_CSV");
  MapNode node_d_b;

  FTAUtils::VecString line;
  for (uint64_t row = 0; row < fileVector.size(); row++)
  {
    line = fileVector[row];

    // Stash name, operator
    _node * node = new _node(line[0], str2Operator(line[1]));

    // Add children
    for (int i = 2; i < line.size(); i++)
    {
      node->_child.push_back(line[i]);
    }

    // Stash the first entry as ROOT of tree
    if (node_d_b.size() == 0)
      curr_root = line[0];

    // Add the newly created node to node lookup hashmap
    node_d_b[line[0]] = node;
  }

  return node_d_b;
}

FTAUtils::MapNode
FTAUtils::buildTreeFromVector(std::vector<std::vector<std::string>> ftVector,
                              std::string & curr_root)
{
  MapNode node_d_b;
  if (ftVector.size() > 0)
  {
    curr_root = ftVector[0][0];
  }
  else
  {
    curr_root = "";
  }
  for (uint64_t j = 0; j != ftVector.size(); j++)
  {

    // Stop if no new line to process
    if (ftVector[j].size() == 0)
      break;

    // Stash name, operator
    _node * node = new _node(ftVector[j][0], str2Operator(ftVector[j][1]));

    // Add children
    for (int i = 2; i < ftVector[j].size(); i++)
    {
      node->_child.push_back(ftVector[j][i]);
    }

    // Add the newly created node to node lookup hashmap
    node_d_b[ftVector[j][0]] = node;
  }
  return (node_d_b);
}

FTAUtils::MapNode
FTAUtils::buildTree(Parser parser, std::string & curr_root, MapInt & beEventsDict)
{
  std::vector<std::string> line;
  MapNode node_d_b;
  while (true)
  {
    line = parser.yieldLine();

    // Stop if no new line to process
    if (line.size() == 0)
      break;

    // Stash name, operator
    _node * node = new _node(line[0], str2Operator(line[1]));

    std::size_t found;

    // Add children
    for (int i = 2; i < line.size(); i++)
    {

      node->_child.push_back(line[i]);
      found = line[i].find("GATE");

      if (found == std::string::npos)
      {
        beEventsDict[line[i]] = 1;
      }
    }

    // Stash the first entry as ROOT of tree
    if (node_d_b.size() == 0)
      curr_root = line[0];

    // Add the newly created node to node lookup hashmap
    node_d_b[line[0]] = node;
  }

  return node_d_b;
}

FTAUtils::MapNode
FTAUtils::buildTree(std::string fileName, std::string & curr_root, MapInt & beEventsDict)
{
  FTAUtils::VecVecString fileVector = FTAUtils::generateVectorsFromFile(fileName, "FORMAT_CSV");
  std::vector<std::string> line;
  MapNode node_d_b;
  for (uint64_t row = 0; row < fileVector.size(); row++)
  {
    line = fileVector[row];

    // Stash name, operator
    _node * node = new _node(line[0], str2Operator(line[1]));

    std::size_t found;

    // Add children
    for (int i = 2; i < line.size(); i++)
    {

      node->_child.push_back(line[i]);
      found = line[i].find("GATE");

      if (found == std::string::npos)
      {
        beEventsDict[line[i]] = 1;
      }
    }

    // Stash the first entry as ROOT of tree
    if (node_d_b.size() == 0)
      curr_root = line[0];

    // Add the newly created node to node lookup hashmap
    node_d_b[line[0]] = node;
  }

  return node_d_b;
}

FTAUtils::MapNode
FTAUtils::buildTreeFromVector(std::vector<std::vector<std::string>> ftVector,
                              std::string & curr_root,
                              FTAUtils::MapInt & beEventsDict)
{
  MapNode node_d_b;
  if (ftVector.size() > 0)
  {
    curr_root = ftVector[0][0];
  }
  else
  {
    curr_root = "";
  }
  for (uint64_t j = 0; j != ftVector.size(); j++)
  {

    // Stop if no new line to process
    if (ftVector[j].size() == 0)
      break;

    // Stash name, operator
    _node * node = new _node(ftVector[j][0], str2Operator(ftVector[j][1]));

    std::size_t found;

    // Add children
    for (int i = 2; i < ftVector[j].size(); i++)
    {

      node->_child.push_back(ftVector[j][i]);
      found = ftVector[j][i].find("GATE");

      if (found == std::string::npos)
      {
        beEventsDict[ftVector[j][i]] = 1;
      }
    }

    // Add the newly created node to node lookup hashmap
    node_d_b[ftVector[j][0]] = node;
  }
  return (node_d_b);
}

FTAUtils::_operator_t
FTAUtils::str2Operator(std::string op)
{
  // Inverse mapping for printing purpose only
  std::map<std::string, _operator_t> op_dict = {{"AND", AND}, {"OR", OR}};

  std::string op_s = str2Upper(op, true);

  // ASSERT
  if (op_dict.count(op_s) == 0)
  {
    fprintf(stderr,
            "[ASSERT] In File: %s, Line: %d => "
            "Illegal Operator found: %s.\n",
            __FILE__,
            __LINE__,
            op.c_str());
    abort();
  }
  return op_dict[op_s];
}

FTAUtils::SetSetString
FTAUtils::computeMinimumCutSets(MapNode node_d_b, std::string curr_root)
{
  // Add root to set
  SetSetString curr_sets, updated_sets;
  std::set<std::string> root_set, sub_sets;
  root_set.insert(curr_root);
  curr_sets.insert(root_set);
  std::vector<std::vector<std::string>> paths;
  std::vector<std::string> temp;
  temp.push_back(curr_root);
  paths.push_back(temp);
  std::map<std::string, int> node_d_b_nodes_dict;

  for (MapNode::iterator node_it = node_d_b.begin(); node_it != node_d_b.end(); node_it++)
  {
    node_d_b_nodes_dict[node_it->first] = 1;
  }

  /**
   * test code for nodes
  std::cout << "------test nodes start ..." << std::endl;
  std::map<std::string, _node *>::iterator itr;
  for (itr = node_d_b.begin(); itr != node_d_b.end(); ++itr) {
    std::cout << '\t' << itr->first << "->" << itr->second->_name << '\n';
    std::vector<std::string> childsubb = itr->second->_child;
    for (auto i = childsubb.begin(); i != childsubb.end(); i++)
    {
      std::cout << *i << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
  std::cout << "------test nodes done!" << std::endl;
  */

  // Start with root node to expand on heirarchy
  cutSetsExpand(node_d_b, paths, node_d_b_nodes_dict);

  for (uint64_t i = 0; i < paths.size(); i++)
  {
    std::set<std::string> temp_set;

    for (uint64_t j = 0; j < paths[i].size(); j++)
    {
      temp_set.insert(paths[i][j]);
    }

    updated_sets.insert(temp_set);
  }
  // Check if first row is empty
  // NOTE: Since its std::set, only first row could
  // be empty
  if (updated_sets.begin()->size() == 0)
  {
    updated_sets.erase(updated_sets.begin());
  }

  removeSubsets(updated_sets);

  return updated_sets;
}

void
FTAUtils::removeSubsets(SetSetString & curr_sets)
{
  std::set<uint64_t> rm_its;
  uint64_t index1 = 0;
  for (SetSetString::iterator row1_it = curr_sets.begin(); row1_it != curr_sets.end(); ++row1_it)
  {
    uint64_t index2 = index1 + 1;
    for (SetSetString::iterator row2_it = next(row1_it, 1); row2_it != curr_sets.end(); ++row2_it)
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
    curr_sets.erase(next(curr_sets.begin(), *it - index));
    index++;
  }
}

void
FTAUtils::cutSetsExpand(MapNode node_d_b,
                        std::vector<std::vector<std::string>> & paths,
                        std::map<std::string, int> node_d_b_nodes_dict)
{
  while (true)
  {

    bool changed = false;

    for (uint64_t row = 0; row < paths.size(); row++)
    {
      for (uint64_t column = 0; column < paths[row].size(); column++)
      {
        if (node_d_b_nodes_dict[paths[row][column]])
        {
          changed = true;

          std::vector<std::string> temp_row = paths[row];
          paths.erase(paths.begin() + row);

          std::string cur_node = temp_row[column];
          temp_row.erase(temp_row.begin() + column);
          std::map<std::string, int> cached_nodes;

          for (uint64_t k = 0; k < temp_row.size(); k++)
          {
            cached_nodes[temp_row[k]] = 1;
          }

          if (node_d_b[cur_node]->_op == str2Operator("AND"))
          {
            for (uint64_t k = 0; k < node_d_b[cur_node]->_child.size(); k++)
            {
              if (cached_nodes[node_d_b[cur_node]->_child[k]] != 1)
              {
                temp_row.push_back(node_d_b[cur_node]->_child[k]);
              }
            }

            paths.push_back(temp_row);
          }
          else
          {
            bool added = false;

            for (uint64_t k = 0; k < node_d_b[cur_node]->_child.size(); k++)
            {
              if (cached_nodes[node_d_b[cur_node]->_child[k]] != 1)
              {
                std::vector<std::string> new_temp_row = temp_row;
                new_temp_row.push_back(node_d_b[cur_node]->_child[k]);
                paths.push_back(new_temp_row);
                added = true;
              }
            }

            if (added == false)
            {
              paths.push_back(temp_row);
            }
          }
        }

        if (changed == true)
        {
          break;
        }
      }

      if (changed == true)
      {
        break;
      }
    }
    if (changed != true)
    {
      break;
    }
  }
}

FTAUtils::_node *
FTAUtils::getNode(MapNode & node_d_b, std::string name)
{
  return (node_d_b.count(name) != 0) ? node_d_b[name] : NULL;
}

FTAUtils::VecVecString
FTAUtils::generateVectorsFromFile(std::string fileName, std::string format)
{

  if (format != "FORMAT_CSV")
  {
    fprintf(stderr,
            "[ASSERT] In File: %s, Line: %d => "
            "Unsupported parse format.\n",
            __FILE__,
            __LINE__);
    abort();
  }
  std::ifstream * fileP;
  fileP = new std::ifstream;
  fileP->open(fileName, std::ifstream::in);

  if (!fileP->is_open())
  {
    throw FTAUtils::FileException();
  }

  FTAUtils::VecVecString lines;

  while (true)
  {

    std::string buffer;
    FTAUtils::VecString line;

    // Get a line that has something except \n
    if (getline(*fileP, buffer))
    {
      std::string token;
      std::istringstream iss(buffer);
      while (getline(iss, token, ','))
      {
        std::string trimmed_token = FTAUtils::trim(token);
        if (trimmed_token != "")
        {
          line.push_back(trimmed_token);
        }
      }
    }

    // Stop if no new line to process
    if (line.size() == 0)
      break;
    lines.push_back(line);
  }
  return lines;
}

FTAUtils::Parser::Parser(std::string fileName, Parser::_parseFormatT format)
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
  {
    fprintf(stderr, "Unable to open file");
    abort();
  }
}

FTAUtils::Parser::~Parser() {}

FTAUtils::VecVecString
FTAUtils::Parser::yieldLines()
{
  VecVecString lines;
  std::vector<std::string> line;
  while (true)
  {
    line = Parser::yieldLine();

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
      std::string trimmed_token = FTAUtils::trim(token);
      if (trimmed_token != "")
      {
        line.push_back(trim(token));
      }
    }
  }
  return line;
}

std::string
FTAUtils::trim(const std::string & str)
{
  std::string white_space = " \t\n";

  size_t first = str.find_first_not_of(white_space);
  if (first == std::string::npos)
  {
    return "";
  }
  if (std::string::npos == first)
  {
    return str;
  }
  size_t last = str.find_last_not_of(white_space);
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
