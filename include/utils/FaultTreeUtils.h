#ifndef _FAULTTREE_H
#define _FAULTTREE_H

#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
namespace FTAUtils
{
class Parser;
class CException;

enum _operator_t
{
  AND,
  OR,
  UNDEF
};

struct _node
{
  std::string _name;
  _operator_t _op;
  std::vector<std::string> _child;
  _node(std::string name, _operator_t op)
  {
    this->_name = name;
    this->_op = op;
  }
};

enum _dist_t
{
  PE,
  EXP,
  GAMMA,
  WEIBULL,
  EXTREME,
  NORM,
  LNORM,
  CHI_SQ,
  CAUCHY,
  FISHER_F,
  STUDENT_T
};

enum _analysis_t
{
  FRAGILITY,
  RISK
};
enum parseFormat
{
  FORMAT_CSV,
  FORMAT_UNDEF
};

typedef std::vector<std::string> VecString;
typedef std::vector<std::vector<double>> VecVecDouble;
typedef std::vector<std::vector<std::string>> VecVecString;
typedef std::set<std::set<std::string>> SetSetString;
typedef std::pair<std::string, VecVecDouble> PairVecVecDouble;
typedef std::map<std::string, VecVecDouble> MapVecVecDouble;
typedef std::map<std::string, VecVecString> MapVecVecString;
typedef std::map<std::string, _node *> MapNode;
typedef std::map<std::string, _analysis_t> MapAnalysis;
typedef std::map<std::string, bool> MapBool;
typedef std::map<std::string, int> MapInt;
typedef std::map<std::string, std::vector<std::string>> MapVecString;

/**
 * Creates a fault tree and computes minimal cut sets given a
 * events with type and dependency.
 * The minimal cut sets are computed using the MOCUS algorithm
 * described in the document below.
 * https://www.nrc.gov/docs/ML1119/ML11195A299.pdf
 *
 * Inputs:
 *  events[list]: A list with each row defining an event as follows:
 *    'Event0', Event.AND|Event.OR, ['Dep0', 'Dep1', ...]
 *  events[filename]: A filename containing data representing the events
 *                    in similar fashion as the list above.
 *     Event0, AND|OR, Dep0, Dep1, ...
 *  events[root]: The root which the fault tree will be based on
 *
 * ----------------------------------
 * an example of built tree
 *
 * file_name = logic2.txt
 * root = "" (default)
 *
 * TOP, AND, GATE1, GATE2
 * GATE1, OR, FT-N/m-1, FT-N/m-2, FT-N/m-3
 * GATE2, OR, B1, B3, B4
 * GATE3, OR, B2, B4
 * FT-N/m-1, AND, GATE3, B3, B5
 * FT-N/m-2, AND, GATE3, B1
 * FT-N/m-3, AND, B3, B5, B1
 *
 * @param file_name
 * @param root Default value of root is the first node in file
 * @return
 */
SetSetString FaultTree(std::string file_name, std::string root = "");
SetSetString FaultTree(std::vector<std::vector<std::string>> ftVector, std::string root = "");
SetSetString FaultTree(std::string file_name, std::string root, MapInt & beEventsDict);
SetSetString
FaultTree(std::vector<std::vector<std::string>> ftVector, std::string root, MapInt & beEventsDict);

/**
 * Creates a fault tree and computes minimal cut sets given a events
 * with type and dependency.
 * The minimal cut sets are computed using the MOCUS algorithm described
 * in the document below.
 * https://www.nrc.gov/docs/ML1119/ML11195A299.pdf
 *
 * Inputs:
 *  events[list]: A list with each row defining an event as follows:
 *    'Event0', Event.AND|Event.OR, ['Dep0', 'Dep1', ...]
 *  events[filename]: A filename containing data representing the events
 *                    in similar fashion as the list above.
 *     Event0, AND|OR, Dep0, Dep1, ...
 *  events[nodes]: A map of all nodes which are generated based on
 *                 the events list above
 *
 * for example
 * ------------------------------
 * input information of all nodes
 *
 * node: FT-N/m-1
 * name: FT-N/m-1
 * children: GATE3 B3 B5
 * operator: AND
 *
 * node: FT-N/m-2
 * name: FT-N/m-2
 * children: GATE3 B1
 * operator: AND
 *
 * node: FT-N/m-3
 * name: FT-N/m-3
 * children: B3 B5 B1
 * operator: AND
 *
 * node: GATE1
 * name: GATE1
 * children: FT-N/m-1 FT-N/m-2 FT-N/m-3
 * operator: OR
 *
 * node: GATE2
 * name: GATE2
 * children: B1 B3 B4
 * operator: OR
 *
 * node: GATE3
 * name: GATE3
 * children: B2 B4
 * operator: OR
 *
 * node: TOP
 * name: TOP
 * children: GATE1 GATE2
 * operator: AND
 *
 * ------------------------------
 * return built tree
 *
 * TOP, AND, GATE1, GATE2
 * GATE1, OR, FT-N/m-1, FT-N/m-2, FT-N/m-3
 * GATE2, OR, B1, B3, B4
 * GATE3, OR, B2, B4
 * FT-N/m-1, AND, GATE3, B3, B5
 * FT-N/m-2, AND, GATE3, B1
 * FT-N/m-3, AND, B3, B5, B1
 *
 * @param node_d_b all nodes informatioin
 * @return built tree
 */
// SetSetString FaultTree(MapNode node_d_b);

/**
 * Builds m-ary fault tree based on the set parser
 *
 * @param parser read txt file
 * @param curr_root the root of the tree
 * @return
 */
MapNode buildTree(Parser parser, std::string & curr_root);
MapNode buildTree(std::string fileName, std::string & curr_root);
MapNode buildTreeFromVector(std::vector<std::vector<std::string>> ftVector,
                            std::string & curr_root);
MapNode buildTree(Parser parser, std::string & curr_root, MapInt & beEventsDict);
MapNode buildTree(std::string fileName, std::string & curr_root, MapInt & beEventsDict);
MapNode buildTreeFromVector(std::vector<std::vector<std::string>> ftVector,
                            std::string & curr_root,
                            MapInt & beEventsDict);
/**
 * Computes minimum cut sets based on MOCUS Algorithm
 *
 * @param node_d_b all nodes informatioin
 * @param curr_root root of sets
 * @return sets based on selected root
 */
SetSetString computeMinimumCutSets(MapNode node_d_b, std::string curr_root);

/**
 * Start with root node to expand on heirarchy, it is based of the
 * information of nodes, including name, children, and operator
 *
 * Recursive call function to flood fill sets by expanding them based on
 * operation ALGO:
 *       1. Iterate through entire list and match for own node's name
 *       2. Replace self with children based on operation
 *          (i) . Replace self with children in same row if AND
 *          (ii). Replace self with child one per row if OR
 *       3. Recurse on each non leaf child
 *
 * NOTE: 1. Updates "curr_sets" variable
 *       2. Uses std::set 2d array, hence absorption and idempotence properties
 *          are implicit
 *
 * @param node_d_b all nodes informatioin
 * @param curr_sets Cut sets container for in-place computations
 * @param node the node will be replaced by its children
 * @return the expanded sets
 */
void cutSetsExpand(MapNode node_d_b,
                   std::vector<std::vector<std::string>> & paths,
                   std::map<std::string, int> node_d_b_nodes_dict);

/**
 * remove the subsets from sets, for example,
 * remove row1 if row2 is a subset of row1
 *
 * @param sets Cut sets container for in-place computations
 */
void removeSubsets(SetSetString & sets);

/**
 * get the node, including its name, children, and operator
 *
 * The map::count() is a built-in function in C++ STL which returns 1
 * if the element with key K is present in the map container.
 * It returns 0 if the element with key K is not present in the container.
 *
 * @param node_d_b all nodes information
 * @param name
 * @return the information of selected node
 */
_node * getNode(MapNode & node_d_b, std::string name);

/**
 * String trim method which removes all leading and lagging whitespace in a
 * string
 *
 * @param str
 * @return
 */
std::string trim(const std::string & str);

/**
 * Converts ASCII string to upper case
 *
 * @param str_in
 * @param trim_input
 * @return
 */
std::string str2Upper(const std::string & str_in, bool trim_input = false);

/**
 * change the type of operator from std::string to _operator_t
 *
 * @param op
 * @return
 */
_operator_t str2Operator(std::string op);
VecVecString generateVectorsFromFile(std::string fileName, std::string format);

struct ValueException : public std::exception
{
  const char * what() const throw() { return "ValueError"; }
};

struct FileException : public std::exception
{
  const char * what() const throw() { return "Unable to open file."; }
};

} // end of namespace FTAUtils

/*
 * Parser class
 */
class FTAUtils::Parser
{
public:
  // Supported formats for parsing
  enum _parseFormatT
  {
    FORMAT_CSV,
    FORMAT_UNDEF
  };

  /**
   * Constructor for parser class Loads up fileP based on format
   */
  Parser(std::string fileName, _parseFormatT format);

  /**
   * Destructor for parser class
   */
  ~Parser();

  /**
   * Yields all records, populates the standard structure and returns
   * This function acts as an abstract layer to hide different formats
   * that might be supported in future
   *
   * @return Array of strings
   */
  VecVecString yieldLines();

  /**
   * Yields a single record, populates the standard structure and returns
   * This function acts as an abstract layer to hide different formats
   * that might be supported in future
   *
   * @return Array of strings
   */
  std::vector<std::string> yieldLine();

private:
  // Handle to file
  std::ifstream * fileP;
};

class FTAUtils::CException
{
public:
  std::string msg;
  CException(std::string s) : msg(s) {}
};

#endif // _FAULTTREE_H