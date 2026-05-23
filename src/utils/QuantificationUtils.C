#include "QuantificationUtils.h"

FTAUtils::Quantification::Quantification(std::map<std::string, vector_double> & params_double,
                                         std::map<std::string, vector_string> & params_string,
                                         std::map<std::string, int> & params_int,
                                         std::map<std::string, bool> & params_bool,
                                         std::map<std::string, _analysis_t> & params_analysis_t,
                                         std::string events_file,
                                         std::string events_prob_file,
                                         _analysis_t analysis,
                                         std::string hazard_file,
                                         double im_lower,
                                         double im_upper,
                                         int n_bins,
                                         bool uncertainty,
                                         std::string root,
                                         int n_sample,
                                         int seed)
{
  //---------------- SAVE PARAMETERS ------------------------------------------
  std::vector<double> params_double1;
  params_double1.push_back(im_lower);
  params_double1.push_back(im_upper);

  vector_double params_double2;
  params_double2.push_back(params_double1);
  params_double.insert(pair_string_double("IM", params_double2));
  params_int.insert(std::pair<std::string, int>("n_bins", n_bins));
  params_int.insert(std::pair<std::string, int>("nsamp", n_sample));
  params_int.insert(std::pair<std::string, int>("seed", seed));
  params_bool.insert(std::pair<std::string, bool>("uncertainty", uncertainty));
  params_analysis_t.insert(std::pair<std::string, _analysis_t>("analysis", analysis));

  //---------------- COMPUTE --------------------------------------------------
  // Construct the fault tree with MOCUS algo for minimal cut sets
  FTAUtils::FaultTree ft = FTAUtils::FaultTree(events_file, root);
  Parser parser_events = Parser(events_file, Parser::FORMAT_CSV);
  vector_string lines_events = parser_events.yieldLines();
  set_string cut_sets = ft.getCutSets();
  params_string.insert(std::pair<std::string, vector_string>("events_files", lines_events));

  // Read basic events file
  FTAUtils::Parser parser_event_prob =
      FTAUtils::Parser(events_prob_file, FTAUtils::Parser::FORMAT_CSV);
  vector_string lines_events_prob;

  if (analysis == FRAGILITY)
  {
    // Read and interpolate hazard curve
    FTAUtils::Parser parser_hazard = FTAUtils::Parser(hazard_file, FTAUtils::Parser::FORMAT_CSV);
    vector_string lines_hazard = parser_hazard.yieldLines();
    vector_double hazard = FTAUtils::Quantification::linesToDouble(lines_hazard);
    params_double.insert(pair_string_double("hazard", hazard));

    // List of Intensity Measure bin values
    std::vector<double> im_bins = getBinMeans(im_lower, im_upper, n_bins);

    // List of Intensity Measure bin values
    std::vector<double> hazard_freq = hazInterp(hazard, im_bins);

    // Dictionary of basic events for fragility input
    lines_events_prob = beProb(parser_event_prob, n_sample, seed, analysis, im_bins, uncertainty);

    // Top event fragility (lognormal parameters)
    double mu, sigma;
    fragility(cut_sets, n_bins, im_bins, mu, sigma);

    // Dictionary of basic events risk (convoluting fragility and hazard)
    computeRisk(n_bins, hazard_freq);
  }
  else
  {
    // Dictionary of basic events risk (risk inputs)
    lines_events_prob =
        beProb(parser_event_prob, n_sample, seed, analysis, std::vector<double>(), uncertainty);
  }

  params_string.insert(std::pair<std::string, vector_string>("basic_events", lines_events_prob));

  // Calculate minimal cut sets probability
  // Digest cut set probability to unified probability
  // Adding 1 to accomodate mean as 0th element
  std::vector<double> * digest = cutSetProbWDigest(cut_sets, n_sample + 1);
  vector_double mc_im = minCutIM(digest[1]);
  Stats s0(digest[0]);
  Stats s1(digest[1]);
  Stats s2(digest[2]);
  std::vector<int> count_v;
  std::map<std::string, vector_double> be_stats = beIM(cut_sets, n_sample + 1, digest[1], count_v);

  // return the FTA top event risk
  std::vector<double> fta_0;
  vector_double fta_1;
  fta_0.push_back(s0._pe);
  fta_0.push_back(s1._pe);
  fta_0.push_back(s2._pe);
  fta_1.push_back(fta_0);
  params_double.insert(std::make_pair("fta", fta_1));

  // Return the Risk Reduction Ratio for B1, B2, B3, B4, B5
  params_double.insert(pair_string_double("fv", be_stats["fv"]));
  params_double.insert(pair_string_double("rrr", be_stats["rrr"]));
  params_double.insert(pair_string_double("rir", be_stats["rir"]));

  // Return the Risk Reduction Difference for B1, B2, B3, B4, B5
  params_double.insert(pair_string_double("rri", be_stats["rri"]));
  params_double.insert(pair_string_double("rii", be_stats["rii"]));
  params_double.insert(pair_string_double("bi", be_stats["bi"]));
}

