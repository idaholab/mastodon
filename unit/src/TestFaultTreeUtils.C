// MOOSE includes
#include "Conversion.h"
#include "MooseUtils.h"
#include "gtest/gtest.h"

// Boost distribution includes
#include "BoostDistribution.h"

// MASTODON includes
#include "MastodonUtils.h"

// QUANTIFICATIONUTILS includes
#include "FaultTreeUtils.h"

// Helper to make sure the test tree is correct.
void
assertTree(FTAUtils::MapNode tree_node) // node_d_b
{
  // TOP
  EXPECT_EQ(tree_node["TOP"]->_name, "TOP");
  EXPECT_EQ(tree_node["TOP"]->_op, FTAUtils::AND);
  EXPECT_EQ(tree_node["TOP"]->_child.at(0), "GATE1");
  EXPECT_EQ(tree_node["TOP"]->_child.at(1), "GATE2");

  // GATE1
  EXPECT_EQ(tree_node["GATE1"]->_name, "GATE1");
  EXPECT_EQ(tree_node["GATE1"]->_op, FTAUtils::OR);
  EXPECT_EQ(tree_node["GATE1"]->_child.at(0), "FT-N/m-1");
  EXPECT_EQ(tree_node["GATE1"]->_child.at(1), "FT-N/m-2");
  EXPECT_EQ(tree_node["GATE1"]->_child.at(2), "FT-N/m-3");

  // GATE2
  EXPECT_EQ(tree_node["GATE2"]->_name, "GATE2");
  EXPECT_EQ(tree_node["GATE2"]->_op, FTAUtils::OR);
  EXPECT_EQ(tree_node["GATE2"]->_child.at(0), "B1");
  EXPECT_EQ(tree_node["GATE2"]->_child.at(1), "B3");
  EXPECT_EQ(tree_node["GATE2"]->_child.at(2), "B4");

  // FT-N/m-1
  EXPECT_EQ(tree_node["FT-N/m-1"]->_name, "FT-N/m-1");
  EXPECT_EQ(tree_node["FT-N/m-1"]->_op, FTAUtils::AND);
  EXPECT_EQ(tree_node["FT-N/m-1"]->_child.at(0), "GATE3");
  EXPECT_EQ(tree_node["FT-N/m-1"]->_child.at(1), "B3");
  EXPECT_EQ(tree_node["FT-N/m-1"]->_child.at(2), "B5");

  // FT-N/m-2
  EXPECT_EQ(tree_node["FT-N/m-2"]->_name, "FT-N/m-2");
  EXPECT_EQ(tree_node["FT-N/m-2"]->_op, FTAUtils::AND);
  EXPECT_EQ(tree_node["FT-N/m-2"]->_child.at(0), "GATE3");
  EXPECT_EQ(tree_node["FT-N/m-2"]->_child.at(1), "B1");

  // FT-N/m-3
  EXPECT_EQ(tree_node["FT-N/m-3"]->_name, "FT-N/m-3");
  EXPECT_EQ(tree_node["FT-N/m-3"]->_op, FTAUtils::AND);
  EXPECT_EQ(tree_node["FT-N/m-3"]->_child.at(0), "B3");
  EXPECT_EQ(tree_node["FT-N/m-3"]->_child.at(1), "B5");
  EXPECT_EQ(tree_node["FT-N/m-3"]->_child.at(2), "B1");

  // GATE3
  EXPECT_EQ(tree_node["GATE3"]->_name, "GATE3");
  EXPECT_EQ(tree_node["GATE3"]->_op, FTAUtils::OR);
  EXPECT_EQ(tree_node["GATE3"]->_child.at(0), "B2");
  EXPECT_EQ(tree_node["GATE3"]->_child.at(1), "B4");
}

