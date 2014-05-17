#ifndef MESHNATURAL_HPP
#define MESHNATURAL_HPP

#include <cstdint>
#include <algorithm>
#include <limits>

#include "cm/mesh/interface.hpp"
#include "cm/skin/attributes.hpp"
#include "cm/details/skin.hpp"

namespace cm {

/**
 * \brief   Mesh created from Skin's sensors.
 * \tparam  dim   dimensionality of the values stored in the node.
 * 
 * \note  All the algorithms in this library consider that if D == 1, we consider forces in the "z"
 *        direction.
 *
 * Each node corresponds to one sensor in the skin, with (x,y)
 * values copied over.
 *
 * FIXME  This mesh has a hardcoded value for node area set to M_PI * 0.002 * 0.002, which is the
 * area of the taxel currently used in the real (hardware) skin. However, this mesh can also be used
 * for Forces/Displacements and such
 */
class MeshNatural : public MeshInterface
{
public:
  template <class SensorIterator>
  MeshNatural(
    const size_t dim,
    SensorIterator sensors_begin,
    SensorIterator sensors_end
  ) :
    MeshInterface(dim, details::distance(sensors_begin, sensors_end)),
    taxelArea_(
      std::numeric_limits<double>::has_quiet_NaN ?
        std::numeric_limits<double>::quiet_NaN()
      :
        ((std::numeric_limits<double>::has_infinity) ?
         -1 * std::numeric_limits<double>::infinity()
        :
          std::numeric_limits<double>::lowest()
        )
    )
  {
    size_t n = 0;
    for (SensorIterator it = sensors_begin; it != sensors_end; ++it) {
      this->node(n).x = (*it).relative_position[0];
      this->node(n).y = (*it).relative_position[1];
      ++n;
    }
    generateMinMax();
    setNotForSkin();
  }

  template <class SensorIterator>
  MeshNatural(
    const size_t dim,
    SensorIterator sensors_begin,
    SensorIterator sensors_end,
    const SkinAttributes& skin_attr
  ) :
    MeshInterface(dim, details::distance(sensors_begin, sensors_end)),
    taxelArea_(skin_attr.taxelArea)
  {
    size_t n = 0;
    for (SensorIterator it = sensors_begin; it != sensors_end; ++it) {
      this->node(n).x = (*it).relative_position[0];
      this->node(n).y = (*it).relative_position[1];
      ++n;
    }
    generateMinMax();
    setForSkin();
  }

  /**
   * \brief Set the "used for skin" flag. Affects the node_area() function.
   */
  void setForSkin();

  /**
   * \brief Unset the "used for skin" flag. Affects the node_area() function.
   */
  void setNotForSkin();

  /**
   * \brief Check the "used for skin" flag.
   */
  bool isForSkin() const;

  MeshNatural& operator=(const MeshNatural&) = default;
  MeshNatural(const MeshNatural&)            = default;
  MeshNatural& operator=(MeshNatural&&)      = default;
  MeshNatural(MeshNatural&&)                 = default;
  ~MeshNatural()                             = default;

private:
  /**
   * Set to true if mesh is used for real skin, false if it is used e.g. for Forces
   *
   * Affects which node area is reported; for skin meshes, it will report the area of a taxel (which
   * is obtained from constructor's skin_attr parameter); for other meshes, it should report the
   * area of a Voronoi cell for a node. Since this is not implemented yet, will throw a
   * runtime_error.
   *
   * Can be also set through setForSkin() and setNotForSkin()
   */
  bool  forSkin_;
  const double taxelArea_;

  double impl_node_area(size_t i) const;
};

} /* namespace cm */

#endif /* MESHNATURAL_HPP */
