#ifndef _QUANTIFICATION_H
#define _QUANTIFICATION_H

#include "FaultTreeUtils.h"
#include "MastodonUtils.h"
#include <random>

namespace FTAUtils
{

struct Stats
{
  double _pe;
  double _mean;
  double _median;
  double _sd;
  double _variance;
  double _p5;
  double _p95;
  int _duplicate_events_flag = 0;
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
      _pe = -1;
      _mean = -1;
      _median = -1;
      _sd = -1;
      _variance = -1;
      _p5 = -1;
      _p95 = -1;
      _duplicate_events_flag = 1;
    }
    else
    {

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

/**
 * Calculates the minimal cut set probabilities and system level probability by
 * propagating basic event probabilities through the fault tree.
 *
 * Two approaches, 1 and 2, are used for top event risk calculation. When the
 * input parameter, lite, is True only Approach 2 is performed. When lite is
 * False, both Approach 1 and 2 are performed.
 *
 * Approach 1: This is a comprehensive calculation where basic event fragilities
 * are first convolved with the hazard to calculate the risks. The basic event
 * risks are then propagated through the fault tree as point estimates. This
 * allows for the calculation of importance measures and all the importance
 * measures are calculated and results are output into the corresponding folder.
 * The top event risk is calculated using three methods: Rare event approximation
 * Upper bound, and Min Max. Risk calculated using approach 1 is provided by the
 * toprisk_1 function. The results calculation using approach 1 are saved in the
 * approach_1 folder.
 *
 * Approach 2: In approach 2, the fragilities are propagated through the fault
 * tree in each bin using the Min-Max method (exact calculation). This results in
 * the top event fragility. The top event fragility is then convolved with the
 * hazard to calculate risk. Risk calculated using approach 2 is provided by the
 * toprisk_2 function. The results calculated using approach 2 are saved in the
 * approach_2 folder.
 *
 * 11 input parameters: 1,2,3,4,5,6,7,9 are for Fragility inputs and seismic PRA.
 *                      1,2,3,8,9,10,11 are for probabilistic risk inputs
 *                      for uncertainty analysis.
 *                      1,2,3 are for deterministic risk inputs.
 *
 * (1) events_file[string]: A filename containing data representing the events in
 *                          similar fashion as the list below.
 *
 *                          The list with each row defining an event as follows:
 *                         'Event0', Event.AND|Event.OR, ['Dep0', 'Dep1', ...]
 *
 * (2) basic_events[string]: A filename containing data representing the
 *                           basic events in similar fashion as the list above.
 *                           Basic_event0, PE, Mean
 *
 *                           The list with each row defining a basic event
 *                           probability as follows: 'Basic_event0', 'PE', [Mean]
 *
 * (3) analysis[string]: Default is "Risk". The basic event prob.
 *                       are assumed to be risk values.
 *
 * (4) hazard_file[string]: A filename containing data representing the hazard
 *                            data in a similar fashion as the list below.
 *
 *                           The list with each row defining PGA and Prob. of
 *                           Exceedance as follows:
 *                         [[PGA_1, Prob. of exceedance_1],
 *                          [PGA_2, Prob. of exceedance_2],
 *                           ...
 *                          [PGA_n, Prob. of exceedance_n]]
 *
 *                           If None, the default list is
 *                         [[0.0608, 0.01], [0.2124, 0.001], [0.4, 0.0001],
 *                          [0.629, 1e-05], [0.9344, 1e-06], [1.3055, 1e-07]]
 *
 * (5) nbins[int]: Default is 15. Number of bins for Intensity Measure.
 *                 Can input any integer greater than 1.
 *
 * (6) im_lower[double]: IM[0]
 * (7) im_upper[double]: IM[1]
 *
 *     IM[list]: - If none is provided, a default range of [0.1,4] is assumed.
 *               - If a list of two elements is provided, it is assumed that
 *                 this corresponds to the lower and upper bounds of the intensity
 *                 measures. This range is then split into nbins according to the
 *                 nbins input parameter.
 *               - If a list longer than 2 elements is provided, these elements
 *                 are assumed to be the IM bin extents and the IM values for each
 *                 bin are calculated as the geometric mean of the extents of each bin.
 *                 The input parameter, nbins, is ignored in this case and the
 *                 number of bins is assumed to be equal to the length of this list minus 1.
 *
 * (8) uncertainty: Default is False (Point Estimate values).
 *                  If True, performs Monte Carlo simulation.
 *
 * (9) root[string]: Default value of root is the first node in file.
 *
 * (10) n_sample[int]: Default is 1. Number of samples for uncertainty analysis.
 *                     Can input any integer greater than 1.
 *
 * (11) seed[int]: Default is None (set seed from clock).
 *                 Can input any integer between 0 and (2**32 - 1).
 *
 * 5 returned parameters used for testing code, the details of these parameters
 *   are shown in the file of TestQuantificationUtils.C
 *
 * (1) params_double: value with type of double
 * (2) params_string: value with type of string
 * (3) params_int: value with type of int
 * (4) params_bool: value with type of bool
 * (5) params_analysis_t: value with type of analysis_t
 *
 * -----------------------------------------
 *
 * @param params_double returned value
 * @param params_string returned value
 * @param params_int returned value
 * @param params_bool returned value
 * @param params_analysis_t returned value
 * @param events_file
 * @param events_prob_file
 * @param analysis
 * @param hazard_file
 * @param n_bins
 * @param im_lower IM[0]
 * @param im_upper IM[1]
 * @param uncertainty
 * @param root
 * @param n_sample
 * @param seed
 *
 */
void Quantification(MapVecVecDouble & params_double,
                    MapVecVecString & params_string,
                    MapInt & params_int,
                    MapBool & params_bool,
                    MapAnalysis & params_analysis_t,
                    std::string events_file,
                    std::string events_prob_file,
                    _analysis_t analysis = RISK,
                    std::string hazard_file = "hazard.txt",
                    int n_bins = 15,
                    double im_lower = 0.1,
                    double im_upper = 4,
                    bool uncertainty = false,
                    std::string root = "",
                    int n_sample = 1,
                    int seed = 0);

void Quantification(MapVecVecDouble & params_double,
                    MapVecVecString & params_string,
                    MapInt & params_int,
                    MapBool & params_bool,
                    MapAnalysis & params_analysis_t,
                    VecVecString events_file,
                    std::string events_prob_file,
                    _analysis_t analysis = RISK,
                    std::string hazard_file = "hazard.txt",
                    int n_bins = 15,
                    double im_lower = 0.1,
                    double im_upper = 4,
                    bool uncertainty = false,
                    std::string root = "",
                    int n_sample = 1,
                    int seed = 0);

void Quantification(MapVecVecDouble & params_double,
                    MapVecVecString & params_string,
                    MapInt & params_int,
                    MapBool & params_bool,
                    MapAnalysis & params_analysis_t,
                    VecVecString events_file,
                    VecVecString events_prob_vector,
                    _analysis_t analysis = RISK,
                    std::string hazard_file = "hazard.txt",
                    int n_bins = 15,
                    double im_lower = 0.1,
                    double im_upper = 4,
                    bool uncertainty = false,
                    std::string root = "",
                    int n_sample = 1,
                    int seed = 0);
/**
 * Generates probability vector for a specified distribution
 * Nomenclature of a and b changes with distribution.
 * eg., a => mean, b => std for NORM
 *
 * @param dist
 * @param a
 * @param b
 * @param n
 * @param seed
 * @param im
 * @param analysis
 * @param uc
 * @return
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
 * Parses and floods probabilties of basic elements
 *
 * @param curr_b_nodes
 * @param parser
 * @param n_sample
 * @param seed
 * @param analysis
 * @param intmes
 * @param uncert
 */
VecVecString beProb(std::map<std::string, std::vector<double>> & curr_b_nodes,
                    std::string events_prob_file,
                    int n_sample,
                    int seed,
                    _analysis_t analysis,
                    std::vector<double> intmes,
                    bool uncert,
                    FTAUtils::MapInt beEventsProb,
                    double & IE);
VecVecString beProb(std::map<std::string, std::vector<double>> & curr_b_nodes,
                    VecVecString events_prob_vector,
                    int n_sample,
                    int seed,
                    _analysis_t analysis,
                    std::vector<double> intmes,
                    bool uncert,
                    FTAUtils::MapInt beEventsProb,
                    double & IE);

/**
 * Computes probability for a given cut set on a per set basis based on
 * pre-flooded basic elem probs
 *
 * @param curr_b_nodes
 * @param cut_sets Cut sets container for in-place computations
 * @param n
 * @param bypass
 * @param bypass_key
 * @param bypass_value
 * @return probability
 */
VecVecDouble computeCutSetProb(std::map<std::string, std::vector<double>> curr_b_nodes,
                               SetSetString cut_sets,
                               int n,
                               bool bypass = false,
                               std::string bypass_key = "",
                               double bypass_value = 0);

/**
 * Computes probability for each row of a given cut set
 *
 * @param curr_b_nodes
 * @param row
 * @param n
 * @param sign_positive
 * @param bypass
 * @param bypass_key
 * @param bypass_value
 * @return
 */
std::vector<double> cutSetRowProb(std::map<std::string, std::vector<double>> curr_b_nodes,
                                  std::set<std::string> row,
                                  int n,
                                  bool sign_positive = true,
                                  bool bypass = false,
                                  std::string bypass_key = "",
                                  double bypass_value = 0);

/**
 * Computes accumulated probability for the entire cut set
 * 3 Digests are computed:
 *   0. Min Max
 *   1. Upper Bound
 *   2. Top Rare
 *
 * NOTE
 *   1. Its better to compute these 2 together as they have common loops
 *      which can save time
 *   2. Sets the vector cutSetProb
 *
 * @param curr_b_nodes
 * @param cut_sets
 * @param cut_set_prob
 * @param only_min_max
 * @return
 */
std::vector<double> * cutSetProbWDigest(std::map<std::string, std::vector<double>> curr_b_nodes,
                                        SetSetString cut_sets,
                                        VecVecDouble & cut_set_prob,
                                        int n,
                                        bool only_min_max = false,
                                        double IE = 1);

/**
 * Wrapper function for gate probability arith
 * AND => a * b
 * OR  => (1 - a) * (1 - b)
 *
 * @param a
 * @param b
 * @param is_and
 * @return
 */
double getGateProb(double a, double b, bool is_and);

/**
 * Computes cut-set Fussel-Vesely Important measures
 *
 * @param cut_set_prob
 * @param upperBound
 */
VecVecDouble minCutIM(VecVecDouble cut_set_prob, std::vector<double> upperBound);

/**
 * For each basic element:
 *   1. Calculate #occurrence in minimal cut set (beCount)
 *   2. Store row index of all occurrence (beIndex)
 *
 * @param curr_b_nodes
 * @param cut_sets
 * @param cut_set_prob
 * @param n
 * @param upper_bound
 * @param count_v
 * @param IE
 * @return
 */
MapVecVecDouble beIM(std::map<std::string, std::vector<double>> curr_b_nodes,
                     SetSetString cut_sets,
                     VecVecDouble cut_set_prob,
                     int n,
                     std::vector<double> upper_bound,
                     std::vector<int> & count_v,
                     double IE);

/**
 * Translator function for string -> double
 *
 * @param lines
 * @return
 */
VecVecDouble linesToDouble(VecVecString lines);

/**
 * Getting BIN mean values of intensity measure
 *
 * @param im_lower
 * @param im_upper
 * @param n_bins
 * @return
 */
std::vector<double> getBinMeans(double im_lower, double im_upper, int n_bins);

/**
 * Interpolating the hazard curve based on range of intensity measures
 *
 * @param hazard
 * @param im_bins
 * @return
 */
std::vector<double> hazInterp(VecVecDouble hazard, std::vector<double> im_bins);

/**
 * Function for top event fragility
 *
 * @param curr_b_nodes
 * @param cut_sets
 * @param n
 * @param im_bins
 * @param mu
 * @param sigma
 * @param cut_set_prob
 * @return
 */
std::vector<double> fragility(std::map<std::string, std::vector<double>> curr_b_nodes,
                              SetSetString cut_sets,
                              int n,
                              std::vector<double> im_bins,
                              double & mu,
                              double & sigma,
                              VecVecDouble & cut_set_prob);

/**
 * Calculate risk by convoluting hazard and fragility
 * Assign risk to basic event probabilites
 *
 * WARNING
 *   This consumes _b_nodes and then overwrites it
 *
 * @param curr_b_nodes
 * @param n
 * @param hazard
 */
void computeRisk(std::map<std::string, std::vector<double>> & curr_b_nodes,
                 int n,
                 std::vector<double> hazard);

/**
 * @param data_point
 * @param n
 * @param rv
 * @return
 */
std::vector<double> genQuantificationRVec(double data_point, int n, std::vector<double> rv);

/**
 * @param a
 * @param min
 * @param max
 */
double Clip(double a, double min, double max);

/**
 * @param x
 */
double normalCDF(double x);

/**
 * @param x
 * @param mu
 * @param sigma
 * @return
 */
double normalCDF(double x, double mu, double sigma);

/**
 * Compute interpolated value at x from parallel arrays ( xData, yData )
 * Assumes that xData has at least two elements, is sorted and is strictly
 * monotonic increasing boolean argument extrapolate determines behaviour beyond
 * ends of array (if needed)
 *
 * @param data
 * @param x
 * @param extrapolate
 * @return
 */
double interpolate(VecVecDouble data, double x, bool extrapolate);

} // namespace FTAUtils

#endif // _QUANTIFICATION_H
