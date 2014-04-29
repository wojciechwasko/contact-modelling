#define BOOST_TEST_MODULE skin_yaml_providere
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>
#include <stdexcept>

#include "yaml-cpp/yaml.h"

#include "MeshInterface.hpp"
#include "SkinYamlProvider.hpp"

#include "inc/custom_test_macros.hpp"

struct MeshWrap {
  std::unique_ptr<MeshInterface> p;

  MeshInterface::nodes_iterator
  begin()
  {
    if (!p)
      throw std::runtime_error("Empty mesh in MeshWrap!");
    return p->nodes_begin();
  }

  MeshInterface::nodes_iterator
  end()
  {
    if (!p)
      throw std::runtime_error("Empty mesh in MeshWrap!");
    return p->nodes_end();
  }
};

BOOST_AUTO_TEST_CASE(read_yaml_file)
{
  // first, write the YAML representation
  boost::filesystem::path temp = boost::filesystem::unique_path("test-yaml-%%%%-%%%%-%%%%-%%%%");
  const std::string tempstr    = temp.native();  // optional
  
  YAML::Node rootnode;
  YAML::Node meshnode_1;
  YAML::Node meshnode_2;
  meshnode_1["relative_position"].push_back(1.23);
  meshnode_1["relative_position"].push_back(4.56);
  meshnode_1["value"] = 123;
  meshnode_2["relative_position"].push_back(3.21);
  meshnode_2["relative_position"].push_back(6.54);
  meshnode_2["value"] = 321;
  rootnode["nodes"].push_back(meshnode_1);
  rootnode["nodes"].push_back(meshnode_2);
  rootnode["attributes"] = YAML::Node();
  rootnode["attributes"]["h"] = 0.002;
  rootnode["attributes"]["E"] = 3e6;
  rootnode["attributes"]["taxelArea"] = 0.002;
  std::ofstream out(tempstr);
  out << rootnode;
  out.close(); // close the file or YAML reader won't be able to open it.

  std::vector<double> expected_nodes_coordinates = { 1.23, 4.56, 3.21, 6.54 };
  std::vector<double> expected_update_values = {123, 321};

  std::vector<double> nodes_coordinates;
  std::vector<double> update_values;

  SkinYamlProvider provider(tempstr);
  std::unique_ptr<MeshInterface> m ((MeshInterface*) provider.createMesh());
  provider.update(update_values);
  std::vector<double> mesh_nodes_coordinates;
  std::for_each(m->nodes_begin(), m->nodes_end(), [&] (MeshInterface::node_type n) {
    mesh_nodes_coordinates.push_back(n.x);
    mesh_nodes_coordinates.push_back(n.y);
  });

  CHECK_CLOSE_COLLECTION(expected_nodes_coordinates, mesh_nodes_coordinates, 1e-4);
  CHECK_CLOSE_COLLECTION(expected_update_values, update_values, 1e-4);
  BOOST_CHECK_EQUAL(provider.getAttributes().h, 0.002);
  BOOST_CHECK_EQUAL(provider.getAttributes().E, 3e6);
}
