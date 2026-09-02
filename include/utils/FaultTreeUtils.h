#ifndef _FAULT_TREE_H
#define _FAULT_TREE_H

#include <set>
#include <map>
#include <string>
#include <vector>

typedef std::vector<std::vector<double>> vector_double;
typedef std::vector<std::vector<std::string>> vector_string;
typedef std::set<std::set<std::string>> set_string;
typedef std::pair<std::string, vector_double> pair_string_double;

namespace FTAUtils
{
class Parser;
class Quantification;
class FaultTree;
class CException;

/**
 *  String trim method which removes all leading and lagging whitespace in a string
 */
std::string trim(const std::string & str);

/**
 *  Converts ASCII string to upper case
 */
std::string str2Upper(const std::string & str_in, bool trim_input = false);

/**
 *  Returns interpolated value at x from parallel arrays ( xData, yData )
 *  Assumes that xData has at least two elements, is sorted and is strictly monotonic increasing
 *  boolean argument extrapolate determines behaviour beyond ends of array (if needed)
 */
double interpolate(vector_double data, double x, bool extrapolate);

/**
 *  Phi(-∞, x) aka N(x)
 */
double normalCDF(double x);
double normalCDF(double x, double mu, double sigma);

double Clip(double a, double min, double max);

std::vector<double> genQuantificationRVec(double dataPoint, int n, std::vector<double> rv);
} // namespace FTAUtils

/********************* Fault Tree Definition *********************/

class FTAUtils::FaultTree
{
public:
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

  /**
   * Constructor for fault tree class
   * Default value of root is the first node in file
   */
  FaultTree(std::string file_name, std::string root = "");
  FaultTree(set_string & sets_link, std::map<std::string, _node *> _node_base);

  /**
   *  Destructor
   */
  ~FaultTree();
  std::string getRoot();

  /**
   *  Function returns cut sets at the given point
   *  NOTE
   *    If MOCUS ran before this function call, min cut sets will be returned
   */
  set_string getCutSets();

  /**
   *  Builds m-ary fault tree
   */
  std::map<std::string, _node *> buildTree(Parser parser);

  /**
   *  Computes minimum cut sets based on MOCUS Algorithm
   */
  set_string computeMinimumCutSets();
  std::vector<std::string> event(std::map<std::string, _node *>);

private:
  // Hash map for operators
  std::map<std::string, _operator_t> _opDict = {{"AND", AND}, {"OR", OR}};
  // Inverse mapping for printing purpose only
  std::map<_operator_t, std::string> _opDictInv = {{AND, "AND"}, {OR, "OR"}};
  std::map<std::string, _node *> _node_d_b;
  std::string _root;

  // Cut sets container for in-place computations
  set_string _sets;

  /**
   *  Translates string to opeartor
   */
  _operator_t str2Operator(std::string op);
  void rmSets();
  _node * getNode(std::string name);

  /**
   *  Recursive call function to flood fill sets by expanding them based on
   *  operation ALGO
   *    1. Iterate through entire list and match for own node's name
   *    2. Replace self with children based on operation
   *         (i) . Replace self with children in same row if AND
   *         (ii). Replace self with child one per row if OR
   *    3. Recurse on each non leaf child
   *
   *  NOTE
   *    1. Updates "sets" variable
   *    2. Uses std::set 2d array, hence absorption and idempotence properties
   *       are implicit
   */
  void cutSetsExpand(_node * node);
  void removeSubsets();
};

/************************** Parser Definition **************************/

class FTAUtils::Parser
{
public:
  // Supported formats for parsing
  enum parseFormatT
  {
    FORMAT_CSV,
    FORMAT_UNDEF
  };

  /**
   *  Constructor for parser class
   */
  Parser(std::string fileName, parseFormatT format);

  /**
   * Destructor for parser class
   */
  ~Parser();

  /**
   *  Yields all records, populates the standard structure and returns
   *  This function acts as an abstract layer to hide different formats
   *  that might be supported in future
   *
   *  Returns: Array of strings
   */
  vector_string yieldLines();

  /**
   *  Yields a single record, populates the standard structure and returns
   *  This function acts as an abstract layer to hide different formats
   *  that might be supported in future
   *
   *  Returns: Array of strings
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

#endif // _FAULT_TREE_H
