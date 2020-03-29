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


std::string trim(const std::string & str);


std::string str2Upper(const std::string & str_in, bool trim_input = false);

double interpolate(vector_double data, double x, bool extrapolate);


double normalCDF(double x);
double normalCDF(double x, double mu, double sigma);

double Clip(double a, double min, double max);

std::vector<double> genQuantificationRVec(double dataPoint, int n, std::vector<double> rv);
}

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


  FaultTree(std::string file_name, std::string root = "");
  FaultTree(set_string & sets_link, std::map<std::string, _node *> _node_base);


  ~FaultTree();
  std::string getRoot();


  set_string getCutSets();


  std::map<std::string, _node *> buildTree(Parser parser);

  set_string computeMinimumCutSets();
  std::vector<std::string> event(std::map<std::string, _node *>);

private:
  std::map<std::string, _operator_t> _opDict = {{"AND", AND}, {"OR", OR}};
  std::map<_operator_t, std::string> _opDictInv = {{AND, "AND"}, {OR, "OR"}};
  std::map<std::string, _node *> _node_d_b;
  std::string _root;

  set_string _sets;

  _operator_t str2Operator(std::string op);
  void rmSets();
  _node * getNode(std::string name);


  void cutSetsExpand(_node * node);
  void removeSubsets();
};


class FTAUtils::Parser
{
public:
  enum parseFormatT
  {
    FORMAT_CSV,
    FORMAT_UNDEF
  };


  Parser(std::string fileName, parseFormatT format);

  ~Parser();


  vector_string yieldLines();


  std::vector<std::string> yieldLine();

private:

  std::ifstream * fileP;
};

class FTAUtils::CException
{
public:
  std::string msg;
  CException(std::string s) : msg(s) {}
};

#endif
