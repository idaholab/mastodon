// MOOSE includes
#include "gtest/gtest.h"
#include "MooseUtils.h"
#include "Conversion.h"

// Boost distribution includes
#include "BoostDistribution.h"

// MASTODON includes
#include "MastodonUtils.h"

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

// Test for checkEqualSize function in MastodonUtils
TEST(MastodonUtils, checkEqualSize)
{
  // Inputs for testing
  std::vector<Real> vec1 = {-1, 0, 1};
  std::vector<Real> vec2 = {-1, 0, 1};
  std::vector<Real> vec3 = {-1, 0, 1};
  std::vector<Real> vec4 = {-1, 0, 1, 2};
  std::vector<std::vector<Real>> vvec1 = {vec1, vec2, vec3};
  std::vector<std::vector<Real>> vvec2 = {vec1, vec2, vec4};
  // Outputs for testing
  //   None
  // Value check
  EXPECT_FALSE(MastodonUtils::checkEqualSize(vvec2));
  EXPECT_TRUE(MastodonUtils::checkEqualSize(vvec1));
}

// Test for checkEqual function in MastodonUtils
TEST(MastodonUtils, checkEqual)
{
  // Inputs for testing
  std::vector<Real> vec1 = {-1, 0, 1};
  std::vector<Real> vec2 = {-1, 0, 1};
  std::vector<Real> vec3 = {4, 5, 6};
  std::vector<Real> vec4 = {2};
  // Outputs for testing
  //   None
  // Value check
  EXPECT_TRUE(MastodonUtils::checkEqual(vec1, vec2, 1));
  EXPECT_FALSE(MastodonUtils::checkEqual(vec1, vec3));
  EXPECT_FALSE(MastodonUtils::checkEqual(vec1, vec4));
}

// Test for isNegativeOrZero function in MastodonUtils
TEST(MastodonUtils, isNegativeOrZero)
{
  // Inputs for testing
  std::vector<Real> vec1 = {-1, 0, 1};
  std::vector<Real> vec2 = {1, 2, 3};
  // Outputs for testing
  //   None
  // Value check
  EXPECT_TRUE(MastodonUtils::isNegativeOrZero(vec1));
  EXPECT_FALSE(MastodonUtils::isNegativeOrZero(vec2));
}

// Test for mean function in MastodonUtils
TEST(MastodonUtils, mean)
{
  // Inputs for testing
  std::vector<Real> vec = {0.5, 0, 1, 1.3, 4};
  // Outputs for testing
  Real mean_of_vec = (0.5 + 0 + 1 + 1.3 + 4) / 5.0;
  // Value check
  EXPECT_EQ(MastodonUtils::mean(vec), mean_of_vec);
}

// Test for median function in MastodonUtils
TEST(MastodonUtils, median)
{
  // Inputs for testing
  std::vector<Real> odd_vec = {1, 0, 1.1, 1.3, 4};
  std::vector<Real> even_vec = {4, 0, 1, -1};
  // Outputs for testing
  //  None
  // Value check
  EXPECT_EQ(MastodonUtils::median(odd_vec), 1.1);
  EXPECT_EQ(MastodonUtils::median(even_vec, "linear"), 0.5);
  EXPECT_EQ(MastodonUtils::median(even_vec, "lower"), 0);
  EXPECT_EQ(MastodonUtils::median(even_vec, "higher"), 1);
}

// Test for percentile function in MastodonUtils
TEST(MastodonUtils, percentile)
{
  // Inputs for testing
  std::vector<Real> vec = {1.2, 0.1, 1.1, 1.3, 2.3, 8.9, 10.5, -1.4, 0.1, -0.3};
  // sorted vector is {-1.4, -0.3, 0.1, 0.1, 1.1, 1.2, 1.3, 2.3, 8.9, 10.5}
  // Outputs for testing
  //  None
  // Value check
  EXPECT_EQ(MastodonUtils::percentile(vec, 0), -1.4);
  EXPECT_EQ(MastodonUtils::percentile(vec, 100), 10.5);
  EXPECT_EQ(MastodonUtils::percentile(vec, 84, "lower"), 2.3);
  EXPECT_EQ(MastodonUtils::percentile(vec, 84, "higher"), 8.9);
  EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(MastodonUtils::percentile(vec, 84, "linear"), 4.94));
  EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(MastodonUtils::percentile(vec, 84), 4.94));
}

