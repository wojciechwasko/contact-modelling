#ifndef SKINYAMLPROVIDER_HPP
#define SKINYAMLPROVIDER_HPP

#include <array>
#include <vector>
#include <cstdint>

#include "SkinProviderInterface.hpp"

/**
 * \brief   Get skin sensors and response from a YAML file.
 *
 * \note  Right now, the SkinYamlProvider is hard-coded to accept only 1-dimensional
 *        sensors (see Format below), but in principle, there's no reason why it couldn't be
 *        generalised to multi-dimensional case.
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
class SkinYamlProvider : public SkinProviderInterface
{
public:
  using typename SkinProviderInterface::converter_type;
  using typename SkinProviderInterface::target_values_type;

  SkinYamlProvider(converter_type converter, const std::string& filename);

private:
  struct HelperMeshNode {
    std::array<double, 2> relative_position;
  };

  std::vector<HelperMeshNode>      nodes_;
  std::vector<uint16_t>         raw_values_;

  void impl_update(target_values_type& target_vec);
  MeshNatural* impl_createMesh();
};

#endif /* SKINYAMLPROVIDER_HPP */
