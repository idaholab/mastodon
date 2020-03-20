#ifndef _FAULT_TREE_H
#define _FAULT_TREE_H

#include <set>
#include <map>
#include <string>
#include <vector>

#include "Utils.h"

namespace FTAUtils
{
  class Parser;
  class Quantification;
  class FaultTree;
  class CException;
  std::string trim(const std::string& str);
  std::string str2Upper(const std::string& str_in, bool trim_input=false);
  double interpolate( std::vector <std::vector<double>> data, double x, bool extrapolate );
  double normalCDF(double x); // Phi(-∞, x) aka N(x)
  double normalCDF ( double x, double mu, double sigma );
  double lnCDF ( double x, double mu, double sigma );
}

// Fault Tree class
class FTAUtils::FaultTree {
public:
  enum _operator_t { AND, OR, UNDEF };

  struct _node {
    std::string _name;
    _operator_t _op;
    std::vector<std::string> _child;
    _node(std::string name, _operator_t op) {
      this->_name = name;
      this->_op = op;
    }
  };

  FaultTree(std::string file_name, std::string root = "");
  ~FaultTree();
  std::string getRoot();
  void printSets();
  static void printSets(std::set<std::set<std::string>> sets);
  static void printRow(std::set<std::string> row);
  std::set<std::set<std::string>> getCutSets();

private:
  // Hash map for operators
  std::map<std::string, _operator_t> _opDict = {{"AND", AND}, {"OR", OR}};

  // Inverse mapping for printing purpose only
  std::map<_operator_t, std::string> _opDictInv = {{AND, "AND"}, {OR, "OR"}};
  std::map<std::string, _node *> _node_d_b;
  std::string _root;

  // Cut sets container for in-place computations
  std::set<std::set<std::string>> _sets;

  // Member functions
  // void buildTree(std::vector<std::string> line);
  void buildTree(Parser parser);
  void computeMinimumCutSets();
  _operator_t str2Operator(std::string op);
  void rmSets();
  _node *getNode(std::string name);
  void cutSetsExpand(_node *node);
  void removeSubsets();
};

class FTAUtils::Parser {
   public:
      // Supported formats for parsing
      enum parseFormatT {
         FORMAT_CSV,
         FORMAT_UNDEF
      };

      Parser( std::string fileName, parseFormatT format );
      ~Parser();
      std::vector <std::string> yieldLine();
      std::vector <std::vector<std::string>> yieldLines();

   private:
      // Handle to file
      std::ifstream*  fileP;
};

class FTAUtils::CException
{
public:
    std::string msg;
    CException(std::string s) : msg(s) {}
};

#endif // _FAULT_TREE_H