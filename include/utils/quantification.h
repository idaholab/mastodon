#ifndef _QUANTIFICATION_H
#define _QUANTIFICATION_H

#include "faultTree.h"
// #include "parser.h"
#include "MastodonUtils.h"

#define CLIP(A, MIN, MAX) (((A) < MIN) ? MIN : ((A) > MAX ? MAX : (A)))

#define GEN_QUALIFICATION_R_VEC(gen, d, n, rv)                                 \
  for (int index = 0; index < n; index++) {                                    \
    double dataPoint = d(gen);                                                 \
    rv.push_back(CLIP(dataPoint, 0.0, 1.0));                                   \
  }

// Quantification class
class Quantification {
public:
  enum _dist_t {
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

  enum _analysis_t {
    FRAGILITY, // = 0
    RISK       //       = 1
  };

  Quantification(std::string events_file, std::string events_prob_file,
                 _analysis_t analysis = FRAGILITY, std::string hazard_file = "",
                 double im_lower = 0.1, double im_upper = 4, int n_bins = 15,
                 bool uncertainty = false, std::string root = "", int n_sample = 1,
                 int seed = 0);

private:
  struct Stats {
    double _pe;
    double _mean;
    double _median;
    double _sd;
    double _variance;
    double _p5;
    double _p95;
    Stats(std::vector<double> vec_in) {

      std::vector<double> vec;
      vec.clear();

      // Remove all INF from vector
      for (int index = 0; index < vec_in.size(); index++) {
        if (!std::isinf(vec_in[index])) {
          vec.push_back(vec_in[index]);
        }
      }

      int size = vec.size();
      ASSERT(size > 0, "Size 0 vector passed for stats computation");

      _pe = vec[0];

      // TODO: Remove 0th element from further calculations
      // vec.erase( vec.begin() );

      sort(vec.begin(), vec.end());
      _mean = accumulate(vec.begin(), vec.end(), 0.0) / size;

      _variance = 0;
      for (int i = 0; i < vec.size(); i++) {
        _variance += pow(vec[i] - _mean, 2);
      }
      _variance /= vec.size();
      _sd = sqrt(_variance);

      _median = (size % 2) ? vec[size / 2]
                           : ((vec[size / 2 - 1] + vec[size / 2]) / 2);

      _p5 = getPercentile(vec, 5);
      _p95 = getPercentile(vec, 95);
    }
    double getPercentile(std::vector<double> vec, int percentile) {
      double p_i = (vec.size() + 1) * (percentile / 100.0);
      int p_min = floor(p_i);
      p_min = (p_min < 1) ? 1 : p_min;
      int p_max = ceil(p_i);
      p_max = (p_max > (vec.size() - 1)) ? vec.size() : p_max;
      return vec[p_min - 1] +
             ((p_i - p_min) * (vec[p_max - 1] - vec[p_min - 1]));
    }
    void printStats() {
      std::cout << "Mean: " << _mean << ", Median: " << _median << ", SD: " << _sd
           << ", 5th: " << _p5 << ", 95th: " << _p95 << "\n\n";
    }
  };

  // TODO: 3 of the following are not supported as they need 1 arg rather than 2
  // for their distribution. Remove these 3 if not needed else add support
  std::map<std::string, _dist_t> _str2dist = {
      /*{"EXP"      , EXP      },*/
      {"GAMMA", GAMMA},
      {"WEIBULL", WEIBULL},
      {"EXTREME", EXTREME},
      {"NORM", NORM},
      {"LNORM", LNORM},
      {"PE", PE},
      /*{"CHI_SQ"   , CHI_SQ   },*/
      {"CAUCHY", CAUCHY},
      {"FISHER_F", FISHER_F},
      /*{"STUDENT_T", STUDENT_T}*/
  };

  std::vector<std::vector<double>> _cut_set_prob;
  std::map<std::string, std::vector<double>> _b_nodes;
  std::vector<double> getProbVector(_dist_t dist, double a, double b, int n,
                               int seed, std::vector<double> im,
                               _analysis_t analysis, bool uc);
  // void beProb(ns::Parser parser, int n_sample, int seed, _analysis_t analysis, 
  //             std::vector<double> intmes, bool uncert);
  // void beProb(std::vector<std::vector<double>> line, int n_sample, int seed, _analysis_t analysis, 
  //             std::vector<double> intmes, bool uncert);
  std::vector<std::vector<double>> computeCutSetProb(std::set<std::set<std::string>> cut_sets, int n,
                                           bool bypass = false,
                                           std::string bypass_key = "",
                                           double bypass_value = 0);
  std::vector<double> cutSetRowProb(std::set<std::string> row, int n,
                               bool sign_positive = true, bool bypass = false,
                               std::string bypass_key = "", double bypass_value = 0);
  std::vector<double> *cutSetProbWDigest(std::set<std::set<std::string>> cut_sets, int n,
                                    bool only_min_max = false);
  double getGateProb(double a, double b, bool is_and);
  std::vector<std::vector<double>> minCutIM(std::vector<double> upperBound);
  std::map<std::string, std::vector<std::vector<double>>> beIM(std::set<std::set<std::string>> cut_sets, int n,
                                           std::vector<double> upper_bound,
                                           std::vector<int> &count_v);
  std::vector<std::vector<double>> linesToDouble(std::vector<std::vector<std::string>> lines);
  std::vector<double> getBinMeans(double im_lower, double im_upper, int n_bins);
  std::vector<double> hazInterp(std::vector<std::vector<double>> hazard,
                           std::vector<double> im_bins);
  std::vector<double> fragility(std::set<std::set<std::string>> cut_sets, int n,
                           std::vector<double> im_bins, double &mu, double &sigma);
  void computeRisk(int n, std::vector<double> hazard);
};

#endif // _QUANTIFICATION_H
