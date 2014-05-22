#include "cm/skin_provider/yaml.hpp"

#include <stdexcept>
#include <algorithm>

#include "yaml-cpp/yaml.h"

#include "cm/grid/grid.hpp"
#include "cm/grid/cell_shapes.hpp"

/**
 * \cond DEV
 */
struct HelperGridCell_and_Value {
  cm::details::HelperGridCell cell;
  double value;
};

namespace YAML {
template<>
struct convert<HelperGridCell_and_Value> {
  static Node encode(const HelperGridCell_and_Value& rhs) {
    Node node;
    node["relative_position"].push_back(rhs.cell.relative_position[0]);
    node["relative_position"].push_back(rhs.cell.relative_position[1]);
    node["value"] = rhs.value;
    return node;
  }

  static bool decode(const Node& node, HelperGridCell_and_Value& rhs) {
    if (!node["relative_position"] || !node["relative_position"].IsSequence() ||
      node["relative_position"].size() != 2 || !node["value"]) {
      return false;
    }

    rhs.cell.relative_position[0] = node["relative_position"][0].as<double>();
    rhs.cell.relative_position[1] = node["relative_position"][1].as<double>();
    rhs.value = node["value"].as<double>();
    return true;
  }
};

template <>
struct convert<cm::SkinAttributes> {
  static Node encode(const cm::SkinAttributes& rhs) {
    Node node;
    node["h"] = rhs.h;
    node["E"] = rhs.E;
    node["nu"] = rhs.nu;
    node["taxelRadius"] = rhs.taxelRadius;
    return node;
  }

  static bool decode(const Node& node, cm::SkinAttributes& rhs) {
    if (!node["h"] || !node["E"] || !node["nu"] || !node["taxelRadius"]) {
      return false;
    }
    rhs.h = node["h"].as<double>();
    rhs.E = node["E"].as<double>();
    rhs.nu = node["nu"].as<double>();
    rhs.taxelRadius = node["taxelRadius"].as<double>();
    return true;
  }
};

}

/**
 * \endcond
 */

namespace cm {

using details::HelperGridCell;

SkinProviderYaml::SkinProviderYaml(
  const std::string& filename
)
  : SkinProviderInterface(1) // hard-coded to 1D sensors only
{
  YAML::Node input = YAML::LoadFile(filename);
  if (!input["cells"]) {
    throw std::runtime_error("SkinProviderYaml: YAML must contain 'cells'!");
  }
  if (!input["cells"].IsSequence()) {
    throw std::runtime_error("SkinProviderYaml: cells must be a sequence!");
  }
  if (!input["attributes"]) {
    throw std::runtime_error("SkinProviderYaml: YAML must contain 'attributes'!");
  }
  skin_attributes_ = input["attributes"].as<SkinAttributes>();

  YAML::Node cells = input["cells"];
  typedef YAML::const_iterator yci;
  HelperGridCell_and_Value temp_cell_val;
  for (yci it = cells.begin(); it != cells.end(); ++it) {
    temp_cell_val = it->as<HelperGridCell_and_Value>();

    cells_.push_back(temp_cell_val.cell);
    values_.push_back(temp_cell_val.value);
  }
}

void
SkinProviderYaml::impl_update(SkinProviderYaml::target_values_type& target_vec) const
{
  target_vec.assign(values_.cbegin(), values_.cend());
}

Grid*
SkinProviderYaml::impl_createGrid() const
{
  return Grid::fromSensors(D, Circle(skin_attributes_.taxelRadius), cells_.cbegin(), cells_.cend()); 
}

SkinAttributes
SkinProviderYaml::impl_getAttributes() const
{
  return skin_attributes_;
}

} /* namespace cm */
