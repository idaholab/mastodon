// MOOSE includes
#include "gtest/gtest.h"
#include "MooseUtils.h"
#include "Conversion.h"

// Boost distribution includes
#include "BoostDistribution.h"

// MASTODON includes
#include "MastodonUtils.h"

// QUANTIFICATIONUTILS includes
#include "QuantificationUtils.h"

// For input
std::vector<FileName> file_lists_q;
std::vector<double> IM;
int nbins;
bool uncertainty;
int nsamp;
int seed;

// For output
std::map<std::string, std::vector<std::vector<double>>> params_double_1, params_double_2,
    params_double_3, params_double_4, params_double_5, params_double_6, params_double_7,
    params_double_8, params_double_9;

std::map<std::string, std::vector<std::vector<std::string>>> params_string_1, params_string_2,
    params_string_3, params_string_4, params_string_5, params_string_6, params_string_7,
    params_string_8, params_string_9;

std::map<std::string, bool> params_bool_1, params_bool_2, params_bool_3, params_bool_4,
    params_bool_5, params_bool_6, params_bool_7, params_bool_8, params_bool_9;

std::map<std::string, FTAUtils::Quantification::_analysis_t> params_analysis_t_1,
    params_analysis_t_2, params_analysis_t_3, params_analysis_t_4, params_analysis_t_5,
    params_analysis_t_6, params_analysis_t_7, params_analysis_t_8, params_analysis_t_9;

std::map<std::string, int> params_int_1, params_int_2, params_int_3, params_int_4, params_int_5,
    params_int_6, params_int_7, params_int_8, params_int_9;

