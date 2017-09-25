// MOOSE includes
#include "DelimitedFileReader.h"
#include "LinearInterpolation.h"

// MASTODON includes
#include "HazardCurve.h"
#include "MastodonUtils.h"

template <>
InputParameters
validParams<HazardCurve>()
{
  InputParameters params = validParams<GeneralUserObject>();
  params.set<MultiMooseEnum>("execute_on") = "initial";
  params.addRequiredParam<UserObjectName>("ground_motions",
                                          "The GroundMotionReader object to extract ground motion "
                                          "data from to build the hazard curve.");
  params.addRequiredParam<std::string>("filename", "The hazard curve csv filename.");
  params.addParam<unsigned int>(
      "number_of_bins", 10, "The number of bins to create from the hazard curve data.");
  params.addParam<std::vector<Real>>("reference_acceleration",
                                     std::vector<Real>(1, 1),
                                     "Ground motion reference acceleration(s).");
  return params;
}

HazardCurve::HazardCurve(const InputParameters & parameters)
  : GeneralUserObject(parameters),
    _hazard_reader(getParam<std::string>("filename")),
    _ground_motion_reader(getUserObject<GroundMotionReader>("ground_motions"))
{
  const unsigned int & n_bins = getParam<unsigned int>("number_of_bins");
  const std::vector<Real> & ref = getParam<std::vector<Real>>("reference_acceleration");
  if (ref.size() == 1)
    _reference.resize(n_bins, ref[0]);
  else if (ref.size() == n_bins)
    _reference = ref;
  else
    mooseError("The 'reference_acceleration' input must (size: ",
               ref.size(),
               ") in the '",
               name(),
               "' block should be a scalar or a vector equal to the length of the "
               "prescribed number of bins (",
               n_bins,
               ").");
}

void
HazardCurve::execute()
{
  _hazard_reader.read();

  _ground_motion_data = HazardCurve::execute(
      name(), _reference, _hazard_reader, _ground_motion_reader.readers(), _hazard_sample);
}

const std::vector<Real> &
HazardCurve::getData(const unsigned int & bin,
                     const unsigned int & index,
                     const GroundMotionReader::Component & comp) const
{
  return _ground_motion_data[bin][index].at(comp);
}

const std::vector<std::pair<Real, Real>> &
HazardCurve::getSamples() const
{
  return _hazard_sample;
}

std::vector<GroundMotionReader::Data>
HazardCurve::execute(
    const std::string & name,
    const std::vector<Real> & reference,
    const MooseUtils::DelimitedFileReader & hazard_reader,
    const std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> & ground_motion_readers,
    std::vector<std::pair<Real, Real>> & sample_data)
{
  // Read hazard curve data
  const std::vector<std::vector<double>> & data = hazard_reader.getData();
  mooseAssert(!data.empty(), "The supplied hazard data is empty.");

  // Number of bins
  const unsigned int n_bins = reference.size();

  // Adjust the data for linear interpolation
  std::vector<Real> pga = data[1];                        // x-axis: Peak ground acceleration
  std::vector<Real> mafe = MastodonUtils::log10(data[0]); // y-axis: fannual frequency of exceedence
  if (pga.front() > pga.back())
  {
    std::reverse(pga.begin(), pga.end());
    std::reverse(mafe.begin(), mafe.end());
  }

  // Build linear interpolation object
  LinearInterpolation linear_interp(pga, mafe);

  // Determine bin spacing
  std::vector<double>::const_iterator x_min = std::min_element(pga.begin(), pga.end());
  std::vector<double>::const_iterator x_max = std::max_element(pga.begin(), pga.end());
  double dx = (*x_max - *x_min) / n_bins;

  // Clear sample data
  sample_data.clear();
  sample_data.reserve(n_bins);

  // Populate ground motion data
  std::vector<GroundMotionReader::Data> ground_motion_data(reference.size());
  double x = (*x_min) + dx / 2.;
  for (unsigned int i = 0; i < n_bins; ++i)
  {
    Real scale = x / reference[i];
    ground_motion_data[i] = GroundMotionReader::getData(name, ground_motion_readers, scale);

    Real y = linear_interp.sample(x);
    sample_data.emplace_back(x, std::pow(10, y));

    x += dx;
  }
  return ground_motion_data;
}