std::vector<double>
FTAUtils::Quantification::hazInterp(vector_double hazard, std::vector<double> im_bins)
{
  std::vector<double> data;
  // Convert all hazard values to log10
  for (int index = 0; index < hazard.size(); index++)
  {
    // ASSERT
    if (hazard[index].size() != 2)
    {
      fprintf(stderr,
              "[ASSERT] In File: %s, Line: %d => "
              "hazard dimension error.\n",
              __FILE__,
              __LINE__);
      abort();
    }

    hazard[index][0] = log10(hazard[index][0]);
    hazard[index][1] = log10(hazard[index][1]);
  }

  // Interpolate
  for (int index = 0; index < im_bins.size(); index++)
  {
    data.push_back(pow(10, interpolate(hazard, log10(im_bins[index]), true)));
  }
  return data;
}

std::vector<double>
FTAUtils::Quantification::getBinMeans(double im_lower, double im_upper, int n_bins)
{
  std::vector<double> bins;
  double delta = (im_upper - im_lower) / (n_bins);
  for (int index = 0; index < n_bins; index++)
  {
    double bin = im_lower + (delta * (index + 0.5));
    bins.push_back(bin);
  }
  return bins;
}

vector_double
FTAUtils::Quantification::linesToDouble(vector_string lines)
{
  vector_double lines_double;
  for (int index = 0; index < lines.size(); index++)
  {
    std::vector<double> double_vector(lines[index].size());
    transform(lines[index].begin(),
              lines[index].end(),
              double_vector.begin(),
              [](const std::string & val) { return stod(val); });
    lines_double.push_back(double_vector);
  }
  return lines_double;
}

double
FTAUtils::Clip(double a, double min, double max)
{
  return ((a) < min) ? min : ((a) > max ? max : (a));
}

std::vector<double>
FTAUtils::genQuantificationRVec(double dataPoint, int n, std::vector<double> rv)
{
  for (int index = 0; index < n; index++)
  {
    // double dataPoint = double(gen);
    rv.push_back(FTAUtils::Clip(dataPoint, 0.0, 1.0));
  }
  return rv;
}

std::vector<double>
FTAUtils::Quantification::getProbVector(_dist_t dist,
                                        double a,
                                        double b,
                                        int n,
                                        int seed,
                                        std::vector<double> im,
                                        _analysis_t analysis,
                                        bool uc)
{
  std::vector<double> rv;

  std::default_random_engine gen(seed);

  if (analysis == FRAGILITY)
  {
    // ASSERT
    if (dist != LNORM)
    {
      fprintf(stderr,
              "[ASSERT] In File: %s, Line: %d => "
              "unsupported distribution for fragility analysis.\n",
              __FILE__,
              __LINE__);
      abort();
    }

    for (int index = 0; index < im.size(); index++)
    {
      rv.push_back(FTAUtils::normalCDF(log(im[index] / a) / b));
    }
  }
  else
  {
    if (!uc)
    {
      switch (dist)
      {
        case PE:
        {
          rv = FTAUtils::genQuantificationRVec(double(a), 2, rv);
        }
        break;
        case NORM:
        {
          rv = FTAUtils::genQuantificationRVec(double(a), 2, rv);
        }
        break;
        default:
        {
          // ASSERT
          fprintf(stderr,
                  "[ASSERT] In File: %s, Line: %d => "
                  "Un-supported dist found.\n",
                  __FILE__,
                  __LINE__);
          abort();
        }
      }
    }
    else
    {
      switch (dist)
      {
        case PE:
        {
          rv = FTAUtils::genQuantificationRVec(double(a), n, rv);
        }
        break;
        case NORM:
        {
          double rv_norm = FTAUtils::Clip(a, 0, 1);
          rv.push_back(rv_norm);

          std::normal_distribution<double> d(a, b);

          rv = FTAUtils::genQuantificationRVec(d(gen), n, rv);
        }
        break;
        default:
        {
          // ASSERT
          fprintf(stderr,
                  "[ASSERT] In File: %s, Line: %d => "
                  "Un-supported dist found.\n",
                  __FILE__,
                  __LINE__);
          abort();
        }
      }
    }
  }
  return rv;
}

