#ifndef _QUANTIFICATION_H
#define _QUANTIFICATION_H

// #include "all.h"
#include "faultTree.h"
#include "parser.h"

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

  Quantification(string events_file, string events_prob_file,
                 _analysis_t analysis = FRAGILITY, string hazard_file = "",
                 double im_lower = 0.1, double im_upper = 4, int n_bins = 15,
                 bool uncertainty = false, string root = "", int n_sample = 1,
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
    Stats(vector<double> vec_in) {

      vector<double> vec;
      vec.clear();

      // Remove all INF from vector
      for (int index = 0; index < vec_in.size(); index++) {
        if (!isinf(vec_in[index])) {
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
    double getPercentile(vector<double> vec, int percentile) {
      double p_i = (vec.size() + 1) * (percentile / 100.0);
      int p_min = floor(p_i);
      p_min = (p_min < 1) ? 1 : p_min;
      int p_max = ceil(p_i);
      p_max = (p_max > (vec.size() - 1)) ? vec.size() : p_max;
      return vec[p_min - 1] +
             ((p_i - p_min) * (vec[p_max - 1] - vec[p_min - 1]));
    }
    void printStats() {
      cout << "Mean: " << _mean << ", Median: " << _median << ", SD: " << _sd
           << ", 5th: " << _p5 << ", 95th: " << _p95 << "\n\n";
    }
  };

  // TODO: 3 of the following are not supported as they need 1 arg rather than 2
  // for their distribution. Remove these 3 if not needed else add support
  map<string, _dist_t> _str2dist = {
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

  vector<vector<double>> _cut_set_prob;
  map<string, vector<double>> _b_nodes;
  vector<double> getProbVector(_dist_t dist, double a, double b, int n,
                               int seed, vector<double> im,
                               _analysis_t analysis, bool uc);
  void beProb(ns::Parser parser, int n_sample, int seed, _analysis_t analysis,
              vector<double> intmes, bool uncert);
  vector<vector<double>> computeCutSetProb(set<set<string>> cut_sets, int n,
                                           bool bypass = false,
                                           string bypass_key = "",
                                           double bypass_value = 0);
  vector<double> cutSetRowProb(set<string> row, int n,
                               bool sign_positive = true, bool bypass = false,
                               string bypass_key = "", double bypass_value = 0);
  vector<double> *cutSetProbWDigest(set<set<string>> cut_sets, int n,
                                    bool only_min_max = false);
  double getGateProb(double a, double b, bool is_and);
  vector<vector<double>> minCutIM(vector<double> upperBound);
  map<string, vector<vector<double>>> beIM(set<set<string>> cut_sets, int n,
                                           vector<double> upper_bound,
                                           vector<int> &count_v);
  vector<vector<double>> linesToDouble(vector<vector<string>> lines);
  vector<double> getBinMeans(double im_lower, double im_upper, int n_bins);
  vector<double> hazInterp(vector<vector<double>> hazard,
                           vector<double> im_bins);
  vector<double> fragility(set<set<string>> cut_sets, int n,
                           vector<double> im_bins, double &mu, double &sigma);
  void computeRisk(int n, vector<double> hazard);
};

#endif // _QUANTIFICATION_H