// Helper to make sure the mocus is correct.
void
assertMocus(FTAUtils::SetSetString tree_mocus)
{

  FTAUtils::SetSetString::iterator it_tree_mocus;
  std::set<std::string> tree_mocus_children, set_mocus, set_gold;
  std::set<std::string>::iterator it_tree_mocus_children;

  FTAUtils::SetSetString matrix_gold = {
      {"B2", "B1"}, {"B4", "B1"}, {"B1", "B5", "B3"}, {"B5", "B2", "B3"}, {"B4", "B5", "B3"}};

  FTAUtils::SetSetString::iterator it_matrix_gold = matrix_gold.begin();

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
FTAUtils::MapNode
eventList() // node_d_b
{
  FTAUtils::MapNode node_d_b;

  FTAUtils::VecVecString line = {{"TOP", "AND", "GATE1", "GATE2"},
                                 {"GATE1", "OR", "FT-N/m-1", "FT-N/m-2", "FT-N/m-3"},
                                 {"GATE2", "OR", "B1", "B3", "B4"},
                                 {"GATE3", "OR", "B2", "B4"},
                                 {"FT-N/m-1", "AND", "GATE3", "B3", "B5"},
                                 {"FT-N/m-2", "AND", "GATE3", "B1"},
                                 {"FT-N/m-3", "AND", "B3", "B5", "B1"}};

  std::vector<FTAUtils::_operator_t> op = {FTAUtils::AND,
                                           FTAUtils::OR,
                                           FTAUtils::OR,
                                           FTAUtils::OR,
                                           FTAUtils::AND,
                                           FTAUtils::AND,
                                           FTAUtils::AND};

  for (int i = 0; i < line.size(); i++)
  {
    FTAUtils::_node * node = new FTAUtils::_node(line[i][0], op[i]);
    // Add children
    for (int j = 2; j < line[i].size(); j++)
      node->_child.push_back(line[i][j]);
    // Add the newly created node to node lookup hashmap
    node_d_b[line[i][0]] = node;
  }

  return node_d_b;
}

TEST(FTAUtils, FaultTree)
{
  // ==================== TestCase for FaultTree Object ===================

  // ---------------- Test error message for events input ----------------

  // ###### File Inputs ######

  try
  {
    std::vector<FileName> file_lists1 = {"not_a_valid_filename.txt", ""};
    FTAUtils::SetSetString tree_mocus_from_list1 =
        FTAUtils::FaultTree(file_lists1[0], file_lists1[1]);
  }
  catch (FTAUtils::FileException & e)
  {
    /*
     * ------- IO Error -------
     * does not exist
     */
    std::string error_message(e.what());

    EXPECT_EQ(error_message, "Unable to open file.");
  }

  // ---------------- Test creating tree from list ----------------

  FTAUtils::MapNode tree_node_from_list2 = eventList(); // node_d_b
  FTAUtils::VecVecString line = {{"TOP", "AND", "GATE1", "GATE2"},
                                 {"GATE1", "OR", "FT-N/m-1", "FT-N/m-2", "FT-N/m-3"},
                                 {"GATE2", "OR", "B1", "B3", "B4"},
                                 {"GATE3", "OR", "B2", "B4"},
                                 {"FT-N/m-1", "AND", "GATE3", "B3", "B5"},
                                 {"FT-N/m-2", "AND", "GATE3", "B1"},
                                 {"FT-N/m-3", "AND", "B3", "B5", "B1"}};
  FTAUtils::SetSetString tree_mocus_from_list2 = FTAUtils::FaultTree(line, "");

  assertTree(tree_node_from_list2);
  assertMocus(tree_mocus_from_list2);

  // ---------------- Test creating tree from file ----------------

  std::vector<FileName> file_lists3 = {"logic2.txt", ""};
  std::string curr_root3;
  FTAUtils::Parser parser_events3 = FTAUtils::Parser(file_lists3[0], FTAUtils::Parser::FORMAT_CSV);
  FTAUtils::MapNode tree_node_from_file3 = FTAUtils::buildTree(file_lists3[0], curr_root3);
  FTAUtils::SetSetString tree_mocus_from_file3 =
      FTAUtils::computeMinimumCutSets(tree_node_from_file3, curr_root3);

  assertTree(tree_node_from_file3);
  assertMocus(tree_mocus_from_file3);
}
