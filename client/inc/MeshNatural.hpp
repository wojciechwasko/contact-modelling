#ifndef MESHNATURAL_HPP
#define MESHNATURAL_HPP

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
template <class TNode, class SkinSensorIterator>
class MeshNatural : public MeshInterface<MeshNatural<TNode, SkinSensorIterator> >
{
  SkinSensorIterator sensors_begin_;
  SkinSensorIterator   sensors_end_;

  friend class MeshInterface<MeshNatural<TNode, SkinSensorIterator> >;
  typedef MeshInterface<MeshNatural<TNode, SkinSensorIterator> > interface_type;

  public:
    INJECT_MESH_TRAITS_TYPEDEFS(MeshNatural<TNode COMMA SkinSensorIterator>)
    MeshNatural(
      SkinSensorIterator sensors_begin,
      SkinSensorIterator sensors_end
    ) :
      interface_type(skin_helpers::distance(sensors_begin, sensors_end)),
      sensors_begin_(sensors_begin),
      sensors_end_(sensors_end),
      nodes(skin_helpers::distance(sensors_begin, sensors_end))
    {
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
      }
    }

  private:
    container_type nodes;
    iterator begin() { return nodes.begin(); };
    iterator end()   { return nodes.end();   };
    const_iterator cbegin() const { return nodes.cbegin(); };
    const_iterator cend()   const { return nodes.cend();   };
    reference       impl_ra_nobounds(size_t n)       { return nodes[n]; };
    const_reference impl_ra_nobounds(size_t n) const { return nodes[n]; };
    
  public:
};

/**
 * \brief type traits for Natural mesh.
 *
 * \note Sorry for the weird TNode/node_type naming, but 14.6.1/7:
 * > A template-parameter shall not be redeclared within its scope
 * > (including nested scopes). A template-parameter shall not have
 * > the same name as the template name.
 */
template <class TNode, class SkinSensorIterator>
struct MeshImpl_traits<MeshNatural<TNode, SkinSensorIterator> > {
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
