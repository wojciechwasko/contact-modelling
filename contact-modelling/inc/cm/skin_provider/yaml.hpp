#ifndef SKINPROVIDERYAML_HPP
#define SKINPROVIDERYAML_HPP

#include <array>
#include <vector>
#include <cstdint>

#include "cm/skin_provider/interface.hpp"

namespace cm {

/**
 * \cond DEV
 */
namespace details {
/**
 * \brief   A helper struct to allow later creation of Grid through
 * Grid::fromSensors()
 */
struct HelperGridCell {
  std::array<double, 2> relative_position;
};
}
/**
 * \endcond
 */

/**
 * \brief   Get skin sensors and response from a YAML file.
 *
 * \note  Right now, the SkinProviderYaml is hard-coded to accept only
 * 1-dimensional sensors (see Format below), but in principle, there's no reason
 * why it couldn't be generalised to multi-dimensional case.
 * \par
 * \note  Sensor response values are taken as-is, no conversion is applied.
 * Moreover, subsequent calls to update() will yield the same values, read from
 * the YAML file
 *
 * Format:
 *
 *     cells:
 *     - relative_position:
 *         - 1.23
 *         - 4.56
 *       value: 123
 *     - relative_position:
 *         - 3.21
 *         - 6.54
 *       value: 321
 *     attributes:
 *       h: 0.002
 *       E: 210000
 *       nu: 0.5
 *       taxelRadius: 0.002
 *
 * \todo Open the file ourselves; YAML does not error-out on a un-readable file.
 */
class SkinProviderYaml : public cm::SkinProviderInterface
{
public:
  using typename cm::SkinProviderInterface::target_values_type;

  SkinProviderYaml(const std::string& filename);

private:
  /**
   * \brief   Grid's cells' locations, as read from the YAML file
   */
  std::vector<details::HelperGridCell>             cells_;
  /**
   * \brief   Values, as read from the YAML file
   */
  std::vector<double>                    values_;
  /**
   * \brief   Attributes of the skin, as read from the YAML file
   */
  SkinAttributes                skin_attributes_;

  void impl_update(target_values_type& target_vec) const;
  Grid* impl_createGrid() const;
  SkinAttributes impl_getAttributes() const;
};

} /* namespace cm */

#endif /* SKINPROVIDERYAML_HPP */
