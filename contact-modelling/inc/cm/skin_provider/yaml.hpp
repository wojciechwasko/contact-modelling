#ifndef SKINPROVIDERYAML_HPP
#define SKINPROVIDERYAML_HPP

#include <array>
#include <vector>
#include <cstdint>

#include "cm/skin_provider/interface.hpp"

namespace cm {

/**
 * \brief   Get skin sensors and response from a YAML file.
 *
 * \note  Right now, the SkinProviderYaml is hard-coded to accept only 1-dimensional
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
class SkinProviderYaml : public cm::SkinProviderInterface
{
public:
  using typename cm::SkinProviderInterface::target_values_type;

  SkinProviderYaml(const std::string& filename);

private:
  struct HelperMeshNode {
    std::array<double, 2> relative_position;
  };

  std::vector<HelperMeshNode>             nodes_;
  std::vector<double>                    values_;
  SkinAttributes                skin_attributes_;

  void impl_update(target_values_type& target_vec) const;
  MeshNatural* impl_createMesh() const;
  SkinAttributes impl_getAttributes() const;
};

} /* namespace cm */

#endif /* SKINPROVIDERYAML_HPP */