vector_string
FTAUtils::Quantification::beProb(FTAUtils::Parser parser,
                                 int n_sample,
                                 int seed,
                                 _analysis_t analysis,
                                 std::vector<double> intmes,
                                 bool uncert)
{
  std::vector<std::string> line;
  vector_string lines;
  while (true)
  {
    line = parser.yieldLine();

    // Stop if no new line to process
    if (line.size() != 0)
      lines.push_back(line);
    else
      break;

    // Stash name, probability vector
    double b = line.size() > 3 ? stod(line[3]) : 0;
    _b_nodes[line[0]] = getProbVector(
        _str2dist[line[1]], stod(line[2]), b, n_sample, seed, intmes, analysis, uncert);
  }

  return lines;
}

std::vector<double> *
FTAUtils::Quantification::cutSetProbWDigest(set_string cut_sets, int n, bool only_min_max)
{
  std::vector<double> * digest = new std::vector<double>[3];

  // Digest 2: Generate power set to generate all possible combinations
  vector_double ps_p;
  int pow_set_size = pow(2, cut_sets.size());
  // Start from 1 as NULL set is not needed
  for (int index = 1; index < pow_set_size; index++)
  {
    std::set<std::string> power_set_row;
    for (int j = 0; j < cut_sets.size(); j++)
    {
      // Check if jth bit in the index is set
      // If set then form row of power set
      if (index & (1 << j))
      {
        std::set<std::string> j_elem = *next(cut_sets.begin(), j);
        for (std::set<std::string>::iterator it = j_elem.begin(); it != j_elem.end(); ++it)
        {
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
  for (int index = 0; index < n; index++)
  {
    double min_max = 0;
    double upper_bound = 1;
    double top_rare = 0;
    if (!only_min_max)
    {
      for (vector_double::iterator it = _cut_set_prob.begin(); it != _cut_set_prob.end(); ++it)
      {
        top_rare += (*it)[index];
        upper_bound *= 1 - (*it)[index];
      }
      digest[1].push_back(1 - upper_bound);
      digest[2].push_back(top_rare);
    }
    for (vector_double::iterator it = ps_p.begin(); it != ps_p.end(); ++it)
    {
      min_max += (*it)[index];
    }
    digest[0].push_back(min_max);
  }

  return digest;
}

vector_double
FTAUtils::Quantification::computeCutSetProb(
    set_string cut_sets, int n, bool bypass, std::string bypass_key, double bypass_value)
{
  // For each row in cut set, compute:
  // 1. For each sample in the generated vector, compute AND gate
  //    probability analysis for each basic element
  vector_double quant;
  for (set_string::iterator row = cut_sets.begin(); row != cut_sets.end(); ++row)
  {
    quant.push_back(cutSetRowProb(*row, n, true, bypass, bypass_key, bypass_value));
  }
  return quant;
}

std::vector<double>
FTAUtils::Quantification::cutSetRowProb(std::set<std::string> row,
                                        int n,
                                        bool sign_positive,
                                        bool bypass,
                                        std::string bypass_key,
                                        double bypass_value)
{
  std::vector<double> prob;
  double sign_m = sign_positive ? 1 : -1;
  for (int index = 0; index < n; index++)
  {
    double value = 1;
    for (std::set<std::string>::iterator col = row.begin(); col != row.end(); ++col)
    {
      // ASSERT
      if (_b_nodes.find(*col) == _b_nodes.end())
      {
        fprintf(stderr,
                "[ASSERT] In File: %s, Line: %d => "
                "'%s' key not found in _b_nodes.\n",
                __FILE__,
                __LINE__,
                (*col).c_str());
        abort();
      }

      double bv = (bypass && (bypass_key == *col)) ? bypass_value : _b_nodes[*col][index];
      value = getGateProb(value, bv, true);
    }
    prob.push_back(value * sign_m);
  }
  return prob;
}

double
FTAUtils::Quantification::getGateProb(double a, double b, bool is_and)
{
  return is_and ? a * b : (1.0 - a) * (1.0 - b);
}

vector_double
FTAUtils::Quantification::minCutIM(std::vector<double> upper_bound)
{
  vector_double mc_i_m;
  for (int row = 0; row < _cut_set_prob.size(); row++)
  {
    std::vector<double> mc_i_m_row;
    for (int col = 0; col < upper_bound.size(); col++)
    {
      mc_i_m_row.push_back((100.0 * _cut_set_prob[row][col]) / upper_bound[col]);
    }
    mc_i_m.push_back(mc_i_m_row);
  }
  return mc_i_m;
}

void
FTAUtils::Quantification::computeRisk(int n, std::vector<double> hazard)
{
  for (std::map<std::string, std::vector<double>>::iterator bn_it = _b_nodes.begin();
       bn_it != _b_nodes.end();
       ++bn_it)
  {
    double risk = 0;
    for (int index = 0; index < n; index++)
    {
      risk += bn_it->second[index] * hazard[index];
    }
    _b_nodes[bn_it->first] = getProbVector(PE, risk, 0, 1, 0, std::vector<double>(), RISK, false);
  }
}

std::vector<double>
FTAUtils::Quantification::fragility(
    set_string cut_sets, int n, std::vector<double> im_bins, double & mu, double & sigma)
{
  // 1. Calculate TOP event fragility using min-max approach (exact) {digest[0]}
  std::vector<double> * digest = cutSetProbWDigest(cut_sets, n, true);

  // 2. lognormal parameters of TOP Event fragility
  // solveLnParams(im_bins, digest[0], mu, sigma);

  // Use the maximizelikehood function instead of solver function
  // Use SGD for optimization
  std::vector<double> loc_space = {0.01, 2};
  std::vector<double> sca_space = {0.01, 1};
  std::vector<double> max_values = MastodonUtils::maximizeLogLikelihood(
      im_bins, digest[0], loc_space, sca_space, 1000, false, 1e-05, 0.0001, 1000, 1028);
  mu = max_values[0];
  sigma = max_values[1];

  // TODO: 3. TOP event Fragility plot
  return digest[0];
}

std::map<std::string, vector_double>
FTAUtils::Quantification::beIM(set_string cut_sets,
                               int n,
                               std::vector<double> upper_bound,
                               std::vector<int> & count_v)
{
  std::map<std::string, vector_double> stats;
  for (std::map<std::string, std::vector<double>>::iterator bn_it = _b_nodes.begin();
       bn_it != _b_nodes.end();
       ++bn_it)
  {
    // Generate available vector to save computation on per index loop
    std::vector<bool> available;
    int count = 0;
    for (set_string::iterator cs_it = cut_sets.begin(); cs_it != cut_sets.end(); ++cs_it)
    {
      bool is_a = cs_it->find(bn_it->first) != cs_it->end();
      count += is_a;
      available.push_back(is_a);
    }
    count_v.push_back(count);
    if (count != 0)
    {
      std::vector<double> fv, rrr, rir, rri, rii, bi;

      vector_double mc_p1 = computeCutSetProb(cut_sets, n, true, bn_it->first, 1);
      vector_double mc_p0 = computeCutSetProb(cut_sets, n, true, bn_it->first, 0);

      // OR it with loop merging
      for (int index = 0; index < n; index++)
      {
        double f0_val = 1;
        double f1_val = 1;
        double fi_val = 1;
        for (int row = 0; row < mc_p0.size(); row++)
        {
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
      stats["fv"].push_back({fv_stats._pe,
                             fv_stats._mean,
                             fv_stats._median,
                             fv_stats._sd,
                             fv_stats._p5,
                             fv_stats._p95});
      stats["rrr"].push_back({rrr_stats._pe,
                              rrr_stats._mean,
                              rrr_stats._median,
                              rrr_stats._sd,
                              rrr_stats._p5,
                              rrr_stats._p95});
      stats["rri"].push_back({rri_stats._pe,
                              rri_stats._mean,
                              rri_stats._median,
                              rri_stats._sd,
                              rri_stats._p5,
                              rri_stats._p95});
      stats["rir"].push_back({rir_stats._pe,
                              rir_stats._mean,
                              rir_stats._median,
                              rir_stats._sd,
                              rir_stats._p5,
                              rir_stats._p95});
      stats["rii"].push_back({rii_stats._pe,
                              rii_stats._mean,
                              rii_stats._median,
                              rii_stats._sd,
                              rii_stats._p5,
                              rii_stats._p95});
      stats["bi"].push_back({bi_stats._pe,
                             bi_stats._mean,
                             bi_stats._median,
                             bi_stats._sd,
                             bi_stats._p5,
                             bi_stats._p95});
    }
  }
  return stats;
}
