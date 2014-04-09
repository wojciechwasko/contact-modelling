#define BOOST_TEST_MODULE skin_yaml_providere
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <vector>
#include <algorithm>

#include "yaml-cpp/yaml.h"

#include "SkinYamlProvider.hpp"

#include "inc/custom_test_macros.hpp"


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
  std::ofstream out(tempstr);
  out << rootnode;
  out.close(); // close the file or YAML reader won't be able to open it.

  std::vector<double> expected_nodes_coordinates = { 1.23, 4.56, 3.21, 6.54 };
  std::vector<double> expected_update_values = {123, 321};

  std::vector<double> nodes_coordinates;
  std::vector<double> update_values;

  // dummy converter which does nothing
  auto converter = [] (uint16_t v) -> double { return (double)v; };
  SkinYamlProvider<> provider(converter, tempstr);
  for (auto it = provider.sensors_begin(); it != provider.sensors_end(); ++it) {
    nodes_coordinates.push_back((*it).relative_position[0]);
    nodes_coordinates.push_back((*it).relative_position[1]);
  }
  provider.update(update_values);

  CHECK_CLOSE_COLLECTION(expected_nodes_coordinates, nodes_coordinates, 1e-4);
  CHECK_CLOSE_COLLECTION(expected_update_values, update_values, 1e-4);
}
