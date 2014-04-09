#ifndef SKINYAMLPROVIDER_HPP
#define SKINYAMLPROVIDER_HPP

#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>

#include "yaml-cpp/yaml.h"

#include "SkinProviderInterface.hpp"

template <class raw, class conv>
class SkinYamlProvider;

template <class raw, class conv>
struct SkinProvider_traits<SkinYamlProvider<raw, conv>>
{
  struct HelperMeshNode {
    std::array<double, 2> relative_position;
  };
  typedef typename std::vector<HelperMeshNode>::iterator sensor_iterator;
  typedef raw rawT; 
  typedef conv convT;
};

/**
 * \brief   Get skin sensors and response from a YAML file.
 *
 * Format:
 * nodes:
 * - relative_position:
 *     - 1.23
 *     - 4.56
 *   value: 123
 * - relative_position:
 *     - 3.21
 *     - 6.54
 *   value: 321
 *
 * TODO Open the file ourselves; YAML does not error-out on a un-readable file.
 */
template <class raw = uint16_t, class conv = double>
class SkinYamlProvider : public SkinProviderInterface<SkinYamlProvider<raw, conv>>
{
friend class SkinProviderInterface<SkinYamlProvider<raw,conv>>;
  public:

    using typename SkinProviderInterface<SkinYamlProvider<raw, conv>>::sensor_iterator;
    using typename SkinProviderInterface<SkinYamlProvider<raw, conv>>::convT;
    using typename SkinProviderInterface<SkinYamlProvider<raw, conv>>::rawT;
    using typename SkinProviderInterface<SkinYamlProvider<raw, conv>>::converter_type;

    SkinYamlProvider(converter_type converter, const std::string& filename)
      : SkinProviderInterface<SkinYamlProvider<raw,conv>>(converter)
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
        raw_values_.push_back(node["value"].as<rawT>());
      }
    }

  protected:
    typedef typename SkinProvider_traits<SkinYamlProvider<raw,conv>>::HelperMeshNode HelperMeshNode;
    std::vector<HelperMeshNode> nodes_;
    std::vector<rawT> raw_values_;

    sensor_iterator impl_sensors_begin() { return nodes_.begin(); }
    sensor_iterator impl_sensors_end()   { return nodes_.end();   }

    void impl_update(std::vector<convT>& target_vec)
    {
      typedef rawT raw_type;
      target_vec.clear();
      std::for_each(raw_values_.cbegin(), raw_values_.cend(), [&] (raw_type val) {
        target_vec.push_back(this->converter_(val));
      });
    }

    std::vector<convT> impl_update()
    {
      std::vector<convT> ret(raw_values_.size());
      this->update(ret);
      return ret;
    }

};

#endif /* SKINYAMLPROVIDER_HPP */
