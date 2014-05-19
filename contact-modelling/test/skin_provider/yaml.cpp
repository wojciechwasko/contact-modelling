#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <vector>
#include <algorithm>
#include <memory>
#include <stdexcept>

#include "yaml-cpp/yaml.h"

#include "cm/grid/grid.hpp"
#include "cm/skin_provider/yaml.hpp"

#include "custom_test_macros.hpp"

struct GridWrap {
  std::unique_ptr<cm::Grid> p;

  cm::Grid::cells_iterator
  begin()
  {
    if (!p)
      throw std::runtime_error("Empty grid in GridWrap!");
    return p->cells_begin();
  }

  cm::Grid::cells_iterator
  end()
  {
    if (!p)
      throw std::runtime_error("Empty grid in GridWrap!");
    return p->cells_end();
  }
};

BOOST_AUTO_TEST_SUITE(skin_provider_yaml)

BOOST_AUTO_TEST_CASE(read_yaml_file)
{
  // first, write the YAML representation
  boost::filesystem::path temp = boost::filesystem::unique_path("test-yaml-%%%%-%%%%-%%%%-%%%%");
  const std::string tempstr    = temp.native();  // optional
  
  YAML::Node rootcell;
  YAML::Node gridcell_1;
  YAML::Node gridcell_2;
  gridcell_1["relative_position"].push_back(1.23);
  gridcell_1["relative_position"].push_back(4.56);
  gridcell_1["value"] = 123;
  gridcell_2["relative_position"].push_back(3.21);
  gridcell_2["relative_position"].push_back(6.54);
  gridcell_2["value"] = 321;
  rootcell["cells"].push_back(gridcell_1);
  rootcell["cells"].push_back(gridcell_2);
  rootcell["attributes"] = YAML::Node();
  rootcell["attributes"]["h"] = 0.002;
  rootcell["attributes"]["E"] = 3e6;
  rootcell["attributes"]["nu"] = 0.5;
  rootcell["attributes"]["taxelRadius"] = 0.002;
  std::ofstream out(tempstr);
  out << rootcell;
  out.close(); // close the file or YAML reader won't be able to open it.

  std::vector<double> expected_cells_coordinates = { 1.23, 4.56, 3.21, 6.54 };
  std::vector<double> expected_update_values = {123, 321};

  std::vector<double> cells_coordinates;
  std::vector<double> update_values;

  cm::SkinProviderYaml provider(tempstr);
  std::unique_ptr<cm::Grid> m ((cm::Grid*) provider.createGrid());
  provider.update(update_values);
  std::vector<double> grid_cells_coordinates;
  std::for_each(m->cells_begin(), m->cells_end(), [&] (cm::Grid::cell_type n) {
    grid_cells_coordinates.push_back(n.x);
    grid_cells_coordinates.push_back(n.y);
  });

  CHECK_CLOSE_COLLECTION(expected_cells_coordinates, grid_cells_coordinates, 1e-4);
  CHECK_CLOSE_COLLECTION(expected_update_values, update_values, 1e-4);
  BOOST_CHECK_EQUAL(provider.getAttributes().h, 0.002);
  BOOST_CHECK_EQUAL(provider.getAttributes().E, 3e6);
  BOOST_CHECK_EQUAL(provider.getAttributes().nu, 0.5);
  BOOST_CHECK_EQUAL(provider.getAttributes().taxelRadius, 0.002);
}

BOOST_AUTO_TEST_SUITE_END()
