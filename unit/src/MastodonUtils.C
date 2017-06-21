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
  for (int i = 0; i < reg_acc.size(); ++i)
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