// Test for sigma function in MastodonUtils
TEST(MastodonUtils, standardDeviation)
{
  // Inputs for testing
  std::vector<Real> vec = {1.2, 0.1, 1.1, 1.3, 2.3, 8.9, 10.5, -1.4, 0.1, -0.3};
  // sorted vector is {-1.4, -0.3, 0.1, 0.1, 1.1, 1.2, 1.3, 2.3, 8.9, 10.5}
  // Outputs for testing
  //  None
  // Value check
  Real vec_sigma = MastodonUtils::standardDeviation(vec);
  EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(vec_sigma, 4.00716, vec_sigma / 1000));
}

// Test for sigma function in MastodonUtils
TEST(MastodonUtils, lognormalStandardDeviation)
{
  // Inputs for testing
  std::vector<Real> vec = {1.2, 0.1, 1.1, 1.3, 2.3, 8.9, 10.5, 1.52, 0.1, 0.39};
  // sorted vector is {0.1, 0.1, 0.39, 1.1, 1.2, 1.3, 1.52, 2.3, 8.9, 10.5}
  // sorted logvector is {-1}
  // Outputs for testing
  //  None
  // Value check
  Real vec_beta = MastodonUtils::lognormalStandardDeviation(vec);
  EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(vec_beta, 1.58875, vec_beta / 10000));
}

// Test for zeropad function in MastodonUtils
TEST(MastodonUtils, zeropad)
{
  // Inputs for testing
  int n1 = 4;
  int n2 = 15;
  int n_tot = 1256;
  // Outputs for testing
  std::string zpad_n1 = MastodonUtils::zeropad(n1, n_tot);
  std::string zpad_n2 = MastodonUtils::zeropad(n2, n_tot);
  // Value check
  EXPECT_EQ(zpad_n1.compare("0004"), 0);
  EXPECT_EQ(zpad_n2.compare("0015"), 0);
}

// Test for glob function in MastodonUtils
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

// Test for adjust function in MastodonUtils
TEST(MastodonUtils, adjust)
{
  std::vector<Real> x = {1, 2, 3, 4};
  std::vector<Real> y = MastodonUtils::adjust(x, 2, 9);
  ASSERT_EQ(x.size(), y.size());
  for (std::size_t i = 0; i < x.size(); ++i)
    EXPECT_EQ(2 * x[i] + 9, y[i]);
}

// Test for log10 function in MastodonUtils
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

// Unit tests for functions that require external BOOST
#ifdef LIBMESH_HAVE_EXTERNAL_BOOST
// Test for greaterProbability function in MastodonUtils
TEST(MastodonUtils, greaterProbability)
{
  // Inputs for testing
  boost::math::lognormal_distribution<> demand_distribution(log(0.71), 0.39);
  boost::math::lognormal_distribution<> capacity_distribution(log(0.71), 0.39);
  // Outputs for testing
  //  None
  // Value check
  Real test_prob = MastodonUtils::greaterProbability(demand_distribution, capacity_distribution);
  EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(test_prob, 0.5, test_prob / 100));
}

// Test for calcLogLikelihood function in MastodonUtils
TEST(MastodonUtils, calcLogLikelihood)
{
  // Inputs for testing
  std::vector<Real> im = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7};
  std::vector<Real> pf = {0.0001, 0.001, 0.13, 0.45, 0.85, 0.97, 0.98};
  Real loc = 0.4;
  Real sca = 0.1;
  unsigned int n = 1000;
  // Outputs for testing
  Real loglikelihood = MastodonUtils::calcLogLikelihood(im, pf, loc, sca, n);
  // Value check
  EXPECT_TRUE(
      MooseUtils::absoluteFuzzyEqual(loglikelihood, -502.1889, std::abs(loglikelihood / 1000)));
}

// Test for maximizeLogLikelihood function in MastodonUtils
TEST(MastodonUtils, maximizeLogLikelihood)
{
  // Inputs for testing
  std::vector<Real> im = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7};
  std::vector<Real> pf = {0.0001, 0.001, 0.13, 0.45, 0.85, 0.97, 0.98};
  std::vector<Real> loc_space = {0.2, 0.6};
  std::vector<Real> sca_space = {0.1, 0.4};
  unsigned int n = 1000;
  // Outputs for testing
  std::vector<Real> max_values =
      MastodonUtils::maximizeLogLikelihood(im, pf, loc_space, sca_space, n);
  // Value check
  EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(max_values[0], 0.4));
  EXPECT_TRUE(MooseUtils::absoluteFuzzyEqual(max_values[1], 0.24));
}
#endif // LIBMESH_HAVE_EXTERNAL_BOOST
