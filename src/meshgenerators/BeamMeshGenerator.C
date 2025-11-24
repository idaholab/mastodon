#include "BeamMeshGenerator.h"
#include "CastUniquePointer.h"
#include "MooseUtils.h"

#include <fstream>

#include "libmesh/replicated_mesh.h"
#include "libmesh/edge_edge2.h"

registerMooseObject("MooseApp", BeamMeshGenerator);

InputParameters
BeamMeshGenerator::validParams()
{
  InputParameters params = MeshGenerator::validParams();

  params.addRequiredParam<FileName>(
      "mesh_file",
      "Mesh file containing node data and element connectivity. Node data should start with header "
      "`# Nodes` and should contain node id, x, y, z coordinates of the nodes and nodeset ids. "
      "Each row corresponds to a different node. Element connectivity starts with header `# "
      "Element Linear` and should contain element id, nodes and block id. Each row corresponds to "
      "a different element. Currently only linear 1d elements can be generated.");

  return params;
}

BeamMeshGenerator::BeamMeshGenerator(const InputParameters & parameters)
  : MeshGenerator(parameters), _mesh_file(getParam<FileName>("mesh_file"))
{
}

std::unique_ptr<MeshBase>
BeamMeshGenerator::generate()
{
  std::ifstream stream_in(_mesh_file.c_str());

  if (!stream_in)
    paramError("mesh_file", "Can't open beam mesh file: ", _mesh_file);

  // Find lines starting with "#". This should refer to either Nodes or Element Linear
  std::string line;
  std::vector<std::string> file_lines;
  unsigned int node_start_linenum = std::numeric_limits<unsigned int>::max();
  unsigned int element_start_linenum = std::numeric_limits<unsigned int>::max();
  unsigned int node_end_linenum = 0;
  unsigned int element_end_linenum = 0;
  unsigned int linenum = 0;

  while (std::getline(stream_in, line))
  {
    linenum++;

    if (line.find("#") == 0 && line.find("Nodes") != std::string::npos)
      node_start_linenum = linenum;

    if (line.find("#") == 0 && line.find("Element Linear") != std::string::npos)
      element_start_linenum = linenum;

    if (line.find("#") == 0 && line.find("Nodes") == std::string::npos &&
        line.find("Element Linear") == std::string::npos)
      mooseError("BeamMeshGenerator: Found a section in the mesh_file that starts with options "
                 "other than 'Nodes' or 'Element Linear'. Only sections starting with Nodes or "
                 "Element Linear are "
                 "currently read by BeamMeshGenerator.");

    // Store data from file into file_lines
    file_lines.push_back(line);
  }

  if ((node_start_linenum == std::numeric_limits<unsigned int>::max()) ||
      (element_start_linenum == std::numeric_limits<unsigned int>::max()) ||
      (node_start_linenum == element_start_linenum))
    mooseError("BeamMeshGenerator: Mesh file should contain separate sections with node and "
               "element information.");

  std::unique_ptr<ReplicatedMesh> mesh = std::make_unique<ReplicatedMesh>(comm(), 2);

  mesh->set_mesh_dimension(1);
  mesh->set_spatial_dimension(3);
  BoundaryInfo & boundary_info = mesh->get_boundary_info();

  if (element_start_linenum > node_start_linenum)
  {
    // first nodes followed by elements
    node_end_linenum = element_start_linenum - 1;
    element_end_linenum = linenum;
  }
  else
  {
    // first elements followed by nodes
    node_end_linenum = linenum;
    element_end_linenum = node_end_linenum - 1;
  }

  // Get node information
  std::vector<Node *> nodes;
  std::vector<double> row;
  unsigned int node_id = 0;
  for (unsigned int i = node_start_linenum; i < node_end_linenum; ++i)
  {
    // Ignore empty lines
    if (file_lines[i].empty())
      continue;

    row.clear();
    processLine(file_lines[i], row, i + 1);

    if (row.size() < 3)
      mooseError("BeamMeshGenerator: Each row within the `Nodes` section should atleast contain 4 "
                 "entries corresponding to node_id, x, y and z coordinate positions. It can "
                 "additionally contain node sets that the node belongs to.");

    nodes.push_back(mesh->add_point(Point(row[1], row[2], row[3]), node_id));
    if (node_id != row[0])
      mooseError("BeamMeshGenerator: node_id should start at 0 and increase monotonically.");

    // add node set information
    if (row.size() > 4)
    {
      for (unsigned int i = 4; i < row.size(); ++i)
        boundary_info.add_node(node_id, int(row[i]));
    }
    node_id++;
  }

  // get element information
  for (unsigned int i = element_start_linenum; i < element_end_linenum; ++i)
  {
    // Ignore empty lines
    if (file_lines[i].empty())
      continue;

    row.clear();
    processLine(file_lines[i], row, i + 1);

    if (row.size() != 4)
      mooseError("BeamMeshGenerator: Each row within the `Element_Linear` section should contain "
                 "only 4 entries corresponding to element_id, node 1, node 2 and block id.");

    if ((row[1] > nodes.size() - 1) || row[2] > nodes.size() - 1)
      mooseError(
          "BeamMeshGenerator: Node id on ", i + 1, " line should not exceed ", nodes.size(), "\n");

    Elem * elem = mesh->add_elem(new Edge2);
    elem->set_node(0) = nodes[row[1]];
    elem->set_node(1) = nodes[row[2]];
    elem->subdomain_id() = row[3];
  }

  mesh->prepare_for_use(false);

  return dynamic_pointer_cast<MeshBase>(mesh);
}

void
BeamMeshGenerator::processLine(const std::string & line,
                               std::vector<double> & row,
                               const unsigned int & num)
{
  // Separate the row and error if it fails
  bool status = MooseUtils::tokenizeAndConvert<double>(line, row, delimiter(line));
  if (!status)
    mooseError("Failed to convert a delimited data into double when reading line ",
               num,
               " in file ",
               _mesh_file,
               ".\n  LINE ",
               num,
               ": ",
               line);
}

const std::string &
BeamMeshGenerator::delimiter(const std::string & line)
{
  if (_delimiter.empty())
  {
    if (line.find(",") != std::string::npos)
      _delimiter = ",";
    else if (line.find("\t") != std::string::npos)
      _delimiter = "\t";
    else
      _delimiter = " ";
  }
  return _delimiter;
}
