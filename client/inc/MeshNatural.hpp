#ifndef MESHNATURAL_HPP
#define MESHNATURAL_HPP

#include <limits>
#include <cstddef>
#include <vector>
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
      sensors_end_(sensors_end),
      nodes(skin_helpers::distance(sensors_begin, sensors_end))
    {
      double min_x = std::numeric_limits<double>::max();
      double min_y = std::numeric_limits<double>::max();
      double max_x = std::numeric_limits<double>::min();
      double max_y = std::numeric_limits<double>::min();
      auto s_it = sensors_begin_;
      auto n_it = nodes.begin();
      size_t v_ind = 0;

      for (; s_it != sensors_end_; s_it++, n_it++) {
        n_it->x = s_it->relative_position[0];
        n_it->y = s_it->relative_position[1];
        for (size_t i = 0; i < n_it->val_dimensionality; ++i) {
          n_it->vals[i] = &(this->values_[v_ind]);
          ++v_ind;
        }
        // select min/max
        if (n_it->x < min_x) min_x = n_it->x;
        if (n_it->x > max_x) max_x = n_it->x;
        if (n_it->y < min_y) min_y = n_it->y;
        if (n_it->y > max_y) max_y = n_it->y;
      }

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

    container_type nodes;
    iterator impl_begin() { return nodes.begin(); };
    iterator impl_end()   { return nodes.end();   };
    const_iterator impl_cbegin() const { return nodes.cbegin(); };
    const_iterator impl_cend()   const { return nodes.cend();   };
    reference       impl_ra_nobounds(size_t n)       { return nodes[n]; };
    const_reference impl_ra_nobounds(size_t n) const { return nodes[n]; };
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
