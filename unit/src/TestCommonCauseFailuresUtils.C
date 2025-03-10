// MOOSE includes
#include "Conversion.h"
#include "MooseUtils.h"
#include "gtest/gtest.h"

// Boost distribution includes
#include "BoostDistribution.h"

// Eventree includes
#include <CommonCauseFailuresUtils.h>

TEST(FTAUtils, CCF)
{

  ///>>>>>>>>>>>>>>>Declaring variables to store results from ETA<<<<<<<<<<<<<<<<<<<<

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
                  invalid_file_name,
                  invalid_file_name,
                  invalid_file_name,
                  invalid_file_name,
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
                  empty_file_name,
                  empty_file_name,
                  empty_file_name,
                  empty_file_name,
                  empty_file_name);
  }
  catch (FTAUtils::FileException & e)
  {
    /*
     * ------- Type Error -------
     */
    std::string error_message(e.what());

    EXPECT_EQ(error_message, "Unable to open file.");
  }

  /*
   * Test checking fta, rrr & rir values for Sequence 1 using files E3_TE.txt, E3_Seq.txt,
   * be_prob_E3.txt,E3_CCF_List.txt,E3_CCF_alpha.txt,E3_CCF_Ft_Details.txt,hazard.txt
   */

  FTAUtils::ETA("E3_TE.txt",
                "E3_Seq.txt",
                dictQuantificationParamsDouble1,
                dictQuantificationParamsAnalysis1,
                dictQuantificationParamsInt1,
                dictQuantificationParamsString1,
                dictQuantificationParamsBool1,
                "be_prob_E3.txt",
                "E3_CCF_List.txt",
                "E3_CCF_alpha.txt",
                "E3_CCF_Ft_Details.txt");

  stream << dictQuantificationParamsDouble1["Seq1"]["fta"][0][0];
  std::string min_max_string = stream.str();
  EXPECT_EQ(min_max_string, "1.13033e-11");
  stream.str(std::string());
  // upper bound
  stream << dictQuantificationParamsDouble1["Seq1"]["fta"][0][1];
  std::string upper_bound_string = stream.str();
  EXPECT_EQ(upper_bound_string, "1.1304e-11");
  stream.str(std::string());
  // rare event
  stream << dictQuantificationParamsDouble1["Seq1"]["fta"][0][2];
  std::string rare_event_string = stream.str();
  EXPECT_EQ(rare_event_string, "1.1304e-11");
  stream.str(std::string());

  // Risk Reduction Ratio for B1, B2, B3, B4
  stream << dictQuantificationParamsDouble1["Seq1"]["rrr"][0][0];
  std::string rrr_1 = stream.str();
  EXPECT_EQ(rrr_1, "1.00965");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rrr"][1][0];
  std::string rrr_2 = stream.str();
  EXPECT_EQ(rrr_2, "-1");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rrr"][2][0];
  std::string rrr_3 = stream.str();
  EXPECT_EQ(rrr_3, "1.3138");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rrr"][3][0];
  std::string rrr_4 = stream.str();
  EXPECT_EQ(rrr_4, "4.02566");
  stream.str(std::string());

  // Risk Increase Ratio for B1, B2, B3, B4
  stream << dictQuantificationParamsDouble1["Seq1"]["rir"][0][0];
  std::string rir_1 = stream.str();
  EXPECT_EQ(rir_1, "3185.72");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rir"][1][0];
  std::string rir_2 = stream.str();
  EXPECT_EQ(rir_2, "2.77762e+07");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rir"][2][0];
  std::string rir_3 = stream.str();
  EXPECT_EQ(rir_3, "3185.49");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble1["Seq1"]["rir"][3][0];
  std::string rir_4 = stream.str();
  EXPECT_EQ(rir_4, "3184.97");
  stream.str(std::string());

  /*
   * Test checking fta, rri, rii & bi values for Sequence 2 using files E3_TE.txt, E3_Seq.txt,
   * sBE_prob.txt,E3_CCF_List.txt,E3_CCF_alpha.txt,E3_CCF_Ft_Details.txt,hazard.txt
   */

  FTAUtils::ETA("E3_TE.txt",
                "E3_Seq.txt",
                dictQuantificationParamsDouble2,
                dictQuantificationParamsAnalysis2,
                dictQuantificationParamsInt2,
                dictQuantificationParamsString2,
                dictQuantificationParamsBool2,
                "sBE_prob.txt",
                "E3_CCF_List.txt",
                "E3_CCF_alpha.txt",
                "E3_CCF_Ft_Details.txt");

  stream << dictQuantificationParamsDouble2["Seq2"]["fta"][0][0];
  min_max_string = stream.str();
  EXPECT_EQ(min_max_string, "0.0701117");
  stream.str(std::string());
  // upper bound
  stream << dictQuantificationParamsDouble2["Seq2"]["fta"][0][1];
  upper_bound_string = stream.str();
  EXPECT_EQ(upper_bound_string, "0.070112");
  stream.str(std::string());
  // rare event
  stream << dictQuantificationParamsDouble2["Seq2"]["fta"][0][2];
  rare_event_string = stream.str();
  EXPECT_EQ(rare_event_string, "0.0711944");
  stream.str(std::string());

  // >>>>>>>> IMdiff Value Check <<<<<<<<

  // Risk Reduction Difference for B1, B2, B3
  stream << dictQuantificationParamsDouble2["Seq2"]["rri"][0][0];
  std::string rri_1 = stream.str();
  EXPECT_EQ(rri_1, "0.00111199");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rri"][1][0];
  std::string rri_2 = stream.str();
  EXPECT_EQ(rri_2, "0.00109702");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rri"][2][0];
  std::string rri_3 = stream.str();
  EXPECT_EQ(rri_3, "1.49528e-05");
  stream.str(std::string());

  // Risk Increase Difference for B1, B2, B3
  stream << dictQuantificationParamsDouble2["Seq2"]["rii"][0][0];
  std::string rii_1 = stream.str();
  EXPECT_EQ(rii_1, "0.0174165");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rii"][1][0];
  std::string rii_2 = stream.str();
  EXPECT_EQ(rii_2, "0.0547621");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["rii"][2][0];
  std::string rii_3 = stream.str();
  EXPECT_EQ(rii_3, "0.0557792");
  stream.str(std::string());

  // Birnbaum Difference for B1, B2, B3
  stream << dictQuantificationParamsDouble2["Seq2"]["bi"][0][0];
  std::string bi_1 = stream.str();
  EXPECT_EQ(bi_1, "0.0185285");
  stream.str(std::string());

  stream << dictQuantificationParamsDouble2["Seq2"]["bi"][1][0];
  std::string bi_2 = stream.str();
  EXPECT_EQ(bi_2, "0.0558591");
  stream.str(std::string());
  stream << dictQuantificationParamsDouble2["Seq2"]["bi"][2][0];
  std::string bi_3 = stream.str();
  EXPECT_EQ(bi_3, "0.0557942");
  stream.str(std::string());
}