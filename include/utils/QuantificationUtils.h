#ifndef _QUANTIFICATION_H
#define _QUANTIFICATION_H

#include <random>
#include "FaultTreeUtils.h"
#include "MastodonUtils.h"

/********************* Quantification Definition *********************/

class FTAUtils::Quantification
{
public:
  enum _dist_t
  {
    PE,    //    = 0
    EXP,   //   = 1
    GAMMA, // = 2
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
    FRAGILITY, // = 0
    RISK       //       = 1
  };

  Quantification(std::map<std::string, vector_double> & params_double,
                 std::map<std::string, vector_string> & params_string,
                 std::map<std::string, int> & params_int,
                 std::map<std::string, bool> & params_bool,
                 std::map<std::string, _analysis_t> & params_analysis_t,
                 std::string events_file,
                 std::string events_prob_file,
                 _analysis_t analysis = FRAGILITY,
                 std::string hazard_file = "",
                 double im_lower = 0.1,
                 double im_upper = 4,
                 int n_bins = 15,
                 bool uncertainty = false,
                 std::string root = "",
                 int n_sample = 1,
                 int seed = 0);

private:
  struct Stats
  {
    double _pe;
    double _mean;
    double _median;
    double _sd;
    double _variance;
    double _p5;
    double _p95;
    Stats(std::vector<double> vec_in)
    {

      std::vector<double> vec;
      vec.clear();

      // Remove all INF from vector
      for (int index = 0; index < vec_in.size(); index++)
      {
        if (!std::isinf(vec_in[index]))
        {
          vec.push_back(vec_in[index]);
        }
      }

      int size = vec.size();

      // ASSERT
      if (!(size > 0))
      {
        fprintf(stderr,
                "[ASSERT] In File: %s, Line: %d => Root node "
                "requested (%s) not found in heirarchy. "
                "Size 0 vector passed for stats computation.\n",
                __FILE__,
                __LINE__);
        abort();
      }

      _pe = vec[0];

      // TODO: Remove 0th element from further calculations
      // vec.erase( vec.begin() );

      sort(vec.begin(), vec.end());
      _mean = accumulate(vec.begin(), vec.end(), 0.0) / size;

      _variance = 0;
      for (int i = 0; i < vec.size(); i++)
      {
        _variance += pow(vec[i] - _mean, 2);
      }
      _variance /= vec.size();
      _sd = sqrt(_variance);

      _median = (size % 2) ? vec[size / 2] : ((vec[size / 2 - 1] + vec[size / 2]) / 2);

      _p5 = getPercentile(vec, 5);
      _p95 = getPercentile(vec, 95);
    }
    double getPercentile(std::vector<double> vec, int percentile)
    {
      double p_i = (vec.size() + 1) * (percentile / 100.0);
      int p_min = floor(p_i);
      p_min = (p_min < 1) ? 1 : p_min;
      int p_max = ceil(p_i);
      p_max = (p_max > (vec.size() - 1)) ? vec.size() : p_max;
      return vec[p_min - 1] + ((p_i - p_min) * (vec[p_max - 1] - vec[p_min - 1]));
    }
  };

  // TODO: 3 of the following are not supported as they need 1 arg rather than 2
  // for their distribution. Remove these 3 if not needed else add support
  std::map<std::string, _dist_t> _str2dist = {
      {"GAMMA", GAMMA},
      {"WEIBULL", WEIBULL},
      {"EXTREME", EXTREME},
      {"NORM", NORM},
      {"LNORM", LNORM},
      {"PE", PE},
      {"CAUCHY", CAUCHY},
      {"FISHER_F", FISHER_F},
  };

  vector_double _cut_set_prob;
  std::map<std::string, std::vector<double>> _b_nodes;

  /**
   *  Generates probability vector for a specified distribution
   *  Nomenclature of a and b changes with distribution.
   *  eg., a => mean, b => std for NORM
   */
  std::vector<double> getProbVector(_dist_t dist,
                                    double a,
                                    double b,
                                    int n,
                                    int seed,
                                    std::vector<double> im,
                                    _analysis_t analysis,
                                    bool uc);

  /**
   *  Parses and floods probabilties of basic elements
   */
  vector_string beProb(FTAUtils::Parser parser,
                       int n_sample,
                       int seed,
                       _analysis_t analysis,
                       std::vector<double> intmes,
                       bool uncert);

  /**
   *  Computes probability for a given cut set on a per set basis based on
   *  pre-flooded basic elem probs
   */
  vector_double computeCutSetProb(std::set<std::set<std::string>> cut_sets,
                                  int n,
                                  bool bypass = false,
                                  std::string bypass_key = "",
                                  double bypass_value = 0);

  std::vector<double> cutSetRowProb(std::set<std::string> row,
                                    int n,
                                    bool sign_positive = true,
                                    bool bypass = false,
                                    std::string bypass_key = "",
                                    double bypass_value = 0);

  /**
   *  Computes accumulated probability for the entire cut set
   *  3 Digests are computed:
   *    0. Min Max
   *    1. Upper Bound
   *    2. Top Rare
   *
   *  NOTE
   *    1. Its better to compute these 2 together as they have common loops
   *           which can save time
   *    2. Sets the vector cutSetProb
   */
  std::vector<double> *
  cutSetProbWDigest(std::set<std::set<std::string>> cut_sets, int n, bool only_min_max = false);

  /**
   *  Wrapper function for gate probability arith
   *  AND => a * b
   *  OR  => (1 - a) * (1 - b)
   */
  double getGateProb(double a, double b, bool is_and);

  /**
   *  Computes cut-set Fussel-Vesely Important measures
   */
  vector_double minCutIM(std::vector<double> upperBound);

  /**
   *  For each basic element:
   *    1. Calculate #occurrence in minimal cut set (beCount)
   *    2. Store row index of all occurrence (beIndex)
   */
  std::map<std::string, vector_double> beIM(std::set<std::set<std::string>> cut_sets,
                                            int n,
                                            std::vector<double> upper_bound,
                                            std::vector<int> & count_v);

  /**
   *  Translator function for string -> double
   */
  vector_double linesToDouble(vector_string lines);

  /**
   *  Function for getting BIN mean values of intensity measure
   */
  std::vector<double> getBinMeans(double im_lower, double im_upper, int n_bins);

  /**
   *  Function for interpolating the hazard curve based on range of intensity
   *  measures
   */
  std::vector<double> hazInterp(vector_double hazard, std::vector<double> im_bins);

  /**
   *  Function for top event fragility
   */
  std::vector<double> fragility(std::set<std::set<std::string>> cut_sets,
                                int n,
                                std::vector<double> im_bins,
                                double & mu,
                                double & sigma);

  /**
   *  Function for calculating risk by convoluting hazard and fragility
   *  Assign risk to basic event probabilites
   *
   *  WARNING
   *    This consumes _b_nodes and then overwrites it
   */
  void computeRisk(int n, std::vector<double> hazard);
};

#endif // _QUANTIFICATION_H
