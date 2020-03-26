// MOOSE includes
#include "gtest/gtest.h"
#include "MooseUtils.h"
#include "Conversion.h"

// Boost distribution includes
#include "BoostDistribution.h"

// MASTODON includes
#include "MastodonUtils.h"

// QUANTIFICATIONUTILS includes
#include "FaultTreeUtils.h"

// Helper to make sure the test tree is correct.
void
assertTree(std::map<std::string, FTAUtils::FaultTree::_node *> tree_node)
{
  // TOP
  EXPECT_EQ(tree_node["TOP"]->_name, "TOP");
  EXPECT_EQ(tree_node["TOP"]->_op, FTAUtils::FaultTree::AND);
  EXPECT_EQ(tree_node["TOP"]->_child.at(0), "GATE1");
  EXPECT_EQ(tree_node["TOP"]->_child.at(1), "GATE2");

  // GATE1
  EXPECT_EQ(tree_node["GATE1"]->_name, "GATE1");
  EXPECT_EQ(tree_node["GATE1"]->_op, FTAUtils::FaultTree::OR);
  EXPECT_EQ(tree_node["GATE1"]->_child.at(0), "FT-N/m-1");
  EXPECT_EQ(tree_node["GATE1"]->_child.at(1), "FT-N/m-2");
  EXPECT_EQ(tree_node["GATE1"]->_child.at(2), "FT-N/m-3");

  // GATE2
  EXPECT_EQ(tree_node["GATE2"]->_name, "GATE2");
  EXPECT_EQ(tree_node["GATE2"]->_op, FTAUtils::FaultTree::OR);
  EXPECT_EQ(tree_node["GATE2"]->_child.at(0), "B1");
  EXPECT_EQ(tree_node["GATE2"]->_child.at(1), "B3");
  EXPECT_EQ(tree_node["GATE2"]->_child.at(2), "B4");

  // FT-N/m-1
  EXPECT_EQ(tree_node["FT-N/m-1"]->_name, "FT-N/m-1");
  EXPECT_EQ(tree_node["FT-N/m-1"]->_op, FTAUtils::FaultTree::AND);
  EXPECT_EQ(tree_node["FT-N/m-1"]->_child.at(0), "GATE3");
  EXPECT_EQ(tree_node["FT-N/m-1"]->_child.at(1), "B3");
  EXPECT_EQ(tree_node["FT-N/m-1"]->_child.at(2), "B5");

  // FT-N/m-2
  EXPECT_EQ(tree_node["FT-N/m-2"]->_name, "FT-N/m-2");
  EXPECT_EQ(tree_node["FT-N/m-2"]->_op, FTAUtils::FaultTree::AND);
  EXPECT_EQ(tree_node["FT-N/m-2"]->_child.at(0), "GATE3");
  EXPECT_EQ(tree_node["FT-N/m-2"]->_child.at(1), "B1");

  // FT-N/m-3
  EXPECT_EQ(tree_node["FT-N/m-3"]->_name, "FT-N/m-3");
  EXPECT_EQ(tree_node["FT-N/m-3"]->_op, FTAUtils::FaultTree::AND);
  EXPECT_EQ(tree_node["FT-N/m-3"]->_child.at(0), "B3");
  EXPECT_EQ(tree_node["FT-N/m-3"]->_child.at(1), "B5");
  EXPECT_EQ(tree_node["FT-N/m-3"]->_child.at(2), "B1");

  // GATE3
  EXPECT_EQ(tree_node["GATE3"]->_name, "GATE3");
  EXPECT_EQ(tree_node["GATE3"]->_op, FTAUtils::FaultTree::OR);
  EXPECT_EQ(tree_node["GATE3"]->_child.at(0), "B2");
  EXPECT_EQ(tree_node["GATE3"]->_child.at(1), "B4");
}

