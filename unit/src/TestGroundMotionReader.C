// MOOSE includes
#include "gtest/gtest.h"
#include "DelimitedFileReader.h"

// MASTODON includes
#include "GroundMotionReader.h"

void
EXPECT_MESSAGE(const std::exception & err, const std::string & gold)
{
  std::size_t pos = std::string(err.what()).find(gold);
  EXPECT_TRUE(pos != std::string::npos);
}

// Test basic functions of GroundMotionReader
TEST(GroundMotionReader, error)
{
  // Data objects
  std::string pattern = "../test/tests/data/ground_motion_*.csv";
  std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> readers;

  // Wrong pattern
  try
  {
    GroundMotionReader::execute("foo", "this/is/wrong*.csv");
    FAIL();
  }
  catch (const std::exception & err)
  {
    EXPECT_MESSAGE(err,
                   "Unable to locate files with the given pattern (this/is/wrong*.csv) in the "
                   "GroundMotionReader object 'foo'.");
  }

  // No execute test 1
  try
  {
    GroundMotionReader::getData("foo", readers, 0, GroundMotionReader::Component::TIME);
    FAIL();
  }
  catch (const std::exception & err)
  {
    EXPECT_MESSAGE(err,
                   "The GroundMotionReader object 'foo' must execute prior to accessing data, "
                   "check the 'execute_on' settings for this object.");
  }

  // No execute test 2
  try
  {
    GroundMotionReader::getData("foo", readers);
    FAIL();
  }
  catch (const std::exception & err)
  {
    EXPECT_MESSAGE(err,
                   "The GroundMotionReader object 'foo' must execute prior to accessing data, "
                   "check the 'execute_on' settings for this object.");
  }

  // Bad index
  try
  {
    readers = GroundMotionReader::execute("foo", pattern);
    GroundMotionReader::getData("foo", readers, 12345, GroundMotionReader::Component::X);
    FAIL();
  }
  catch (const std::exception & err)
  {
    EXPECT_MESSAGE(err,
                   "The GroundMotionReader object 'foo' contains 10 ground motions, but an "
                   "index of 12345 was requested.");
  }
}

// Test basic functions of GroundMotionReader
TEST(GroundMotionReader, execute)
{
  // Data objects
  std::string pattern = "../test/tests/data/ground_motion_*.csv";
  std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> readers;
  readers = GroundMotionReader::execute("foo", pattern);

  {
    const std::vector<double> & data =
        GroundMotionReader::getData("foo", readers, 5, GroundMotionReader::Component::TIME);
    EXPECT_EQ(data[1], 5.000000000000000104e-03);
  }
  {
    const std::vector<double> & data =
        GroundMotionReader::getData("foo", readers, 5, GroundMotionReader::Component::X);
    EXPECT_EQ(data[1], -1.491688400000000068e-05);
  }
  {
    const std::vector<double> & data =
        GroundMotionReader::getData("foo", readers, 5, GroundMotionReader::Component::Y);
    EXPECT_EQ(data[1], -1.720667299999999991e-04);
  }
  {
    const std::vector<double> & data =
        GroundMotionReader::getData("foo", readers, 5, GroundMotionReader::Component::Z);
    EXPECT_EQ(data[1], -4.427559199999999816e-05);
  }

  GroundMotionReader::Data data = GroundMotionReader::getData("foo", readers, 2, 10);
  EXPECT_EQ(data[5][GroundMotionReader::Component::TIME][1], 5.000000000000000104e-03);
  EXPECT_EQ(data[5][GroundMotionReader::Component::X][1], 2 * -1.491688400000000068e-05 + 10);
  EXPECT_EQ(data[5][GroundMotionReader::Component::Y][1], 2 * -1.720667299999999991e-04 + 10);
  EXPECT_EQ(data[5][GroundMotionReader::Component::Z][1], 2 * -4.427559199999999816e-05 + 10);
}
