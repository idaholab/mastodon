#ifndef _FAULT_TREE_H
#define _FAULT_TREE_H

#include "utils.h"
#include "solver.h"

#include "MastodonUtils.h"

// Fault Tree class
class FaultTree {
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
  void buildTree(std::vector<std::string> line);
  void computeMinimumCutSets();
  _operator_t str2Operator(std::string op);
  void rmSets();
  _node *getNode(std::string name);
  void cutSetsExpand(_node *node);
  void removeSubsets();
};

#endif // _FAULT_TREE_H
