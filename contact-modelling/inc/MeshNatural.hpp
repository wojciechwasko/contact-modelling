#ifndef MESHNATURAL_HPP
#define MESHNATURAL_HPP

#include <cstdint>
#include <algorithm>

#include "MeshInterface.hpp"
#include "helpers/skin.hpp"

/**
 * \brief   Mesh created from Skin's sensors.
 * \tparam  dim   dimensionality of the values stored in the node.
 * 
 * \note  All the algorithms in this library consider that if D == 1, we consider forces in the "z"
 *        direction.
 *
 * Each node corresponds to one sensor in the skin, with (x,y)
 * values copied over.
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
    MeshInterface(dim, helpers::skin::distance(sensors_begin, sensors_end))
  {
    size_t n = 0;
    for (SensorIterator it = sensors_begin; it != sensors_end; ++it) {
      this->node(n).x = (*it).relative_position[0];
      this->node(n).y = (*it).relative_position[1];
      ++n;
    }
    generateMinMax();
  }

  MeshNatural& operator=(const MeshNatural&) = default;
  MeshNatural(const MeshNatural&)            = default;
  MeshNatural& operator=(MeshNatural&&)      = default;
  MeshNatural(MeshNatural&&)                 = default;
  ~MeshNatural()                             = default;

private:
  double impl_node_area(size_t i) const;
};



#endif /* MESHNATURAL_HPP */
