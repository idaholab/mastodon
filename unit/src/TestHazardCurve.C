// MOOSE includes
#include "gtest/gtest.h"
#include "DelimitedFileReader.h"

// MASTODON includes
#include "HazardCurve.h"

// Test basic functions of GroundMotionReader
TEST(HazardCurve, execute)
{
  // Data objects
  std::string pattern = "../test/tests/data/ground_motion_*.csv";
  std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> readers;
  readers = GroundMotionReader::execute("foo", pattern);
  std::vector<Real> ref = {1, 2, 3, 4, 5};
  std::vector<std::pair<Real, Real>> sample_data;

  MooseUtils::DelimitedFileReader hazard("../test/tests/data/hazard.csv");
  hazard.read();

  // execute
  std::vector<GroundMotionReader::Data> data =
      HazardCurve::execute("foo", ref, hazard, readers, sample_data);

  // Reference value testing
  {
    unsigned int bin = 1;
    unsigned int index = 0;                                                // ground_motions_00.csv
    unsigned int item = 0;                                                 // 1st row
    GroundMotionReader::Component comp = GroundMotionReader::Component::X; // x column
    EXPECT_NEAR(data[bin][index][comp][item],
                1.398576600000000050e-04 * (sample_data[bin].first / ref[bin]),
                1e-12);
  }

  {
    unsigned int bin = 3;
    unsigned int index = 5;                                                // ground_motions_05.csv
    unsigned int item = 5;                                                 // 6th row
    GroundMotionReader::Component comp = GroundMotionReader::Component::Y; // y column
    EXPECT_NEAR(data[bin][index][comp][item],
                -1.754871600000000066e-04 * (sample_data[bin].first / ref[bin]),
                1e-12);
  }

  // Linear interpolation testing
  EXPECT_EQ(sample_data.size(), 5);

  // PGA (x)
  EXPECT_DOUBLE_EQ(sample_data[0].first, 0.18527);
  EXPECT_DOUBLE_EQ(sample_data[1].first, 0.43421);
  EXPECT_DOUBLE_EQ(sample_data[2].first, 0.68315);
  EXPECT_DOUBLE_EQ(sample_data[3].first, 0.93209);
  EXPECT_DOUBLE_EQ(sample_data[4].first, 1.18103);

  // MAFE (y)
  EXPECT_NEAR(sample_data[0].second, 1.509933371e-03, 1e-12);
  EXPECT_NEAR(sample_data[1].second, 7.0894306e-05, 1e-12);
  EXPECT_NEAR(sample_data[2].second, 6.648008e-06, 1e-12);
  EXPECT_NEAR(sample_data[3].second, 1.017569e-06, 1e-12);
  EXPECT_NEAR(sample_data[4].second, 2.16475e-07, 1e-12);
}
