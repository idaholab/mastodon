// MOOSE includes
#include "Conversion.h"
#include "MooseUtils.h"
#include "gtest/gtest.h"

// Boost distribution includes
#include "BoostDistribution.h"

// Eventree includes
#include <EventTreeUtils.h>

TEST(FTAUtils, ETA)
{
  //>>>>>>>>>>>>>>>Declaring & defining variables to store file names & to store results from
  // ETA<<<<<<<<<<<<<<<<<<<<

  // Storing file top event test file names
  FTAUtils::VecString topEventFiles = {"E1_TE.txt", "E2_TE.txt"};

  // Storing sequence test file names
  FTAUtils::VecString seqFiles = {"E1_Seq.txt", "E2_Seq.txt"};

  // Storing probability file names
  FTAUtils::VecString probFiles = {"be_prob_E1E2.txt", "sBE_prob.txt"};

  std::stringstream stream;

  std::map<std::string, FTAUtils::MapVecVecDouble> dictQuantificationParamsDouble0;
  std::map<std::string, FTAUtils::MapAnalysis> dictQuantificationParamsAnalysis0;
  std::map<std::string, FTAUtils::MapInt> dictQuantificationParamsInt0;
  std::map<std::string, FTAUtils::MapVecVecString> dictQuantificationParamsString0;
  std::map<std::string, FTAUtils::MapBool> dictQuantificationParamsBool0;

  std::map<std::string, FTAUtils::MapVecVecDouble> dictQuantificationParamsDouble1;
  std::map<std::string, FTAUtils::MapAnalysis> dictQuantificationParamsAnalysis1;
  std::map<std::string, FTAUtils::MapInt> dictQuantificationParamsInt1;
  std::map<std::string, FTAUtils::MapVecVecString> dictQuantificationParamsString1;
  std::map<std::string, FTAUtils::MapBool> dictQuantificationParamsBool1;

  std::map<std::string, FTAUtils::MapVecVecDouble> dictQuantificationParamsDouble2;
  std::map<std::string, FTAUtils::MapAnalysis> dictQuantificationParamsAnalysis2;
  std::map<std::string, FTAUtils::MapInt> dictQuantificationParamsInt2;
  std::map<std::string, FTAUtils::MapVecVecString> dictQuantificationParamsString2;
  std::map<std::string, FTAUtils::MapBool> dictQuantificationParamsBool2;

  std::map<std::string, FTAUtils::MapVecVecDouble> dictQuantificationParamsDouble3;
  std::map<std::string, FTAUtils::MapAnalysis> dictQuantificationParamsAnalysis3;
  std::map<std::string, FTAUtils::MapInt> dictQuantificationParamsInt3;
  std::map<std::string, FTAUtils::MapVecVecString> dictQuantificationParamsString3;
  std::map<std::string, FTAUtils::MapBool> dictQuantificationParamsBool3;

  //>>>>>>>>>>>>>>>Test for invalid inputs>>>>>>>>>>>>>>>>>>>>>>>

  try
  {
    std::string invalid_file_name = "not_a_valid_file_name.txt";

    FTAUtils::ETA(invalid_file_name,
                  invalid_file_name,
                  dictQuantificationParamsDouble0,
                  dictQuantificationParamsAnalysis0,
                  dictQuantificationParamsInt0,
                  dictQuantificationParamsString0,
                  dictQuantificationParamsBool0,
                  invalid_file_name);
  }
  catch (FTAUtils::FileException & e)
  {
    /*
     * ------- IO Error -------
     */
    std::string error_message(e.what());

    EXPECT_EQ(error_message, "Unable to open file.");
  }

  try
  {
    std::string empty_file_name = "";
    FTAUtils::ETA(empty_file_name,
                  empty_file_name,
                  dictQuantificationParamsDouble0,
                  dictQuantificationParamsAnalysis0,
                  dictQuantificationParamsInt0,
                  dictQuantificationParamsString0,
                  dictQuantificationParamsBool0,
                  empty_file_name);
  }
  catch (FTAUtils::FileException & e)
  {
    /*
     * ------ IO Error ---------
     */
    std::string error_message(e.what());

    EXPECT_EQ(error_message, "Unable to open file.");
  }

  /*
   * Test checking all values for Sequence 1 using files E1_TE, E1_Seq & be_prob_E1E2
   */

  FTAUtils::ETA(topEventFiles[0],
                seqFiles[0],
                dictQuantificationParamsDouble1,
                dictQuantificationParamsAnalysis1,
                dictQuantificationParamsInt1,
                dictQuantificationParamsString1,
                dictQuantificationParamsBool1,
                probFiles[0]);

  // >>>>>>>> logic Value Check <<<<<<<<

  std::vector<std::vector<std::string>> event_files_matrix_seq1{
      {"Seq1", "OR", "Seq1_Gate1", "Seq1_Gate2"},
      {"Seq1_Gate1", "AND", "D", "E"},
      {"Seq1_Gate2", "AND", "D", "F"},
  };
  EXPECT_EQ(dictQuantificationParamsString1["Seq1"]["events_files"], event_files_matrix_seq1);

  // >>>>>>>> Basic Events Value Check <<<<<<<<

  std::vector<std::vector<std::string>> basic_events_matrix_seq1{
      {"D", "PE", "3.60E-8"},
      {"E", "PE", "7.50E-5"},
      {"F", "PE", "5.40E-4"},
  };
  std::vector<std::vector<std::string>> basic_events_3 =
      dictQuantificationParamsString1["Seq1"]["basic_events"];
  EXPECT_EQ(basic_events_3, basic_events_matrix_seq1);

  // >>>>>>>> antype Value Check <<<<<<<<

  EXPECT_EQ(dictQuantificationParamsAnalysis1["Seq1"]["analysis"], FTAUtils::RISK);

  // >>>>>>>> imrange Value Check <<<<<<<<

  EXPECT_EQ(dictQuantificationParamsDouble1["Seq1"]["IM"][0][0], 0.1);
  EXPECT_EQ(dictQuantificationParamsDouble1["Seq1"]["IM"][0][1], 4);

  // >>>>>>>> nbins Value Check <<<<<<<<

  EXPECT_EQ(dictQuantificationParamsInt1["Seq1"]["n_bins"], 15);

  // >>>>>>>> uncertainty Value Check <<<<<<<<

  EXPECT_EQ(dictQuantificationParamsBool1["Seq1"]["uncertainty"], false);

  // >>>>>>>> nsamp Value Check <<<<<<<<

  EXPECT_EQ(dictQuantificationParamsInt1["Seq1"]["nsamp"], 1);

  // >>>>>>>> seed Value Check <<<<<<<<

  EXPECT_EQ(dictQuantificationParamsInt1["Seq1"]["seed"], 0);

  // >>>>>>>> Risk Value Check <<<<<<<<

  // min-max

  stream << dictQuantificationParamsDouble1["Seq1"]["fta"][0][0];
  std::string min_max_string = stream.str();
  EXPECT_EQ(min_max_string, "2.21385e-11");
  stream.str(std::string());
  // upper bound
  stream << dictQuantificationParamsDouble1["Seq1"]["fta"][0][1];
  std::string upper_bound_string = stream.str();
  EXPECT_EQ(upper_bound_string, "2.214e-11");
  stream.str(std::string());
  // rare event
  stream << dictQuantificationParamsDouble1["Seq1"]["fta"][0][2];
  std::string rare_event_string = stream.str();
  EXPECT_EQ(rare_event_string, "2.214e-11");
  stream.str(std::string());

  // >>>>>>>> IMratio Value Check <<<<<<<<

  // Fussell-Vesely for B1, B2, B3

  stream << dictQuantificationParamsDouble1["Seq1"]["fv"][0][0];
  std::string fv_1 = stream.str();
  EXPECT_EQ(fv_1, "1");
  stream.str("");

  stream << dictQuantificationParamsDouble1["Seq1"]["fv"][1][0];
  std::string fv_2 = stream.str();
  EXPECT_EQ(fv_2, "0.121949");
  stream.str("");

  stream << dictQuantificationParamsDouble1["Seq1"]["fv"][2][0];
  std::string fv_3 = stream.str();
  EXPECT_EQ(fv_3, "0.878051");
  stream.str("");

  // Risk Reduction Ratio for B1, B2, B3
  stream << dictQuantificationParamsDouble1["Seq1"]["rrr"][0][0];
  std::string rrr_1 = stream.str();
  EXPECT_EQ(rrr_1, "-1");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rrr"][1][0];
  std::string rrr_2 = stream.str();
  EXPECT_EQ(rrr_2, "1.13889");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rrr"][2][0];
  std::string rrr_3 = stream.str();
  EXPECT_EQ(rrr_3, "8.20013");
  stream.str(std::string());

  // Risk Increase Ratio for B1, B2, B3
  stream << dictQuantificationParamsDouble1["Seq1"]["rir"][0][0];
  std::string rir_1 = stream.str();
  EXPECT_EQ(rir_1, "2.7776e+07");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rir"][1][0];
  std::string rir_2 = stream.str();
  EXPECT_EQ(rir_2, "1626.9");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rir"][2][0];
  std::string rir_3 = stream.str();
  EXPECT_EQ(rir_3, "1626.14");
  stream.str(std::string());
  // >>>>>>>> IMdiff Value Check <<<<<<<<

  // Risk Reduction Difference for B1, B2, B3
  stream << dictQuantificationParamsDouble1["Seq1"]["rri"][0][0];
  std::string rri_1 = stream.str();
  EXPECT_EQ(rri_1, "2.214e-11");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rri"][1][0];
  std::string rri_2 = stream.str();
  EXPECT_EQ(rri_2, "2.69995e-12");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rri"][2][0];
  std::string rri_3 = stream.str();
  EXPECT_EQ(rri_3, "1.944e-11");
  stream.str(std::string());

  // Risk Increase Difference for B1, B2, B3
  stream << dictQuantificationParamsDouble1["Seq1"]["rii"][0][0];
  std::string rii_1 = stream.str();
  EXPECT_EQ(rii_1, "0.000614959");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rii"][1][0];
  std::string rii_2 = stream.str();
  EXPECT_EQ(rii_2, "3.59973e-08");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rii"][2][0];
  std::string rii_3 = stream.str();
  EXPECT_EQ(rii_3, "3.59806e-08");
  stream.str(std::string());

  // Birnbaum Difference for B1, B2, B3
  stream << dictQuantificationParamsDouble1["Seq1"]["bi"][0][0];
  std::string bi_1 = stream.str();
  EXPECT_EQ(bi_1, "0.00061496");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["bi"][1][0];
  std::string bi_2 = stream.str();
  EXPECT_EQ(bi_2, "3.6e-08");
  stream.str(std::string());
  stream << dictQuantificationParamsDouble1["Seq1"]["bi"][2][0];
  std::string bi_3 = stream.str();
  EXPECT_EQ(bi_3, "3.6e-08");
  stream.str(std::string());

  /*
   * Test checking fta, rrr & rir values for Sequence 2 using files E2_TE, E2_Seq & be_prob_E1E2
   */

  FTAUtils::ETA(topEventFiles[1],
                seqFiles[1],
                dictQuantificationParamsDouble2,
                dictQuantificationParamsAnalysis2,
                dictQuantificationParamsInt2,
                dictQuantificationParamsString2,
                dictQuantificationParamsBool2,
                probFiles[0]);

  // >>>>>>>> Risk Value Check <<<<<<<<

  // min-max

  stream << dictQuantificationParamsDouble2["Seq2"]["fta"][0][0];
  min_max_string = stream.str();
  EXPECT_EQ(min_max_string, "0.000100098");
  stream.str(std::string());
  // upper bound
  stream << dictQuantificationParamsDouble2["Seq2"]["fta"][0][1];
  upper_bound_string = stream.str();
  EXPECT_EQ(upper_bound_string, "0.000100098");
  stream.str(std::string());
  // rare event
  stream << dictQuantificationParamsDouble2["Seq2"]["fta"][0][2];
  rare_event_string = stream.str();
  EXPECT_EQ(rare_event_string, "0.000100098");
  stream.str(std::string());

  // Risk Reduction Ratio for B1, B2, B3, B4
  stream << dictQuantificationParamsDouble2["Seq2"]["rrr"][0][0];
  rrr_1 = stream.str();
  EXPECT_EQ(rrr_1, "1.00062");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rrr"][1][0];
  rrr_2 = stream.str();
  EXPECT_EQ(rrr_2, "1.00062");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rrr"][2][0];
  rrr_3 = stream.str();
  EXPECT_EQ(rrr_3, "1017.26");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rrr"][3][0];
  std::string rrr_4 = stream.str();
  EXPECT_EQ(rrr_4, "1.00036");
  stream.str(std::string());

  // Risk Increase Ratio for B1, B2, B3, B4
  stream << dictQuantificationParamsDouble2["Seq2"]["rir"][0][0];
  rir_1 = stream.str();
  EXPECT_EQ(rir_1, "3.59656");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rir"][1][0];
  rir_2 = stream.str();
  EXPECT_EQ(rir_2, "3.39678");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rir"][2][0];
  rir_3 = stream.str();
  EXPECT_EQ(rir_3, "9990.17");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rir"][3][0];
  std::string rir_4 = stream.str();
  EXPECT_EQ(rir_4, "9990.17");
  stream.str(std::string());

  /*
   * Test checking fta, rri, rii & bi values for Sequence 3 using files E2_TE, E2_Seq & sBE_prob
   */

  FTAUtils::ETA(topEventFiles[1],
                seqFiles[1],
                dictQuantificationParamsDouble3,
                dictQuantificationParamsAnalysis3,
                dictQuantificationParamsInt3,
                dictQuantificationParamsString3,
                dictQuantificationParamsBool3,
                probFiles[1]);

  stream << dictQuantificationParamsDouble3["Seq3"]["fta"][0][0];
  min_max_string = stream.str();
  EXPECT_EQ(min_max_string, "0.00456633");
  stream.str(std::string());
  // upper bound
  stream << dictQuantificationParamsDouble3["Seq3"]["fta"][0][1];
  upper_bound_string = stream.str();
  EXPECT_EQ(upper_bound_string, "0.00464218");
  stream.str(std::string());
  // rare event
  stream << dictQuantificationParamsDouble3["Seq3"]["fta"][0][2];
  rare_event_string = stream.str();
  EXPECT_EQ(rare_event_string, "0.00464634");
  stream.str(std::string());

  // >>>>>>>> IMdiff Value Check <<<<<<<<

  // Risk Reduction Difference for B1, B2, B3
  stream << dictQuantificationParamsDouble3["Seq3"]["rri"][0][0];
  rri_1 = stream.str();
  EXPECT_EQ(rri_1, "1.12875e-05");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble3["Seq3"]["rri"][1][0];
  rri_2 = stream.str();
  EXPECT_EQ(rri_2, "1.12875e-05");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble3["Seq3"]["rri"][2][0];
  rri_3 = stream.str();
  EXPECT_EQ(rri_3, "0.000134388");
  stream.str(std::string());

  // Risk Increase Difference for B1, B2, B3
  stream << dictQuantificationParamsDouble3["Seq3"]["rii"][0][0];
  rii_1 = stream.str();
  EXPECT_EQ(rii_1, "0.000176831");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble3["Seq3"]["rii"][1][0];
  rii_2 = stream.str();
  EXPECT_EQ(rii_2, "0.000149958");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble3["Seq3"]["rii"][2][0];
  rii_3 = stream.str();
  EXPECT_EQ(rii_3, "0.00255219");
  stream.str(std::string());

  // Birnbaum Difference for B1, B2, B3
  stream << dictQuantificationParamsDouble3["Seq3"]["bi"][0][0];
  bi_1 = stream.str();
  EXPECT_EQ(bi_1, "0.000188119");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble3["Seq3"]["bi"][1][0];
  bi_2 = stream.str();
  EXPECT_EQ(bi_2, "0.000161245");
  stream.str(std::string());
  stream << dictQuantificationParamsDouble3["Seq3"]["bi"][2][0];
  bi_3 = stream.str();
  EXPECT_EQ(bi_3, "0.00268657");
  stream.str(std::string());
}