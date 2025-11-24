// MOOSE includes
#include "DelimitedFileReader.h"

// MASTODON includes
#include "GroundMotionReader.h"
#include "MastodonUtils.h"

registerMooseObject("MastodonApp", GroundMotionReader);

InputParameters
GroundMotionReader::validParams()
{
  InputParameters params = GeneralUserObject::validParams();
  params.addClassDescription("Reads ground motion data from files.");
  params.set<ExecFlagEnum>("execute_on") = EXEC_NONE;
  params.suppressParameter<ExecFlagEnum>("execute_on");
  params.addRequiredParam<std::string>(
      "pattern", "The filename pattern (glob) for the ground motions to read.");
  return params;
}

GroundMotionReader::GroundMotionReader(const InputParameters & parameters)
  : GeneralUserObject(parameters), _pattern(getParam<std::string>("pattern"))
{
  execute();
}

void
GroundMotionReader::execute()
{
  _readers = execute(name(), _pattern);
}

const std::vector<double> &
GroundMotionReader::getData(const std::size_t & index, Component comp) const
{
  return getData(name(), _readers, index, comp);
}

GroundMotionReader::Data
GroundMotionReader::getData(const Real & scale, const Real & offset) const
{
  return getData(name(), _readers, scale, offset);
}

std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>>
GroundMotionReader::execute(const std::string & name, const std::string & pattern)
{
  std::vector<std::string> names = MastodonUtils::glob(pattern);
  if (names.empty())
    ::mooseError("Unable to locate files with the given pattern (",
                 pattern,
                 ") in the GroundMotionReader object '",
                 name,
                 "'.");

  std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> readers;
  readers.reserve(names.size());
  for (const std::string & filename : names)
  {
    std::unique_ptr<MooseUtils::DelimitedFileReader> reader =
        std::make_unique<MooseUtils::DelimitedFileReader>(filename);
    reader->read();
    readers.emplace_back(std::move(reader));
  }
  return readers;
}

unsigned int
GroundMotionReader::count() const
{
  return _readers.size();
}

const std::vector<double> &
GroundMotionReader::getData(
    const std::string & name,
    const std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> & readers,
    const std::size_t & index,
    Component comp)
{
  if (readers.empty())
    ::mooseError("The GroundMotionReader object '",
                 name,
                 "' must execute prior to accessing data, check the "
                 "'execute_on' settings for this object.");

  if (index >= readers.size())
    ::mooseError("The GroundMotionReader object '",
                 name,
                 "' contains ",
                 readers.size(),
                 " ground motions, but an index of ",
                 index,
                 " was requested.");

  return readers[index]->getData()[static_cast<std::size_t>(comp)];
}

GroundMotionReader::Data
GroundMotionReader::getData(
    const std::string & name,
    const std::vector<std::unique_ptr<MooseUtils::DelimitedFileReader>> & readers,
    const Real & scale,
    const Real & offset)
{
  if (readers.empty())
    ::mooseError("The GroundMotionReader object '",
                 name,
                 "' must execute prior to accessing data, check the "
                 "'execute_on' settings for this object.");

  Data output(readers.size());
  for (std::size_t i = 0; i < readers.size(); ++i)
  {
    output[i][Component::TIME] = getData(name, readers, i, Component::TIME);
    output[i][Component::X] =
        MastodonUtils::adjust(getData(name, readers, i, Component::X), scale, offset);
    output[i][Component::Y] =
        MastodonUtils::adjust(getData(name, readers, i, Component::Y), scale, offset);
    output[i][Component::Z] =
        MastodonUtils::adjust(getData(name, readers, i, Component::Z), scale, offset);
  }
  return output;
}
