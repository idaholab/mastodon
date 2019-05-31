#ifndef BEAMMESHGENERATOR_H
#define BEAMMESHGENERATOR_H

#include "MeshGenerator.h"

// Forward declarations
class BeamMeshGenerator;

template <>
InputParameters validParams<BeamMeshGenerator>();

/**
 * Generates a mesh of 1D elements based on node and element connectivity information provided in a
 * file
 */
class BeamMeshGenerator : public MeshGenerator
{
public:
  BeamMeshGenerator(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate();

  /**
   * Populate supplied vector with content from line.
   * @param line The line to extract data from.
   * @param row The vector to populate.
   * @param num The current line number.
   */
  void processLine(const std::string & line, std::vector<double> & row, const unsigned int & num);

  /**
   * Determine the delimiter.
   *
   * If the setDelimiter method is not called the data is inspected, if a ',' is found it is assumed
   * to be the delimiter as is the case for \t. Otherwise a space is used.
   */
  const std::string & delimiter(const std::string & line);

protected:
  /// The supplied filename.
  const std::string _mesh_file;

  /// The delimiter separating the supplied data entires.
  std::string _delimiter;
};

#endif // BEAMMESHGENERATOR_H
