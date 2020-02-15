#include "quantification.h"

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
Quantification::Quantification(string events_file, string events_prob_file,
                               _analysis_t analysis, string hazard_file,
                               double im_lower, double im_upper, int n_bins,
                               bool uncertainty, string root, int n_sample,
                               int seed)
/*!endpublic*/
{
  //---------------- COMPUTE --------------------------------------------------
  // Construct the fault tree with MOCUS algo for minimal cut sets
  FaultTree ft = FaultTree(events_file, root);
  set<set<string>> cut_sets = ft.getCutSets();
  // ft.printSets();

  // Read basic events file
  ns::Parser parser_event_prob = ns::Parser(events_prob_file, ns::Parser::FORMAT_CSV);

  if (analysis == FRAGILITY) {
    // Read and interpolate hazard curve
    ns::Parser parser_hazard = ns::Parser(hazard_file, ns::Parser::FORMAT_CSV);
    vector<vector<string>> lines = parser_hazard.yieldLines();
    vector<vector<double>> hazard = linesToDouble(lines);

    // List of Intensity Measure bin values
    vector<double> im_bins = getBinMeans(im_lower, im_upper, n_bins);

    // List of Intensity Measure bin values
    vector<double> hazard_freq = hazInterp(hazard, im_bins);

    // Dictionary of basic events for fragility input
    beProb(parser_event_prob, n_sample, seed, analysis, im_bins, uncertainty);

    // Top event fragility (lognormal parameters)
    double mu, sigma;
    fragility(cut_sets, n_bins, im_bins, mu, sigma);
    cout << "----------- LN PARAMS BEGIN --------------" << endl;
    cout << "mu: " << mu << endl;
    cout << "sigma: " << sigma << endl;
    cout << "------------ LN PARAMS END ---------------" << endl;

    // Dictionary of basic events risk (convoluting fragility and hazard)
    computeRisk(n_bins, hazard_freq);
  } else {
    // Dictionary of basic events risk (risk inputs)
    beProb(parser_event_prob, n_sample, seed, analysis, vector<double>(),
           uncertainty);
  }

  // Calculate minimal cut sets probability
  // Digest cut set probability to unified probability
  // Adding 1 to accomodate mean as 0th element
  vector<double> *digest = cutSetProbWDigest(cut_sets, n_sample + 1);
  vector<vector<double>> mc_im = minCutIM(digest[1]);
  Stats s0(digest[0]);
  Stats s1(digest[1]);
  Stats s2(digest[2]);
  vector<int> count_v;
  map<string, vector<vector<double>>> be_stats =
      beIM(cut_sets, n_sample + 1, digest[1], count_v);

  //---------------- PRINTING --------------------------------------------------
  cout << "---------- PROBABILITY BEGIN -------------" << endl;
  cout << "1. Exact solution: " << s0._pe << endl;
  s0.printStats();
  cout << "2. Upper Bound   : " << s1._pe << endl;
  s1.printStats();
  cout << "3. Rare Event    : " << s2._pe << endl;
  s2.printStats();
  cout << "----------- PROBABILITY END --------------" << endl;
  cout << "-------- CUT SET DETAILS BEGIN -----------" << endl;
  int i = 0;
  for (set<set<string>>::iterator row = cut_sets.begin(); row != cut_sets.end();
       ++row) {
    ft.printRow(*row);
    cout << "\t(PE: " << _cut_set_prob[i][0] << ", IM: " << mc_im[i][0] << ")"
         << endl;
    i++;
  }
  cout << "--------- CUT SET DETAILS END ------------" << endl;
  i = 0;
  for (map<string, vector<double>>::iterator bn_it = _b_nodes.begin();
       bn_it != _b_nodes.end(); ++bn_it) {
    cout << bn_it->first << " (" << count_v[i] << ")" << endl;
    cout << "\t[FV]  " QUANT_BE_STAT_PRINT_HELPER("fv", i);
    cout << "\t[RRR] " QUANT_BE_STAT_PRINT_HELPER("rrr", i);
    cout << "\t[RIR] " QUANT_BE_STAT_PRINT_HELPER("rir", i);
    cout << "\t[RRI] " QUANT_BE_STAT_PRINT_HELPER("rri", i);
    cout << "\t[RII] " QUANT_BE_STAT_PRINT_HELPER("rii", i);
    cout << "\t[BI]  " QUANT_BE_STAT_PRINT_HELPER("bi", i);
    i++;
  }
}

/*
 * Function for interpolating the hazard curve based on range of intensity
 * measures
 */
/*!private*/
vector<double> Quantification::hazInterp(vector<vector<double>> hazard,
                                         vector<double> im_bins)