// Helper to make sure the mocus is correct.
void
assertMocus(std::set<std::set<std::string>> tree_mocus)
{

  std::set<std::set<std::string>>::iterator it_tree_mocus;
  std::set<std::string> tree_mocus_children, set_mocus, set_gold;
  std::set<std::string>::iterator it_tree_mocus_children;

  std::set<std::set<std::string>> matrix_gold = {
      {"B2", "B1"}, {"B4", "B1"}, {"B1", "B5", "B3"}, {"B5", "B2", "B3"}, {"B4", "B5", "B3"}};

  std::set<std::set<std::string>>::iterator it_matrix_gold = matrix_gold.begin();

  EXPECT_EQ(tree_mocus.size(), matrix_gold.size());

  it_tree_mocus = tree_mocus.begin();
  int i = 0;
  while (it_tree_mocus != tree_mocus.end())
  {
    set_mocus = *it_tree_mocus++;
    set_gold = *it_matrix_gold++;
    EXPECT_EQ(set_mocus, set_gold);
    i++;
  }
}

// Create a test event list.
std::map<std::string, FTAUtils::FaultTree::_node *>
eventList()
{

  std::map<std::string, FTAUtils::FaultTree::_node *> _node_d_b;

  std::vector<std::vector<std::string>> line = {{"TOP", "AND", "GATE1", "GATE2"},
                                                {"GATE1", "OR", "FT-N/m-1", "FT-N/m-2", "FT-N/m-3"},
                                                {"GATE2", "OR", "B1", "B3", "B4"},
                                                {"GATE3", "OR", "B2", "B4"},
                                                {"FT-N/m-1", "AND", "GATE3", "B3", "B5"},
                                                {"FT-N/m-2", "AND", "GATE3", "B1"},
                                                {"FT-N/m-3", "AND", "B3", "B5", "B1"}};

  std::vector<FTAUtils::FaultTree::_operator_t> op = {FTAUtils::FaultTree::AND,
                                                      FTAUtils::FaultTree::OR,
                                                      FTAUtils::FaultTree::OR,
                                                      FTAUtils::FaultTree::OR,
                                                      FTAUtils::FaultTree::AND,
                                                      FTAUtils::FaultTree::AND,
                                                      FTAUtils::FaultTree::AND};

  for (int i = 0; i < line.size(); i++)
  {
    FTAUtils::FaultTree::_node * node = new FTAUtils::FaultTree::_node(line[i][0], op[i]);
    // Add children
    for (int j = 2; j < line[i].size(); j++)
      node->_child.push_back(line[i][j]);
    // Add the newly created node to node lookup hashmap
    _node_d_b[line[i][0]] = node;
  }

  return _node_d_b;
}

// For input
std::vector<FileName> file_lists_fault_tree_1, file_lists_fault_tree_2, file_lists_fault_tree_3;

TEST(FTAUtils, FaultTree)
{
  // ==================== TestCase for FaultTree Object ===================

  // ---------------- Test error message for events input ----------------

  // ###### File Inputs ######

  try
  {
    file_lists_fault_tree_1 = {"not_a_valid_filename.txt", ""};

    FTAUtils::FaultTree(file_lists_fault_tree_1[0], file_lists_fault_tree_1[1]);
  }
  catch (FTAUtils::CException e)
  {
    /*
     * ------- IO Error -------
     * does not exist
     */
    EXPECT_EQ(e.msg, "Unable to open file.");
  }

  // ---------------- Test creating tree from list ----------------

  std::map<std::string, FTAUtils::FaultTree::_node *> tree_node_from_list = eventList();
  std::set<std::set<std::string>> tree_mocus_from_list;
  FTAUtils::FaultTree ft_from_list = FTAUtils::FaultTree(tree_mocus_from_list, tree_node_from_list);
  assertTree(tree_node_from_list);
  assertMocus(tree_mocus_from_list);

  // ---------------- Test creating tree from file ----------------

  file_lists_fault_tree_2 = {"logic2.txt", ""};

  FTAUtils::FaultTree ft_from_file_2 =
      FTAUtils::FaultTree(file_lists_fault_tree_2[0], file_lists_fault_tree_2[1]);
  FTAUtils::Parser parser_events_2 =
      FTAUtils::Parser(file_lists_fault_tree_2[0], FTAUtils::Parser::FORMAT_CSV);
  std::map<std::string, FTAUtils::FaultTree::_node *> tree_node_from_file =
      ft_from_file_2.buildTree(parser_events_2);
  std::set<std::set<std::string>> tree_mocus_from_file = ft_from_file_2.computeMinimumCutSets();
  assertTree(tree_node_from_file);
  assertMocus(tree_mocus_from_file);
}
