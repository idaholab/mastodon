#ifndef HazardCurve_H
#define HazardCurve_H

// MOOSE includes
#include "GeneralUserObject.h"
#include "DelimitedFileReader.h"

// MASTODON includes
#include "GroundMotionReader.h"

class HazardCurve;

template <>
InputParameters validParams<HazardCurve>();

/**
 * Reads hazard curve data and create scaled ground motions.
 */
class HazardCurve : public GeneralUserObject
{
public:
  HazardCurve(const InputParameters & parameters);
  virtual void execute() override;
  virtual void initialize() override {}
  virtual void finalize() override {}

  /**
   * Return ground motion for a given hazard bin, index, and component.
   * @param bin The hazard curve bin index.
   * @param index The ground motion index.
   * @param comp The ground motion component.
   */
  const std::vector<Real> & getData(const std::size_t & bin,
                                    const std::size_t & index,
                                    const GroundMotionReader::Component & comp) const;

  /**
   * Return the sampled hazard curve.
   */
  const std::vector<std::pair<Real, Real>> & getSamples() const;

  /**
   * Return the number of curves.
   */
  unsigned int count() const;

  /**
   * Return the number of curves within the provided bin.
   */
  std::size_t count(const std::size_t & bin) const;

  /**
   * Return the number of bins.
   */
  std::size_t bins() const;

protected:
  /// Reader for the hazard data
  MooseUtils::DelimitedFileReader _hazard_reader;

  /// Reader for the ground motions (UserObject)
  const GroundMotionReader & _ground_motion_reader;

  /// Storage for the scaled ground motions
  std::vector<GroundMotionReader::Data> _ground_motion_data;

  /// Hazard sample data
  std::vector<std::pair<Real, Real>> _hazard_sample;

  /// Reference Acceleration
  std::vector<Real> _reference;

  /**
   * Method for checking initialization status.
   */
  void check() const;

public:
  /* The following methods are private static methods that implement the primary functions of this
   * object. They are static so that they may be unit tested. They are private because they are
   * not intended to be used outside of this class (excluding testing).
   */
  static std::vector<GroundMotionReader::Data> execute(
      const std::string & name,
      const std::vector<Real> & reference,
      const MooseUtils::DelimitedFileReader & hazard_reader,
      const std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> & ground_motion_readers,
      std::vector<std::pair<Real, Real>> & sample_data);
};

#endif
