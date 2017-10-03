// MOOSE includes
#include "gtest/gtest.h"
#include "ISoilUtils.h"
#include "MastodonUtils.h"
#include "MooseUtils.h"

// Test for computeSoilLayerProperties function in ISoilUtils
TEST(ISoilUtils, computeSoilLayerProperties)
{
  // Inputs for testing
  std::vector<Real> bb_stress_0 = {3.5, 6.5, 7};
  std::vector<Real> bb_strain_0 = {1, 3, 4};
  std::vector<Real> bb_stress_1 = {7, 13, 14};
  std::vector<Real> bb_strain_1 = {1, 3, 4};
  std::vector<std::vector<Real>> bb_stress = {bb_stress_0, bb_stress_1};
  std::vector<std::vector<Real>> bb_strain = {bb_strain_0, bb_strain_1};
  std::vector<unsigned int> layer_ids = {0, 1};
  std::vector<std::vector<Real>> youngs;
  std::vector<std::vector<Real>> yield_stress;
  std::vector<Real> poissons_ratio = {0.3, 0.3};
  std::string name = "unit_test";
  // Outputs for testing *** CALCULATES YIELD STRAIN AND NOT YIELD STRESS ***
  std::vector<Real> youngs_0 = {5.2, 2.6, 1.3};
  std::vector<Real> yield_stress_0 = {
      1 * std::sqrt(3) / 2.6, 3 * std::sqrt(3) / 2.6, 4 * std::sqrt(3) / 2.6};
  std::vector<Real> youngs_1 = {10.4, 5.2, 2.6};
  std::vector<Real> yield_stress_1 = {
      1 * std::sqrt(3) / 2.6, 3 * std::sqrt(3) / 2.6, 4 * std::sqrt(3) / 2.6};
  std::vector<std::vector<Real>> youngs_out = {youngs_0, youngs_1};
  std::vector<std::vector<Real>> yield_stress_out = {yield_stress_0, yield_stress_1};
  youngs.resize(layer_ids.size());
  yield_stress.resize(layer_ids.size());
  ISoilUtils::computeSoilLayerProperties(
      youngs, yield_stress, bb_stress, bb_strain, layer_ids, poissons_ratio, name);
  // Size check
  EXPECT_EQ(youngs.size(), layer_ids.size())
      << "Number of vectors in youngs is different from the size of layer_ids.\n";
  EXPECT_EQ(yield_stress.size(), layer_ids.size())
      << "Number of vectors in yield_stress is different from the size of layer_ids.\n";
  // Value check
  for (std::size_t i = 0; i < layer_ids.size(); ++i)
  {
    EXPECT_TRUE(MastodonUtils::checkEqual(youngs[i], youngs_out[i]))
        << "youngs is incorrect for layer " << i << ".\n";
    EXPECT_TRUE(MastodonUtils::checkEqual(yield_stress[i], yield_stress_out[i]))
        << "yield_stress is incorrect for layer " << i << ".\n";
  }
}

// Test for computeUserDefinedBackbone function in ISoilUtils
TEST(ISoilUtils, computeUserDefinedBackbone)
{
  // Inputs for testing
  std::vector<std::vector<Real>> backbone_stress;
  std::vector<std::vector<Real>> backbone_strain;
  std::vector<unsigned int> layer_ids = {0, 1};
  std::vector<FileName> backbone_curve_files = {"bb0.csv", "bb1.csv"};
  std::string name = "unit_test";
  // Outputs for testing
  std::vector<Real> bb_strain_0 = {1.1, 2.2, 3.3, 4.4, 5.5};
  std::vector<Real> bb_stress_0 = {5, 10, 15, 20, 25};
  std::vector<Real> bb_strain_1 = {1, 2, 3, 4, 5};
  std::vector<Real> bb_stress_1 = {1.1, 2.2, 3.3, 4.4, 5.5};
  ISoilUtils::computeUserDefinedBackbone(
      backbone_stress, backbone_strain, layer_ids, backbone_curve_files, name);
  // Value check
  EXPECT_TRUE(MastodonUtils::checkEqual(bb_strain_0, backbone_strain[0]))
      << "backbone_strain is incorrect for layer 0.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(bb_strain_1, backbone_strain[1]))
      << "backbone_strain is incorrect for layer 1.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(bb_stress_0, backbone_stress[0]))
      << "backbone_stress is incorrect for layer 0.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(bb_stress_1, backbone_stress[1]))
      << "backbone_stress is incorrect for layer 1.\n";
}