// TestCase for Quantification Object
TEST(FTAUtils, Quantification)
{

  // ===========================================================================
  // =============================== Test Inputs ===============================
  // ===========================================================================

  // +++++++++++++++++++++++ Input values +++++++++++++++++++++++

  // ############### File Inputs ###############

  try
  {
    file_lists_q = {
        "not_a_valid_filename.txt", "not_a_valid_filename.txt", "not_a_valid_filename.txt"};
    IM = {0.1, 4};
    nbins = 15;

    FTAUtils::Quantification(params_double_1,
                             params_string_1,
                             params_int_1,
                             params_bool_1,
                             params_analysis_t_1,
                             file_lists_q[0],
                             file_lists_q[1],
                             FTAUtils::Quantification::RISK,
                             file_lists_q[2],
                             IM[0],
                             IM[1],
                             nbins);
  }
  catch (FTAUtils::CException e)
  {
    /*
     * ------- IO Error -------
     * does not exist
     */
    EXPECT_EQ(e.msg, "Unable to open file.");
  }

  try
  {
    file_lists_q = {"", "", ""};
    IM = {0.1, 4};
    nbins = 15;

    FTAUtils::Quantification(params_double_2,
                             params_string_2,
                             params_int_2,
                             params_bool_2,
                             params_analysis_t_2,
                             file_lists_q[0],
                             file_lists_q[1],
                             FTAUtils::Quantification::RISK,
                             file_lists_q[2],
                             IM[0],
                             IM[1],
                             nbins);
  }
  catch (FTAUtils::CException e)
  {
    /*
     * ------- Type Error -------
     * must be a filename or a list
     */
    EXPECT_EQ(e.msg, "Unable to open file.");
  }

  // ############## Inputs for Fragility ##############

  file_lists_q = {"logic1.txt", "logic1_bas_events_LNORM.txt", "hazard.txt"};
  IM = {0.1, 4};
  nbins = 15;
  FTAUtils::Quantification(params_double_3,
                           params_string_3,
                           params_int_3,
                           params_bool_3,
                           params_analysis_t_3,
                           file_lists_q[0],
                           file_lists_q[1],
                           FTAUtils::Quantification::FRAGILITY,
                           file_lists_q[2],
                           IM[0],
                           IM[1],
                           nbins);

  // >>>>>>>> logic Value Check <<<<<<<<

  std::vector<std::vector<std::string>> event_files_matrix3{{"TE", "OR", "IE3", "IE4"},
                                                            {"IE4", "OR", "C4"},
                                                            {"IE3", "OR", "C3", "IE2"},
                                                            {"IE2", "AND", "C2", "IE1"},
                                                            {"IE1", "OR", "C1"}};
  EXPECT_EQ(params_string_3["events_files"], event_files_matrix3);

  // >>>>>>>> Basic Events Value Check <<<<<<<<

  std::vector<std::vector<std::string>> basic_events_matrix3{{"C1", "LNORM", "1.88", "0.5"},
                                                             {"C2", "LNORM", "3.78", "0.79"},
                                                             {"C3", "LNORM", "2.33", "0.76"},
                                                             {"C4", "LNORM", "3.66", "0.45"}};
  std::vector<std::vector<std::string>> basic_events_3 = params_string_3["basic_events"];
  EXPECT_EQ(basic_events_3, basic_events_matrix3);

  // >>>>>>>> antype Value Check <<<<<<<<

  EXPECT_EQ(params_analysis_t_3["analysis"], FTAUtils::Quantification::FRAGILITY);

  // >>>>>>>> hazard Value Check <<<<<<<<

  std::vector<std::vector<double>> matrix_hazard{{0.0608, 0.01},
                                                 {0.2124, 0.001},
                                                 {0.4, 0.0001},
                                                 {0.629, 1e-05},
                                                 {0.9344, 1e-06},
                                                 {1.3055, 1e-07}};
  std::vector<std::vector<double>> hazard = params_double_3["hazard"];
  EXPECT_EQ(hazard, matrix_hazard);

  // >>>>>>>> imrange Value Check <<<<<<<<

  EXPECT_EQ(params_double_3["IM"][0][0], 0.1);
  EXPECT_EQ(params_double_3["IM"][0][1], 4);

  // >>>>>>>> nbins Value Check <<<<<<<<

  EXPECT_EQ(params_int_3["n_bins"], 15);

  // >>>>>>>> nbins Value Check <<<<<<<<

  try
  {
    file_lists_q = {"logic1.txt", "logic1_bas_events_LNORM.txt", "hazard.txt"};
    IM = {0.1, 4};

    nbins = -15;
    if (!(nbins > 0))
      throw FTAUtils::CException("ValueError");

    FTAUtils::Quantification(params_double_4,
                             params_string_4,
                             params_int_4,
                             params_bool_4,
                             params_analysis_t_4,
                             file_lists_q[0],
                             file_lists_q[1],
                             FTAUtils::Quantification::FRAGILITY,
                             file_lists_q[2],
                             IM[0],
                             IM[1],
                             nbins);
  }
  catch (FTAUtils::CException e)
  {
    /*
     * ------- Value Error -------
     * The supplied value of nbins must be a +ve integer
     */
    EXPECT_EQ(e.msg, "ValueError");
  }

  // ############## Inputs for Risk analysis (not fragility) ##############

  file_lists_q = {"logic2.txt", "logic2_bas_events_PE.txt"};

  FTAUtils::Quantification(params_double_5,
                           params_string_5,
                           params_int_5,
                           params_bool_5,
                           params_analysis_t_5,
                           file_lists_q[0],
                           file_lists_q[1],
                           FTAUtils::Quantification::RISK);

  // >>>>>>>> logic Value Check <<<<<<<<

  std::vector<std::vector<std::string>> event_files_matrix5{
      {"TOP", "AND", "GATE1", "GATE2"},
      {"GATE1", "OR", "FT-N/m-1", "FT-N/m-2", "FT-N/m-3"},
      {"GATE2", "OR", "B1", "B3", "B4"},
      {"GATE3", "OR", "B2", "B4"},
      {"FT-N/m-1", "AND", "GATE3", "B3", "B5"},
      {"FT-N/m-2", "AND", "GATE3", "B1"},
      {"FT-N/m-3", "AND", "B3", "B5", "B1"}};

  EXPECT_EQ(params_string_5["events_files"], event_files_matrix5);

  // >>>>>>>> Basic Events Value Check <<<<<<<<

  std::vector<std::vector<std::string>> basic_events_matrix5{{"B1", "PE", "0.01"},
                                                             {"B2", "PE", "0.02"},
                                                             {"B3", "PE", "0.03"},
                                                             {"B4", "PE", "0.04"},
                                                             {"B5", "PE", "0.05"}};

  std::vector<std::vector<std::string>> basic_events_5 = params_string_5["basic_events"];

  EXPECT_EQ(basic_events_5, basic_events_matrix5);

  // >>>>>>>> antype Value Check <<<<<<<<

  EXPECT_NE(params_analysis_t_5["analysis"], FTAUtils::Quantification::FRAGILITY);

  // >>>>>>>> uncertainty Value Check <<<<<<<<

  EXPECT_EQ(params_bool_5["uncertainty"], false);

  // >>>>>>>> nsamp Value Check <<<<<<<<

  EXPECT_EQ(params_int_5["nsamp"], 1);

  // >>>>>>>> seed Value Check <<<<<<<<

  EXPECT_EQ(params_int_5["seed"], 0);

  // ###### Testing for input errors making sure parameters are input correctly ######

  file_lists_q = {"logic2.txt", "logic2_bas_events_PE.txt", "hazard.txt"};
  IM = {0.1, 4};
  nbins = 15;
  uncertainty = true;
  nsamp = 1000;
  seed = 436546754;

  FTAUtils::Quantification(params_double_6,
                           params_string_6,
                           params_int_6,
                           params_bool_6,
                           params_analysis_t_6,
                           file_lists_q[0],
                           file_lists_q[1],
                           FTAUtils::Quantification::RISK,
                           file_lists_q[2],
                           IM[0],
                           IM[1],
                           nbins,
                           uncertainty,
                           "",
                           nsamp,
                           seed);

  // >>>>>>>> uncertainty Value Check <<<<<<<<

  EXPECT_EQ(params_bool_6["uncertainty"], true);

  // >>>>>>>> nsamp Value Check <<<<<<<<

  EXPECT_EQ(params_int_6["nsamp"], 1000);

  // >>>>>>>> seed Value Check <<<<<<<<

  EXPECT_EQ(params_int_6["seed"], 436546754);

  // ############## Testing for input type errors and value errors ##############

  // >>>>>>>> nsamp Value Check <<<<<<<<

  try
  {
    file_lists_q = {"logic2.txt", "logic2_bas_events_PE.txt", "hazard.txt"};
    IM = {0.1, 4};
    nbins = 15;
    uncertainty = true;

    nsamp = -10;
    if (!(nsamp > 0))
      throw FTAUtils::CException("ValueError");

    seed = 42.0;

    FTAUtils::Quantification(params_double_7,
                             params_string_7,
                             params_int_7,
                             params_bool_7,
                             params_analysis_t_7,
                             file_lists_q[0],
                             file_lists_q[1],
                             FTAUtils::Quantification::RISK,
                             file_lists_q[2],
                             IM[0],
                             IM[1],
                             nbins,
                             uncertainty,
                             "",
                             nsamp,
                             seed);
  }
  catch (FTAUtils::CException e)
  {
    /*
     * ------- Value Error -------
     * The supplied value of nsamp must be a +ve integer.
     */
    EXPECT_EQ(e.msg, "ValueError");
  }

  // >>>>>>>> seed Value Check <<<<<<<<

  try
  {
    file_lists_q = {"logic2.txt", "logic2_bas_events_PE.txt", "hazard.txt"};
    IM = {0.1, 4};
    nbins = 15;
    uncertainty = true;
    nsamp = 10;

    seed = -42;
    if (!(seed > 0))
      throw FTAUtils::CException("ValueError");

    FTAUtils::Quantification(params_double_8,
                             params_string_8,
                             params_int_8,
                             params_bool_8,
                             params_analysis_t_8,
                             file_lists_q[0],
                             file_lists_q[1],
                             FTAUtils::Quantification::RISK,
                             file_lists_q[2],
                             IM[0],
                             IM[1],
                             nbins,
                             uncertainty,
                             "",
                             nsamp,
                             seed);
  }
  catch (FTAUtils::CException e)
  {
    /*
     * ------- Value Error -------
     * The supplied value of seed must be a +ve integer.
     */
    EXPECT_EQ(e.msg, "ValueError");
  }

  // ===========================================================================
  // ============================== Test TOP Risk ==============================
  // ===========================================================================

  // ############ Function for asserting FTA top event risk ############

  // ++++++++++ Check FTA top event risk ++++++++++

  file_lists_q = {"logic2.txt", "logic2_bas_events_PE.txt", "hazard.txt"};
  IM = {0.1, 4};
  nbins = 15;
  FTAUtils::Quantification(params_double_9,
                           params_string_9,
                           params_int_9,
                           params_bool_9,
                           params_analysis_t_9,
                           file_lists_q[0],
                           file_lists_q[1],
                           FTAUtils::Quantification::RISK,
                           file_lists_q[2],
                           IM[0],
                           IM[1],
                           nbins);

  // >>>>>>>> Risk Value Check <<<<<<<<

  // min-max
  EXPECT_EQ(std::to_string(params_double_9["fta"][0][0]), "0.000694");

  // upper bound
  EXPECT_EQ(std::to_string(params_double_9["fta"][0][1]), "0.000705");

  // rare event
  EXPECT_EQ(std::to_string(params_double_9["fta"][0][2]), "0.000705");

  // >>>>>>>> IMratio Value Check <<<<<<<<

  // Fussell-Vesely for B1, B2, B3, B4, B5
  EXPECT_EQ(std::to_string(params_double_9["fv"][0][0]), "0.872395");
  EXPECT_EQ(std::to_string(params_double_9["fv"][1][0]), "0.326300");
  EXPECT_EQ(std::to_string(params_double_9["fv"][2][0]), "0.148963");
  EXPECT_EQ(std::to_string(params_double_9["fv"][3][0]), "0.652584");
  EXPECT_EQ(std::to_string(params_double_9["fv"][4][0]), "0.148963");

  // Risk Reduction Ratio for B1, B2, B3, B4, B5
  EXPECT_EQ(std::to_string(params_double_9["rrr"][0][0]), "7.831866");
  EXPECT_EQ(std::to_string(params_double_9["rrr"][1][0]), "1.483999");
  EXPECT_EQ(std::to_string(params_double_9["rrr"][2][0]), "1.174913");
  EXPECT_EQ(std::to_string(params_double_9["rrr"][3][0]), "2.877066");
  EXPECT_EQ(std::to_string(params_double_9["rrr"][4][0]), "1.174913");

  // Risk Increase Ratio for B1, B2, B3, B4, B5
  EXPECT_EQ(std::to_string(params_double_9["rir"][0][0]), "86.111103");
  EXPECT_EQ(std::to_string(params_double_9["rir"][1][0]), "16.960273");
  EXPECT_EQ(std::to_string(params_double_9["rir"][2][0]), "5.808755");
  EXPECT_EQ(std::to_string(params_double_9["rir"][3][0]), "16.637742");
  EXPECT_EQ(std::to_string(params_double_9["rir"][4][0]), "3.826894");

  // >>>>>>>> IMdiff Value Check <<<<<<<<

  // Risk Reduction Difference for B1, B2, B3, B4, B5
  EXPECT_EQ(std::to_string(params_double_9["rri"][0][0]), "0.000615");
  EXPECT_EQ(std::to_string(params_double_9["rri"][1][0]), "0.000230");
  EXPECT_EQ(std::to_string(params_double_9["rri"][2][0]), "0.000105");
  EXPECT_EQ(std::to_string(params_double_9["rri"][3][0]), "0.000460");
  EXPECT_EQ(std::to_string(params_double_9["rri"][4][0]), "0.000105");

  // Risk Increase Difference for B1, B2, B3, B4, B5
  EXPECT_EQ(std::to_string(params_double_9["rii"][0][0]), "0.059991");
  EXPECT_EQ(std::to_string(params_double_9["rii"][1][0]), "0.011250");
  EXPECT_EQ(std::to_string(params_double_9["rii"][2][0]), "0.003389");
  EXPECT_EQ(std::to_string(params_double_9["rii"][3][0]), "0.011022");
  EXPECT_EQ(std::to_string(params_double_9["rii"][4][0]), "0.001993");

  // Birnbaum Difference for B1, B2, B3, B4, B5
  EXPECT_EQ(std::to_string(params_double_9["bi"][0][0]), "0.060606");
  EXPECT_EQ(std::to_string(params_double_9["bi"][1][0]), "0.011480");
  EXPECT_EQ(std::to_string(params_double_9["bi"][2][0]), "0.003494");
  EXPECT_EQ(std::to_string(params_double_9["bi"][3][0]), "0.011482");
  EXPECT_EQ(std::to_string(params_double_9["bi"][4][0]), "0.002097");
}
