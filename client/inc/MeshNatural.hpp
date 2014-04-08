#ifndef MESHNATURAL_HPP
#define MESHNATURAL_HPP

#include <limits>
#include <cstddef>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <algorithm>

#include "helpers/skin.hpp"
#include "MeshInterface.hpp"

template <size_t dim>
class MeshNatural;

/**
 * \brief type traits for Natural mesh.
 */
template <size_t dim>
struct MeshImpl_traits<MeshNatural<dim> > {

};

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
template <size_t dim>
class MeshNatural : public MeshInterface<MeshNatural<dim>,dim>
{

  friend class MeshInterface<MeshNatural<dim>,dim>;
  typedef MeshInterface<MeshNatural<dim>,dim> interface_type;

  public:
    INJECT_MESH_INTERFACE_TYPES(MeshNatural<dim> COMMA dim)

    template <class SensorIterator>
    MeshNatural(
      SensorIterator sensors_begin,
      SensorIterator sensors_end
    ) :
      interface_type(helpers::skin::distance(sensors_begin, sensors_end))
    {
      double min_x = std::numeric_limits<double>::max();
      double min_y = std::numeric_limits<double>::max();
      double max_x = std::numeric_limits<double>::min();
      double max_y = std::numeric_limits<double>::min();
      size_t n = 0;
      for (SensorIterator it = sensors_begin; it != sensors_end; ++it) {
        const double x = (*it).relative_position[0];
        const double y = (*it).relative_position[1];
        this->node(n).x = x;
        this->node(n).y = y;
        // select min/max
        if (x < min_x) min_x = x;
        if (x > max_x) max_x = x;
        if (y < min_y) min_y = y;
        if (y > max_y) max_y = y;
        ++n;
      }

      // save min/max to member variables
      min_x_ = min_x;
      min_y_ = min_y;
      max_x_ = max_x;
      max_y_ = max_y;
    }

    MeshNatural& operator=(const MeshNatural&) = default;
    MeshNatural(const MeshNatural&)            = default;
    MeshNatural& operator=(MeshNatural&&)      = default;
    MeshNatural(MeshNatural&&)                 = default;
    ~MeshNatural()                             = default;

    double minX() const { return min_x_; }
    double minY() const { return min_y_; }
    double maxX() const { return max_x_; }
    double maxY() const { return max_y_; }

  protected:
    const double impl_node_area(size_t i) const
    {
      throw std::runtime_error("Node areas for Natural Mesh are not yet implemented. "
        "Do it if you have the time! :D");
    }

    void hook_post_erase(const std::vector<size_t>& indices)
    {
      if (0 == indices.size())
        return;
      // else, regenerate the min/max_x/y_ values
      double min_x = std::numeric_limits<double>::max();
      double min_y = std::numeric_limits<double>::max();
      double max_x = std::numeric_limits<double>::min();
      double max_y = std::numeric_limits<double>::min();

      std::for_each(this->nodes_cbegin(), this->nodes_cend(), [&](const node_type& n) {
        if (n.x < min_x) min_x = n.x;
        if (n.x > max_x) max_x = n.x;
        if (n.y < min_y) min_y = n.y;
        if (n.y > max_y) max_y = n.y;
      });
      // save min/max to member variables
      min_x_ = min_x;
      min_y_ = min_y;
      max_x_ = max_x;
      max_y_ = max_y;
    }


  private:
    double min_x_;    double min_y_;
    double max_x_;    double max_y_;
};



#endif /* MESHNATURAL_HPP */
