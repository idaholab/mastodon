// MOOSE includes
#include "gtest/gtest.h"
#include "MastodonUtils.h"
#include "MooseUtils.h"

// Test for regularize function in MastodonUtils
TEST(MastodonUtils, Regularize)
{
  // Inputs for testing
  std::vector<Real> acc = {-1, 0, 1};
  std::vector<Real> tme = {0, 0.01, 0.02};
  // outputs for testing
  std::vector<Real> reg_acc = {-1, -0.5, 0, 0.5, 1};
  std::vector<Real> reg_tme = {0, 0.005, 0.01, 0.015, 0.02};
  std::vector<std::vector<Real>> reg_out;
  reg_out = MastodonUtils::regularize(acc, tme, 0.005);
  // Size check
  EXPECT_EQ(reg_out.size(), 2)
      << "Number of vectors returned by the function is different. Two vectors are expected.\n";
  // Value check
  for (std::size_t i = 0; i < reg_acc.size(); ++i)
  {
    EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(reg_out[0][i], reg_tme[i]))
        << "Time vector is different at index: " << i << ". Calculated: " << reg_out[0][i]
        << ". Expected: " << reg_tme[i] << ".\n";
    EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(reg_out[1][i], reg_acc[i]))
        << "Response vector is different at index: " << i << ". Calculated: " << reg_out[1][i]
        << ". Expected: " << reg_acc[i] << ".\n";
  }
}

// Test for responseSpectrum function in MastodonUtils
TEST(MastodonUtils, ResponseSpectrum)
{
  // Inputs for testing
  Real freq_start = 0.001;
  Real freq_end = 10;
  Real freq_num = 5;
  std::vector<Real> acc = {-0.1, 0, 0.1, 0, -0.1};
  Real xi = 0.05;
  Real reg_dt = 0.01;
  // Outputs for testing
  std::vector<Real> freq = {0.001, 0.01, 0.1, 1, 10};
  std::vector<Real> dspec = {4.49993e-05, 4.49929e-05, 4.49222e-05, 4.35994e-05, 1.7664e-05};
  std::vector<Real> vspec = {2.82739e-07, 2.82699e-06, 2.82255e-05, 0.000273943, 0.00110986};
  std::vector<Real> aspec = {1.7765e-09, 1.77625e-07, 1.77346e-05, 0.00172124, 0.0697346};
  std::vector<std::vector<Real>> respspec =
      MastodonUtils::responseSpectrum(freq_start, freq_end, freq_num, acc, xi, reg_dt);
  // Size check
  EXPECT_EQ(respspec.size(), 4)
      << "Number of vectors returned by the function is different. Four vectors are expected.\n";
  // Value check
  for (unsigned int i = 0; i < respspec[0].size(); ++i)
  {
    EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(respspec[0][i], freq[i]))
        << "Frequency vector is different at index: " << i << ". Calculated: " << respspec[0][i]
        << ". Expected: " << freq[i] << ".\n";
    EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(respspec[1][i], dspec[i], dspec[i] / 1000))
        << "Spectral displacement vector is different at index: " << i
        << ". Calculated: " << respspec[1][i] << ". Expected: " << dspec[i] << ".\n";
    EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(respspec[2][i], vspec[i], vspec[i] / 1000))
        << "Spectral displacement vector is different at index: " << i
        << ". Calculated: " << respspec[2][i] << ". Expected: " << vspec[i] << ".\n";
    EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(respspec[3][i], aspec[i], aspec[i] / 1000))
        << "Spectral displacement vector is different at index: " << i
        << ". Calculated: " << respspec[3][i] << ". Expected: " << aspec[i] << ".\n";
  }
}

TEST(MastodonUtils, glob)
{
  std::vector<std::string> results = MastodonUtils::glob("../test/tests/data/ground_motion_*.csv");
  EXPECT_EQ(results.size(), 10);
  EXPECT_EQ(results[0], "../test/tests/data/ground_motion_00.csv");
  EXPECT_EQ(results[1], "../test/tests/data/ground_motion_01.csv");
  EXPECT_EQ(results[2], "../test/tests/data/ground_motion_02.csv");
  EXPECT_EQ(results[3], "../test/tests/data/ground_motion_03.csv");
  EXPECT_EQ(results[4], "../test/tests/data/ground_motion_04.csv");
  EXPECT_EQ(results[5], "../test/tests/data/ground_motion_05.csv");
  EXPECT_EQ(results[6], "../test/tests/data/ground_motion_06.csv");
  EXPECT_EQ(results[7], "../test/tests/data/ground_motion_07.csv");
  EXPECT_EQ(results[8], "../test/tests/data/ground_motion_08.csv");
  EXPECT_EQ(results[9], "../test/tests/data/ground_motion_09.csv");
}

TEST(MastodonUtils, adjust)
{
  std::vector<Real> x = {1, 2, 3, 4};
  std::vector<Real> y = MastodonUtils::adjust(x, 2, 9);
  ASSERT_EQ(x.size(), y.size());
  for (std::size_t i = 0; i < x.size(); ++i)
    EXPECT_EQ(2 * x[i] + 9, y[i]);
}

TEST(MastodonUtils, log10)
{
  std::vector<Real> x = {1, 2, 3, 4};
  std::vector<Real> y = MastodonUtils::log10(x);
  ASSERT_EQ(x.size(), y.size());
  for (std::size_t i = 0; i < x.size(); ++i)
    EXPECT_EQ(std::log10(x[i]), y[i]);

  try
  {
    std::vector<Real> y = MastodonUtils::log10({0});
    FAIL();
  }
  catch (const std::exception & err)
  {
    std::string gold = "Cannot take the log of 0.";
    std::size_t pos = std::string(err.what()).find(gold);
    EXPECT_TRUE(pos != std::string::npos);
  }

  try
  {
    std::vector<Real> y = MastodonUtils::log10({1, 2, 3, -42});
    FAIL();
  }
  catch (const std::exception & err)
  {
    std::string gold = "Cannot take the log of -42.";
    std::size_t pos = std::string(err.what()).find(gold);
    EXPECT_TRUE(pos != std::string::npos);
  }
}
