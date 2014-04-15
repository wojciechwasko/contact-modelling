#include "SkinYamlProvider.hpp"

#include <stdexcept>
#include <algorithm>

#include "yaml-cpp/yaml.h"

#include "MeshNatural.hpp"

SkinYamlProvider::SkinYamlProvider(
  const std::string& filename
)
  : SkinProviderInterface(1) // hard-coded to 1D sensors only
{
  YAML::Node input = YAML::LoadFile(filename);
  if (!input["nodes"]) {
    throw std::runtime_error("SkinYamlProvider: YAML must contain 'nodes'!");
  }
  if (!input["nodes"].IsSequence()) {
    throw std::runtime_error("SkinYamlProvider: nodes must be a sequence!");
  }

  if (!input["attributes"]) {
    throw std::runtime_error("SkinYamlProvider: YAML must contain 'attributes'!");
  }
  // yaml-cpp does not support chaining on [] operator properly
  YAML::Node attrs = input["attributes"];
  if (!attrs["h"]) {
    throw std::runtime_error("SkinYamlProvider: YAML must contain 'attributes:h'!");
  }
  if (!attrs["E"]) {
    throw std::runtime_error("SkinYamlProvider: YAML must contain 'attributes:E'!");
  }

  skin_attributes_.h = attrs["h"].as<double>();
  skin_attributes_.E = attrs["E"].as<double>();

  YAML::Node nodes = input["nodes"];
  typedef YAML::const_iterator yci;
  // FIXME probably should replace this fugly loop with a YAML::convert<> specialisation
  for (yci it = nodes.begin(); it != nodes.end(); ++it) {
    YAML::Node node = *it; 
    if (
      !node["relative_position"] ||
      !node["relative_position"].IsSequence() ||
      node["relative_position"].size() != 2
    ) {
      throw std::runtime_error("SkinYamlProvider: node must have "
        "'relative_position' attribute -- a sequence of length 2");
    }
    if (!node["value"]) {
      throw std::runtime_error("SkinYamlProvider: node must have "
        "a 'value' (raw response) attribute");
    }

    HelperMeshNode n;
    std::get<0>(n.relative_position) = node["relative_position"][0].as<double>();
    std::get<1>(n.relative_position) = node["relative_position"][1].as<double>();
    nodes_.push_back(n);
    values_.push_back(node["value"].as<double>());
  }
}

void
SkinYamlProvider::impl_update(SkinYamlProvider::target_values_type& target_vec) const
{
  target_vec.assign(values_.cbegin(), values_.cend());
}

MeshNatural*
SkinYamlProvider::impl_createMesh() const
{
  return new MeshNatural(D, nodes_.cbegin(), nodes_.cend()); 
}

SkinAttributes
SkinYamlProvider::impl_getAttributes() const
{
  return skin_attributes_;
}
