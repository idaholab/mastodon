#include "Quantification.h"

/*
 * Constructor for fault tree class
 * NOTE: Default value of root is the first node in file
 */
/*!public*/
FaultTree::FaultTree(std::string file_name, std::string root)
/*!endpublic*/
{
  MooseUtils::DelimitedFileReader demand_logic_file(file_name);
  demand_logic_file.read();
  std::vector<string> fault_tree_logic = demand_logic_file.getNames();
  buildTree(fault_tree_logic);

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
 * Destructor
 */
/*!public*/
FaultTree::~FaultTree()
/*!endpublic*/
{}

/*
 * Builds m-ary fault tree
 */
/*!private*/
void FaultTree::buildTree(std::vector<std::string> line)
/*!endprivate*/
{
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

  return;
}

/*
 * Translates string to opeartor
 */
/*!private*/
FaultTree::_operator_t FaultTree::str2Operator(std::string op)
/*!endprivate*/
{
  std::string op_s = str2Upper(op, true);
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
  std::set<std::string> root_set;
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
  std::set<uint64_t> rm_its;

  uint64_t index1 = 0;
  for (std::set<std::set<std::string>>::iterator row1_it = _sets.begin();
       row1_it != _sets.end(); ++row1_it) {
    uint64_t index2 = index1 + 1;
    for (std::set<std::set<std::string>>::iterator row2_it = next(row1_it, 1);
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
  for (std::set<uint64_t>::iterator it = rm_its.begin(); it != rm_its.end(); ++it) {
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
  std::set<std::set<std::string>> mk_rows;

  // 1. Iterate through entire list and match for own node's name
  for (std::set<std::set<std::string>>::iterator row_it = _sets.begin(); row_it != _sets.end();
       ++row_it) {
    std::set<std::string> row(*row_it);
    // Search for the element in row
    std::set<std::string>::iterator it = find(row.begin(), row.end(), node->_name);
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
          std::set<std::string> row_set(row);
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
  for (std::set<std::set<std::string>>::iterator it = mk_rows.begin(); it != mk_rows.end();
       ++it) {
    std::set<std::string> row(*it);
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
  for (std::set<std::set<std::string>>::iterator row_it = _sets.begin(); row_it != _sets.end();
       ++row_it) {
    _sets.erase(row_it);
  }
}

/*!private*/
FaultTree::_node *FaultTree::getNode(std::string name)
/*!endprivate*/
{
  return (_node_d_b.count(name) != 0) ? _node_d_b[name] : NULL;
}

/*!public*/
std::string FaultTree::getRoot()
/*!endpublic*/
{
  return _root;
}

/*!public*/
void FaultTree::printSets(std::set<std::set<std::string>> sets)
/*!endpublic*/
{
  std::cout << "------------- SETS BEGIN ----------------- " << std::endl;
  for (std::set<std::set<std::string>>::iterator row = sets.begin(); row != sets.end();
       ++row) {
    printRow(*row);
    std::cout << std::endl;
  }
  std::cout << "-------------- SETS END ------------------ " << std::endl;
}

/*!public*/
void FaultTree::printSets()
/*!endpublic*/
{
  printSets(_sets);
}

/*!public*/
void FaultTree::printRow(std::set<std::string> row)
/*!endpublic*/
{
  for (std::set<std::string>::iterator col = row.begin(); col != row.end(); ++col) {
    std::cout << *col << ", ";
  }
}

/*
 * Function returns cut sets at the given point
 * NOTE: If MOCUS ran before this function call, min cut sets will be returned
 */
/*!public*/
std::set<std::set<std::string>> FaultTree::getCutSets()
/*!endpublic*/
{
  return _sets;
}

// A macro to reduce typing while printing stats
#define QUANT_BE_STAT_PRINT_HELPER(index, i)                                   \
  "PE: " << be_stats[index][i][0] << ", Mean: " << be_stats[index][i][1]       \
         << ", Median: " << be_stats[index][i][2]                              \
         << ", SD: " << be_stats[index][i][3]                                  \
         << ", 5th: " << be_stats[index][i][4]                                 \
         << ", 95th: " << be_stats[index][i][5] << "\n"

/*
 * Constructor for qualifications class
 */
/*!public*/
Quantification::Quantification(std::string events_file, std::string events_prob_file,
                               _analysis_t analysis, std::string hazard_file,
                               double im_lower, double im_upper, int n_bins,
                               bool uncertainty, std::string root, int n_sample,
                               int seed)
/*!endpublic*/
{
  //---------------- COMPUTE --------------------------------------------------
  // Construct the fault tree with MOCUS algo for minimal cut sets
  FaultTree ft = FaultTree(events_file, root);
  std::set<std::set<std::string>> cut_sets = ft.getCutSets();
  // ft.printSets();

  // Read basic events file
  MooseUtils::DelimitedFileReader demand_event_prob(events_prob_file);
  demand_event_prob.read();
  std::vector<string> event_prob = demand_event_prob.getNames();

  if (analysis == FRAGILITY) {
    // Read and interpolate hazard curve
    MooseUtils::DelimitedFileReader demand_hazard(hazard_file);
    demand_hazard.read(); // type: MooseUtils::DelimitedFileReader
    std::vector<std::vector<double>> hazard = demand_hazard.getData();

    // List of Intensity Measure bin values
    std::vector<double> im_bins = getBinMeans(im_lower, im_upper, n_bins);

    // List of Intensity Measure bin values
    std::vector<double> hazard_freq = hazInterp(hazard, im_bins);

    // Dictionary of basic events for fragility input
    beProb(event_prob, n_sample, seed, analysis, im_bins, uncertainty);

    // Top event fragility (lognormal parameters)
    double mu, sigma;
    fragility(cut_sets, n_bins, im_bins, mu, sigma);
    std::cout << "----------- LN PARAMS BEGIN --------------" << std::endl;
    std::cout << "mu: " << mu << std::endl;
    std::cout << "sigma: " << sigma << std::endl;
    std::cout << "------------ LN PARAMS END ---------------" << std::endl;

    // Dictionary of basic events risk (convoluting fragility and hazard)

    computeRisk(n_bins, hazard_freq);
  } else {
    // Dictionary of basic events risk (risk inputs)
    beProb(event_prob, n_sample, seed, analysis, std::vector<double>(), uncertainty);
  }

  // Calculate minimal cut sets probability
  // Digest cut set probability to unified probability
  // Adding 1 to accomodate mean as 0th element
  std::vector<double> *digest = cutSetProbWDigest(cut_sets, n_sample + 1);
  std::vector<std::vector<double>> mc_im = minCutIM(digest[1]);
  Stats s0(digest[0]);
  Stats s1(digest[1]);
  Stats s2(digest[2]);
  std::vector<int> count_v;
  std::map<std::string, std::vector<std::vector<double>>> be_stats =
      beIM(cut_sets, n_sample + 1, digest[1], count_v);

  //---------------- PRINTING --------------------------------------------------
  std::cout << "---------- PROBABILITY BEGIN -------------" << std::endl;
  std::cout << "1. Exact solution: " << s0._pe << std::endl;
  s0.printStats();
  std::cout << "2. Upper Bound   : " << s1._pe << std::endl;
  s1.printStats();
  std::cout << "3. Rare Event    : " << s2._pe << std::endl;
  s2.printStats();
  std::cout << "----------- PROBABILITY END --------------" << std::endl;
  std::cout << "-------- CUT SET DETAILS BEGIN -----------" << std::endl;
  int i = 0;
  for (std::set<std::set<std::string>>::iterator row = cut_sets.begin(); row != cut_sets.end();
       ++row) {
    ft.printRow(*row);
    std::cout << "\t(PE: " << _cut_set_prob[i][0] << ", IM: " << mc_im[i][0] << ")"
         << std::endl;
    i++;
  }
  std::cout << "--------- CUT SET DETAILS END ------------" << std::endl;
  i = 0;
  for (std::map<std::string, std::vector<double>>::iterator bn_it = _b_nodes.begin();
       bn_it != _b_nodes.end(); ++bn_it) {
    std::cout << bn_it->first << " (" << count_v[i] << ")" << std::endl;
    std::cout << "\t[FV]  " QUANT_BE_STAT_PRINT_HELPER("fv", i);
    std::cout << "\t[RRR] " QUANT_BE_STAT_PRINT_HELPER("rrr", i);
    std::cout << "\t[RIR] " QUANT_BE_STAT_PRINT_HELPER("rir", i);
    std::cout << "\t[RRI] " QUANT_BE_STAT_PRINT_HELPER("rri", i);
    std::cout << "\t[RII] " QUANT_BE_STAT_PRINT_HELPER("rii", i);
    std::cout << "\t[BI]  " QUANT_BE_STAT_PRINT_HELPER("bi", i);
    i++;
  }
}

/*
 * Function for interpolating the hazard curve based on range of intensity
 * measures
 */
/*!private*/
std::vector<double> Quantification::hazInterp(std::vector<std::vector<double>> hazard, std::vector<double> im_bins)
/*!endprivate*/
{
  std::vector<double> data;
  // Convert all hazard values to log10
  for (int index = 0; index < hazard.size(); index++) {
    ASSERT(hazard[index].size() == 2, "hazard dimension error");
    hazard[index][0] = log10(hazard[index][0]);
    hazard[index][1] = log10(hazard[index][1]);
  }

  // Interpolate
  for (int index = 0; index < im_bins.size(); index++) {
    data.push_back(pow(10, interpolate(hazard, log10(im_bins[index]), true)));
  }
  return data;
}

/*
 * Function for getting BIN mean values of intensity measure
 */
/*!private*/
std::vector<double> Quantification::getBinMeans(double im_lower, double im_upper, int n_bins)
/*!endprivate*/
{
  std::vector<double> bins;
  double delta = (im_upper - im_lower) / (n_bins);
  for (int index = 0; index < n_bins; index++) {
    double bin = im_lower + (delta * (index + 0.5));
    bins.push_back(bin);
  }
  return bins;
}

/*
 * Translator function for string -> double
 */
/*!private*/
std::vector<std::vector<double>>
Quantification::linesToDouble(std::vector<std::vector<std::string>> lines)
/*!endprivate*/
{
  std::vector<std::vector<double>> lines_double;
  for (int index = 0; index < lines.size(); index++) {
    std::vector<double> double_vector(lines[index].size());
    transform(lines[index].begin(), lines[index].end(), double_vector.begin(),
              [](const std::string &val) { return stod(val); });
    lines_double.push_back(double_vector);
  }
  return lines_double;
}

/*
 * Generates probability vector for a specified distribution
 * Nomenclature of a and b changes with distribution.
 * eg., a => mean, b => std for NORM
 */
/*!private*/
std::vector<double> Quantification::getProbVector(_dist_t dist, double a, double b,
                                             int n, int seed, std::vector<double> im,
                                             _analysis_t analysis, bool uc)
/*!endprivate*/
{
  std::vector<double> rv;

  std::default_random_engine gen(seed);

  if (analysis == FRAGILITY) {
    ASSERT(dist == LNORM, "unsupported distribution for fragility analysis");
    for (int index = 0; index < im.size(); index++) {
      rv.push_back(normalCDF(log(im[index] / a) / b));
    }
  } else {
    if (!uc) {
      switch (dist) {
      case PE: {
        GEN_QUALIFICATION_R_VEC(a, double, 2, rv);
      } break;
      case NORM: {
        GEN_QUALIFICATION_R_VEC(a, double, 2, rv);
      } break;
      default:
        ASSERT(false, "Un-supported dist found");
      }
    } else {
      switch (dist) {
      case PE: {
        GEN_QUALIFICATION_R_VEC(a, double, n, rv);
      } break;
      case NORM: {
        rv.push_back(CLIP(a, 0, 1));
        std::normal_distribution<double> d(a, b);
        GEN_QUALIFICATION_R_VEC(gen, d, n, rv);
      } break;
      default:
        ASSERT(false, "Un-supported dist found");
      }
    }
  }
  return rv;
}


/*
 * Parses and floods probabilties of basic elements
 */
/*!private*/
void Quantification::beProb(std::vector<std::string> line, int n_sample, int seed,
                            _analysis_t analysis, std::vector<double> intmes,
                            bool uncert)
/*!endprivate*/
{
  double b = line.size() > 3 ? stod(line[3]) : 0;
  _b_nodes[line[0]] = getProbVector(_str2dist[line[1]], stod(line[2]), b, n_sample, seed, intmes, analysis, uncert);
}

/*
 * Computes accumulated probability for the entire cut set
 * 3 Digests are computed:
 *   0. Min Max
 *   1. Upper Bound
 *   2. Top Rare
 * NOTE: 1. Its better to compute these 2 together as they have common loops
 *          which can save time
 *       2. Sets the vector cutSetProb
 */
/*!private*/
std::vector<double> *Quantification::cutSetProbWDigest(std::set<std::set<std::string>> cut_sets,
                                                  int n, bool only_min_max)
/*!endprivate*/
{
  std::vector<double> *digest = new std::vector<double>[3];

  // Digest 2: Generate power set to generate all possible combinations
  std::vector<std::vector<double>> ps_p;
  int pow_set_size = pow(2, cut_sets.size());
  // Start from 1 as NULL set is not needed
  for (int index = 1; index < pow_set_size; index++) {
    std::set<std::string> power_set_row;
    for (int j = 0; j < cut_sets.size(); j++) {
      // Check if jth bit in the index is set
      // If set then form row of power set
      if (index & (1 << j)) {
        std::set<std::string> j_elem = *next(cut_sets.begin(), j);
        for (std::set<std::string>::iterator it = j_elem.begin(); it != j_elem.end();
             ++it) {
          power_set_row.insert(*it);
        }
      }
    }
    // For elements taken n at a time, if n is odd, positive else negative
    bool is_positive = (__builtin_popcount(index) % 2);
    std::vector<double> cut_prob = cutSetRowProb(power_set_row, n, is_positive);
    ps_p.push_back(cut_prob);

  }

  // Digest 0, 1, 2
  // Avoiding an extra function call to interm event like in the python
  // counterpart for performance reasons (loop merging)
  _cut_set_prob = computeCutSetProb(cut_sets, n);
  for (int index = 0; index < n; index++) {
    double min_max = 0;
    double upper_bound = 1;
    double top_rare = 0;
    if (!only_min_max) {
      for (std::vector<std::vector<double>>::iterator it = _cut_set_prob.begin();
           it != _cut_set_prob.end(); ++it) {
        top_rare += (*it)[index];
        upper_bound *= 1 - (*it)[index];
      }
      digest[1].push_back(1 - upper_bound);
      digest[2].push_back(top_rare);
    }
    for (std::vector<std::vector<double>>::iterator it = ps_p.begin(); it != ps_p.end();
         ++it) {
      min_max += (*it)[index];
    }
    digest[0].push_back(min_max);
  }

  return digest;
}

/*
 * Computes probability for a given cut set on a per set basis based on
 * pre-flooded basic elem probs
 */
/*!private*/
std::vector<std::vector<double>>
Quantification::computeCutSetProb(std::set<std::set<std::string>> cut_sets, int n, bool bypass,
                                  std::string bypass_key, double bypass_value)
/*!endprivate*/
{
  // For each row in cut set, compute:
  // 1. For each sample in the generated vector, compute AND gate
  //    probability analysis for each basic element
  std::vector<std::vector<double>> quant;
  for (std::set<std::set<std::string>>::iterator row = cut_sets.begin(); row != cut_sets.end();
       ++row) {
    quant.push_back(
        cutSetRowProb(*row, n, true, bypass, bypass_key, bypass_value));
  }
  return quant;
}

/*!private*/
std::vector<double> Quantification::cutSetRowProb(std::set<std::string> row, int n,
                                             bool sign_positive, bool bypass,
                                             std::string bypass_key,
                                             double bypass_value)
/*!endprivate*/
{
  std::vector<double> prob;
  double sign_m = sign_positive ? 1 : -1;
  for (int index = 0; index < n; index++) {
    double value = 1;
    for (std::set<std::string>::iterator col = row.begin(); col != row.end(); ++col) {
      ASSERT(_b_nodes.find(*col) != _b_nodes.end(),
             "'%s' key not found in _b_nodes", (*col).c_str());

      double bv = (bypass && (bypass_key == *col)) ? bypass_value
                                                   : _b_nodes[*col][index];
      value = getGateProb(value, bv, true);
    }
    prob.push_back(value * sign_m);
  }
  return prob;
}

/*
 * Wrapper function for gate probability arith
 * AND => a * b
 * OR  => (1 - a) * (1 - b)
 */
/*!private*/
double Quantification::getGateProb(double a, double b, bool is_and)
/*!endprivate*/
{
  return is_and ? a * b : (1.0 - a) * (1.0 - b);
}

/*
 * Computes cut-set Fussel-Vesely Important measures
 */
/*!private*/
std::vector<std::vector<double>> Quantification::minCutIM(std::vector<double> upper_bound)
/*!endprivate*/
{
  std::vector<std::vector<double>> mc_i_m;
  for (int row = 0; row < _cut_set_prob.size(); row++) {
    std::vector<double> mc_i_m_row;
    for (int col = 0; col < upper_bound.size(); col++) {
      mc_i_m_row.push_back((100.0 * _cut_set_prob[row][col]) /
                           upper_bound[col]);
    }
    mc_i_m.push_back(mc_i_m_row);
  }
  return mc_i_m;
}

/*
 * Function for calculating risk by convoluting hazard and fragility
 * Assign risk to basic event probabilites
 * WARNING: This consumes _b_nodes and then overwrites it
 */
/*!private*/
void Quantification::computeRisk(int n, std::vector<double> hazard)
/*!endprivate*/
{
  for (std::map<std::string, std::vector<double>>::iterator bn_it = _b_nodes.begin();
       bn_it != _b_nodes.end(); ++bn_it) {
    double risk = 0;
    for (int index = 0; index < n; index++) {
      risk += bn_it->second[index] * hazard[index];
    }
    _b_nodes[bn_it->first] =
        getProbVector(PE, risk, 0, 1, 0, std::vector<double>(), RISK, false);
  }
}

/*
 * Function for top event fragility
 */
/*!private*/
std::vector<double> Quantification::fragility(std::set<std::set<std::string>> cut_sets, int n,
                                         std::vector<double> im_bins, double &mu,
                                         double &sigma)
/*!endprivate*/
{
  // 1. Calculate TOP event fragility using min-max approach (exact) {digest[0]}
  std::vector<double> *digest = cutSetProbWDigest(cut_sets, n, true);

  // 2. lognormal parameters of TOP Event fragility
  solveLnParams(im_bins, digest[0], mu, sigma);

  // TODO: 3. TOP event Fragility plot
  return digest[0];
}

/* For each basic element:
 * 1. Calculate #occurrence in minimal cut set (beCount)
 * 2. Store row index of all occurrence (beIndex)
 */
/*!private*/
std::map<std::string, std::vector<std::vector<double>>>
Quantification::beIM(std::set<std::set<std::string>> cut_sets, int n,
                     std::vector<double> upper_bound, std::vector<int> &count_v)
/*!endprivate*/
{
  std::map<std::string, std::vector<std::vector<double>>> stats;
  for (std::map<std::string, std::vector<double>>::iterator bn_it = _b_nodes.begin();
       bn_it != _b_nodes.end(); ++bn_it) {
    // Generate available vector to save computation on per index loop
    std::vector<bool> available;
    int count = 0;
    for (std::set<std::set<std::string>>::iterator cs_it = cut_sets.begin();
         cs_it != cut_sets.end(); ++cs_it) {
      bool is_a = cs_it->find(bn_it->first) != cs_it->end();
      count += is_a;
      available.push_back(is_a);
    }
    count_v.push_back(count);
    if (count != 0) {
      std::vector<double> fv, rrr, rir, rri, rii, bi;

      std::vector<std::vector<double>> mc_p1 =
          computeCutSetProb(cut_sets, n, true, bn_it->first, 1);
      std::vector<std::vector<double>> mc_p0 =
          computeCutSetProb(cut_sets, n, true, bn_it->first, 0);

      // OR it with loop merging
      for (int index = 0; index < n; index++) {
        double f0_val = 1;
        double f1_val = 1;
        double fi_val = 1;
        for (int row = 0; row < mc_p0.size(); row++) {
          f0_val *= 1 - mc_p0[row][index];
          f1_val *= 1 - mc_p1[row][index];
          if (available[row])
            fi_val *= 1 - _cut_set_prob[row][index];
        }
        f0_val = 1 - f0_val;
        f1_val = 1 - f1_val;
        fi_val = 1 - fi_val;

        fv.push_back(fi_val / upper_bound[index]);
        rrr.push_back(upper_bound[index] / f0_val);
        rri.push_back(upper_bound[index] - f0_val);
        rir.push_back(f1_val / upper_bound[index]);
        rii.push_back(f1_val - upper_bound[index]);
        bi.push_back(f1_val - f0_val);
      }
      // Calculate stats for current row and stash
      Stats fv_stats(fv);
      Stats rrr_stats(rrr);
      Stats rri_stats(rri);
      Stats rir_stats(rir);
      Stats rii_stats(rii);
      Stats bi_stats(bi);
      stats["fv"].push_back({fv_stats._pe, fv_stats._mean, fv_stats._median,
                             fv_stats._sd, fv_stats._p5, fv_stats._p95});
      stats["rrr"].push_back({rrr_stats._pe, rrr_stats._mean, rrr_stats._median,
                              rrr_stats._sd, rrr_stats._p5, rrr_stats._p95});
      stats["rri"].push_back({rri_stats._pe, rri_stats._mean, rri_stats._median,
                              rri_stats._sd, rri_stats._p5, rri_stats._p95});
      stats["rir"].push_back({rir_stats._pe, rir_stats._mean, rir_stats._median,
                              rir_stats._sd, rir_stats._p5, rir_stats._p95});
      stats["rii"].push_back({rii_stats._pe, rii_stats._mean, rii_stats._median,
                              rii_stats._sd, rii_stats._p5, rii_stats._p95});
      stats["bi"].push_back({bi_stats._pe, bi_stats._mean, bi_stats._median,
                             bi_stats._sd, bi_stats._p5, bi_stats._p95});
    }
  }
  return stats;
}