// Test for computeDarendeliBackbone function in ISoilUtils
TEST(ISoilUtils, computeDarendeliBackbone)
{
  // Inputs for testing
  std::vector<std::vector<Real>> backbone_stress;
  std::vector<std::vector<Real>> backbone_strain;
  std::vector<unsigned int> layer_ids = {0, 1};
  std::vector<Real> initial_shear_modulus = {10000, 20000};
  std::vector<Real> over_consolidation_ratio = {1, 1};
  unsigned int number_of_points = 10;
  std::vector<Real> plasticity_index = {1, 0};
  std::vector<Real> p_ref = {10, 6.07286};
  std::string name = "unit_test";
  ISoilUtils::computeDarendeliBackbone(backbone_stress,
                                       backbone_strain,
                                       layer_ids,
                                       initial_shear_modulus,
                                       over_consolidation_ratio,
                                       plasticity_index,
                                       p_ref,
                                       number_of_points,
                                       name);
  // Outputs for testing
  std::vector<Real> strain_out = {1E-6,
                                  3.593814E-06,
                                  1.291550E-05,
                                  4.641589E-05,
                                  1.668101E-04,
                                  5.994843E-04,
                                  2.15443E-03,
                                  7.742637E-03,
                                  2.782559E-02,
                                  0.1};
  std::vector<Real> stress_out_0 = {9.907442E-03,
                                    3.488226E-02,
                                    1.176193E-01,
                                    3.522291E-01,
                                    8.218794E-01,
                                    1.382561E+00,
                                    1.824395E+00,
                                    2.149412E+00,
                                    2.430726E+00,
                                    2.712482E+00};
  std::vector<Real> stress_out_1 = {1.977760E-02,
                                    6.934950E-02,
                                    2.310354E-01,
                                    6.714766E-01,
                                    1.489817E+00,
                                    2.390503E+00,
                                    3.075415E+00,
                                    3.588236E+00,
                                    4.044779E+00,
                                    4.509025E+00};
  // Value check
  EXPECT_TRUE(MastodonUtils::checkEqual(strain_out, backbone_strain[0], 0.01))
      << "backbone_strain is incorrect for layer 0.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(strain_out, backbone_strain[1], 0.01))
      << "backbone_strain is incorrect for layer 1.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(stress_out_0, backbone_stress[0], 0.01))
      << "backbone_stress is incorrect for layer 0.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(stress_out_1, backbone_stress[1], 0.01))
      << "backbone_stress is incorrect for layer 1.\n";
}

// Test for computeGQHBackbone function in ISoilUtils
TEST(ISoilUtils, computeGQHBackBone)
{
  // Inputs for testing
  std::vector<std::vector<Real>> backbone_stress;
  std::vector<std::vector<Real>> backbone_strain;
  std::vector<unsigned int> layer_ids = {0, 1};
  std::vector<Real> initial_shear_modulus = {20000000, 40000000};
  unsigned int number_of_points = 10;
  std::vector<Real> theta_1 = {-2.28, -2.28};
  std::vector<Real> theta_2 = {-5.54, -5.54};
  std::vector<Real> theta_3 = {1, 1};
  std::vector<Real> theta_4 = {1, 1};
  std::vector<Real> theta_5 = {0.99, 0.99};
  std::vector<Real> taumax = {7500, 15000};
  ISoilUtils::computeGQHBackbone(backbone_stress,
                                 backbone_strain,
                                 layer_ids,
                                 initial_shear_modulus,
                                 number_of_points,
                                 theta_1,
                                 theta_2,
                                 theta_3,
                                 theta_4,
                                 theta_5,
                                 taumax);
  // Outputs for testing
  std::vector<Real> strain_out = {1E-6,
                                  3.593814E-06,
                                  1.291550E-05,
                                  4.641589E-05,
                                  1.668101E-04,
                                  5.994843E-04,
                                  2.15443E-03,
                                  7.742637E-03,
                                  2.782559E-02,
                                  0.1};
  std::vector<Real> stress_out_0 = {19.826806,
                                    69.695988,
                                    232.496470,
                                    671.044652,
                                    1490.255320,
                                    2617.086221,
                                    4084.325314,
                                    5661.610951,
                                    6770.879667,
                                    7266.8512};
  std::vector<Real> stress_out_1 = {39.653612,
                                    139.391976,
                                    464.992940,
                                    1342.089304,
                                    2980.510640,
                                    5234.172442,
                                    8168.650627,
                                    11323.221903,
                                    13541.759334,
                                    14533.7024};
  // // Value check
  EXPECT_TRUE(MastodonUtils::checkEqual(strain_out, backbone_strain[0], 0.01))
      << "backbone_strain is incorrect for layer 0.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(strain_out, backbone_strain[1], 0.01))
      << "backbone_strain is incorrect for layer 1.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(stress_out_0, backbone_stress[0], 0.01))
      << "backbone_stress is incorrect for layer 0.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(stress_out_1, backbone_stress[1], 0.01))
      << "backbone_stress is incorrect for layer 1.\n";
}

// Test for computeCoulombBackbone function in ISoilUtils
TEST(ISoilUtils, computeCoulombBackbone)
{
  // Inputs for testing
  std::vector<std::vector<Real>> backbone_stress;
  std::vector<std::vector<Real>> backbone_strain;
  std::vector<unsigned int> layer_ids = {0, 1};
  std::vector<Real> initial_shear_modulus = {20000, 40000};
  std::vector<Real> friction_coefficient = {0.3, 0.6};
  std::vector<Real> hardening_ratio = {0.01, 0.01};
  std::vector<Real> p_ref = {100, 200};
  std::string name = "unit_test";
  ISoilUtils::computeCoulombBackbone(backbone_stress,
                                     backbone_strain,
                                     layer_ids,
                                     initial_shear_modulus,
                                     friction_coefficient,
                                     hardening_ratio,
                                     p_ref,
                                     name);

  // Outputs for testing
  std::vector<Real> strain_out_0 = {0.0015, 100};
  std::vector<Real> strain_out_1 = {0.003, 100};
  std::vector<Real> stress_out_0 = {30, 20029.7};
  std::vector<Real> stress_out_1 = {120, 40118.8};
  // // Value check
  EXPECT_TRUE(MastodonUtils::checkEqual(strain_out_0, backbone_strain[0], 0.01))
      << "backbone_strain is incorrect for layer 0.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(strain_out_1, backbone_strain[1], 0.01))
      << "backbone_strain is incorrect for layer 1.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(stress_out_0, backbone_stress[0], 0.01))
      << "backbone_stress is incorrect for layer 0.\n";
  EXPECT_TRUE(MastodonUtils::checkEqual(stress_out_1, backbone_stress[1], 0.01))
      << "backbone_stress is incorrect for layer 1.\n";
}
