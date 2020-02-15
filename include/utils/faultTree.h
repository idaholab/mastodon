#ifndef _FAULT_TREE_H
#define _FAULT_TREE_H

#include "all.h"
#include "parser.h"

// Fault Tree class
class FaultTree {
public:
  enum _operator_t { AND, OR, UNDEF };

  struct _node {
    string _name;
    _operator_t _op;
    vector<string> _child;
    _node(string name, _operator_t op) {
      this->_name = name;
      this->_op = op;
    }
  };

  FaultTree(string file_name, string root = "");
  ~FaultTree();
  string getRoot();
  void printSets();
  static void printSets(set<set<string>> sets);
  static void printRow(set<string> row);
  set<set<string>> getCutSets();

private:
  // Hash map for operators
  map<string, _operator_t> _opDict = {{"AND", AND}, {"OR", OR}};

  // Inverse mapping for printing purpose only
  map<_operator_t, string> _opDictInv = {{AND, "AND"}, {OR, "OR"}};
  map<string, _node *> _node_d_b;
  string _root;

  // Cut sets container for in-place computations
  set<set<string>> _sets;

  // Member functions
  void buildTree(ns::Parser parser);
  void computeMinimumCutSets();
  _operator_t str2Operator(string op);
  void rmSets();
  _node *getNode(string name);
  void cutSetsExpand(_node *node);
  void removeSubsets();
};

#endif // _FAULT_TREE_H
