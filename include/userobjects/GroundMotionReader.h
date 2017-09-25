#ifndef GROUNDMOTIONREADER_H
#define GROUNDMOTIONREADER_H

// MOOSE includes
#include "gtest/gtest.h"
#include "GeneralUserObject.h"
#include "DelimitedFileReader.h"

class GroundMotionReader;

template <>
InputParameters validParams<GroundMotionReader>();

/**
 * Reads and provides access to ground motion data.
 */
class GroundMotionReader : public GeneralUserObject
{
public:
  /// Flag for indicating the data column from input ground motionss
  enum class Component
  {
    TIME = 0,
    X = 1,
    Y = 2,
    Z = 3
  };

  /// Alias for underlying storage structure of ground motion data
  using Data = std::vector<std::map<Component, std::vector<Real>>>;

  GroundMotionReader(const InputParameters & parameters);
  virtual void execute() override;
  virtual void initialize() override {}
  virtual void finalize() override {}

  ///@{
  /**
   * Methods for extracting ground motion data.
   */
  const std::vector<double> & getData(const std::size_t & index, Component comp) const;
  Data getData(const Real & scale = 1, const Real & offset = 0) const;
  const std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> & readers() const
  {
    return _readers;
  }
  ///@}

  /**
   * Return the number of ground motions.
   */
  unsigned int count() const;

protected:
  /// Filename pattern (glob)
  const std::string & _pattern;

  /// Storage for the readers, the readers maintain ownership of the data
  std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> _readers;

public:
  /* The following methods are private static methods that implement the primary functions of this
   * object. They are static so that they may be unit tested. They are private because they are
   * not intended to be used outside of this class (excluding testing).
   */
  static std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>>
  execute(const std::string & name, const std::string & pattern);

  static const std::vector<double> &
  getData(const std::string & name,
          const std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> & readers,
          const std::size_t & index,
          Component comp);

  static Data getData(const std::string & name,
                      const std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> & readers,
                      const Real & scale = 1,
                      const Real & offset = 0);
};

#endif
