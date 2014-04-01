#ifndef MESHNATURAL_HPP
#define MESHNATURAL_HPP

#include <limits>
#include <cstddef>
#include <vector>
#include <iterator>
#include <algorithm>

#include "skin_helpers.hpp"
#include "MeshInterface.hpp"

/**
 * \brief   Mesh created from Skin's sensors.
 *
 * Each node corresponds to one sensor in the skin, with (x,y)
 * values copied over.
 */
template <class TNode, class SkinConnector>
class MeshNatural : public MeshInterface<MeshNatural<TNode, SkinConnector> >
{
  typedef typename SkinConnector::sensor_iterator sensor_iterator;
  sensor_iterator sensors_begin_;
  sensor_iterator   sensors_end_;

  friend class MeshInterface<MeshNatural<TNode, SkinConnector> >;
  typedef MeshInterface<MeshNatural<TNode, SkinConnector> > interface_type;

  public:
    INJECT_MESH_TRAITS_TYPEDEFS(MeshNatural<TNode COMMA SkinConnector>)
    MeshNatural(
      sensor_iterator sensors_begin,
      sensor_iterator sensors_end
    ) :
      interface_type(skin_helpers::distance(sensors_begin, sensors_end)),
      sensors_begin_(sensors_begin),
      sensors_end_(sensors_end)
    {
      double min_x = std::numeric_limits<double>::max();
      double min_y = std::numeric_limits<double>::max();
      double max_x = std::numeric_limits<double>::min();
      double max_y = std::numeric_limits<double>::min();
      size_t n = 0;
      for (sensor_iterator it = sensors_begin_; it != sensors_end_; ++it) {
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

    double minX() const { return min_x_; }
    double minY() const { return min_y_; }
    double maxX() const { return max_x_; }
    double maxY() const { return max_y_; }

  private:
    double min_x_;    double min_y_;
    double max_x_;    double max_y_;
};

/**
 * \brief type traits for Natural mesh.
 *
 * \note Sorry for the weird TNode/node_type naming, but 14.6.1/7:
 * > A template-parameter shall not be redeclared within its scope
 * > (including nested scopes). A template-parameter shall not have
 * > the same name as the template name.
 */
template <class TNode, class SkinConnector>
struct MeshImpl_traits<MeshNatural<TNode, SkinConnector> > {
  typedef TNode                                  node_type;
  typedef node_type      &                       reference;
  typedef node_type const&                 const_reference;
  typedef typename std::vector<node_type>            container_type;
  typedef container_type      &        container_reference;
  typedef container_type const&  container_const_reference;
  typedef typename container_type::iterator                iterator;
  typedef typename container_type::const_iterator    const_iterator;
};



#endif /* MESHNATURAL_HPP */
