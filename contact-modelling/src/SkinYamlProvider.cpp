#include "SkinYamlProvider.hpp"

#include <stdexcept>
#include <algorithm>

#include "yaml-cpp/yaml.h"

#include "MeshNatural.hpp"

SkinYamlProvider::SkinYamlProvider(
  SkinYamlProvider::converter_type converter,
  const std::string& filename
)
  : SkinProviderInterface(1, converter) // hard-coded to 1D sensors only
{
  YAML::Node input = YAML::LoadFile(filename);
  if (!input["nodes"]) {
    throw std::runtime_error("SkinYamlProvider: YAML must contain 'nodes'!");
  }
  if (!input["nodes"].IsSequence()) {
    throw std::runtime_error("SkinYamlProvider: nodes must be a sequence!");
  }

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
    raw_values_.push_back(node["value"].as<double>());
  }
}

void
SkinYamlProvider::impl_update(SkinYamlProvider::target_values_type& target_vec)
{
  target_vec.resize(raw_values_.size());
  std::transform(raw_values_.cbegin(), raw_values_.cend(), target_vec.begin(), this->converter_);
}

MeshNatural*
SkinYamlProvider::impl_createMesh()
{
  return new MeshNatural(D, nodes_.cbegin(), nodes_.cend()); 
}