/*!endprivate*/
{
  vector<double> data;
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
vector<double> Quantification::getBinMeans(double im_lower, double im_upper,
                                           int n_bins)
/*!endprivate*/
{
  vector<double> bins;
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
vector<vector<double>>
Quantification::linesToDouble(vector<vector<string>> lines)
/*!endprivate*/
{
  vector<vector<double>> lines_double;
  for (int index = 0; index < lines.size(); index++) {
    vector<double> double_vector(lines[index].size());
    transform(lines[index].begin(), lines[index].end(), double_vector.begin(),
              [](const string &val) { return stod(val); });
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
vector<double> Quantification::getProbVector(_dist_t dist, double a, double b,
                                             int n, int seed, vector<double> im,
                                             _analysis_t analysis, bool uc)
/*!endprivate*/
{
  vector<double> rv;

  default_random_engine gen(seed);

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
        normal_distribution<double> d(a, b);
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
void Quantification::beProb(ns::Parser parser, int n_sample, int seed,
                            _analysis_t analysis, vector<double> intmes,
                            bool uncert)
/*!endprivate*/
{
  vector<string> line;
  while (true) {
    line = parser.yieldLine();

    // Stop if no new line to process
    if (line.size() == 0)
      break;

    // Stash name, probability vector
    double b = line.size() > 3 ? stod(line[3]) : 0;
    _b_nodes[line[0]] = getProbVector(_str2dist[line[1]], stod(line[2]), b,
                                      n_sample, seed, intmes, analysis, uncert);
  }
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
vector<double> *Quantification::cutSetProbWDigest(set<set<string>> cut_sets,
                                                  int n, bool only_min_max)
/*!endprivate*/
{
  vector<double> *digest = new vector<double>[3];

  // Digest 2: Generate power set to generate all possible combinations
  vector<vector<double>> ps_p;
  int pow_set_size = pow(2, cut_sets.size());
  // Start from 1 as NULL set is not needed
  for (int index = 1; index < pow_set_size; index++) {
    set<string> power_set_row;
    for (int j = 0; j < cut_sets.size(); j++) {
      // Check if jth bit in the index is set
      // If set then form row of power set
      if (index & (1 << j)) {
        set<string> j_elem = *next(cut_sets.begin(), j);
        for (set<string>::iterator it = j_elem.begin(); it != j_elem.end();
             ++it) {
          power_set_row.insert(*it);
        }
      }
    }
    // For elements taken n at a time, if n is odd, positive else negative
    bool is_positive = (__builtin_popcount(index) % 2);
    vector<double> cut_prob = cutSetRowProb(power_set_row, n, is_positive);
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
      for (vector<vector<double>>::iterator it = _cut_set_prob.begin();
           it != _cut_set_prob.end(); ++it) {
        top_rare += (*it)[index];
        upper_bound *= 1 - (*it)[index];
      }
      digest[1].push_back(1 - upper_bound);
      digest[2].push_back(top_rare);
    }
    for (vector<vector<double>>::iterator it = ps_p.begin(); it != ps_p.end();
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
vector<vector<double>>
Quantification::computeCutSetProb(set<set<string>> cut_sets, int n, bool bypass,
                                  string bypass_key, double bypass_value)
/*!endprivate*/
{
  // For each row in cut set, compute:
  // 1. For each sample in the generated vector, compute AND gate
  //    probability analysis for each basic element
  vector<vector<double>> quant;
  for (set<set<string>>::iterator row = cut_sets.begin(); row != cut_sets.end();
       ++row) {
    quant.push_back(
        cutSetRowProb(*row, n, true, bypass, bypass_key, bypass_value));
  }
  return quant;
}

/*!private*/
vector<double> Quantification::cutSetRowProb(set<string> row, int n,
                                             bool sign_positive, bool bypass,
                                             string bypass_key,
                                             double bypass_value)
/*!endprivate*/
{
  vector<double> prob;
  double sign_m = sign_positive ? 1 : -1;
  for (int index = 0; index < n; index++) {
    double value = 1;
    for (set<string>::iterator col = row.begin(); col != row.end(); ++col) {
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
vector<vector<double>> Quantification::minCutIM(vector<double> upper_bound)
/*!endprivate*/
{
  vector<vector<double>> mc_i_m;
  for (int row = 0; row < _cut_set_prob.size(); row++) {
    vector<double> mc_i_m_row;
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
void Quantification::computeRisk(int n, vector<double> hazard)
/*!endprivate*/
{
  for (map<string, vector<double>>::iterator bn_it = _b_nodes.begin();
       bn_it != _b_nodes.end(); ++bn_it) {
    double risk = 0;
    for (int index = 0; index < n; index++) {
      risk += bn_it->second[index] * hazard[index];
    }
    _b_nodes[bn_it->first] =
        getProbVector(PE, risk, 0, 1, 0, vector<double>(), RISK, false);
  }
}

/*
 * Function for top event fragility
 */
/*!private*/
vector<double> Quantification::fragility(set<set<string>> cut_sets, int n,
                                         vector<double> im_bins, double &mu,
                                         double &sigma)
/*!endprivate*/
{
  // 1. Calculate TOP event fragility using min-max approach (exact) {digest[0]}
  vector<double> *digest = cutSetProbWDigest(cut_sets, n, true);

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
map<string, vector<vector<double>>>
Quantification::beIM(set<set<string>> cut_sets, int n,
                     vector<double> upper_bound, vector<int> &count_v)
/*!endprivate*/
{
  map<string, vector<vector<double>>> stats;
  for (map<string, vector<double>>::iterator bn_it = _b_nodes.begin();
       bn_it != _b_nodes.end(); ++bn_it) {
    // Generate available vector to save computation on per index loop
    vector<bool> available;
    int count = 0;
    for (set<set<string>>::iterator cs_it = cut_sets.begin();
         cs_it != cut_sets.end(); ++cs_it) {
      bool is_a = cs_it->find(bn_it->first) != cs_it->end();
      count += is_a;
      available.push_back(is_a);
    }
    count_v.push_back(count);
    if (count != 0) {
      vector<double> fv, rrr, rir, rri, rii, bi;

      vector<vector<double>> mc_p1 =
          computeCutSetProb(cut_sets, n, true, bn_it->first, 1);
      vector<vector<double>> mc_p0 =
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
