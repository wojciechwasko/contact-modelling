#ifndef MESHNATURAL_HPP
#define MESHNATURAL_HPP

#include <cstddef>
#include <vector>

#include "MeshInterface.hpp"

/**
 * \brief   Mesh created from Skin's sensors.
 *
 * Each node corresponds to one sensor in the skin, with (x,y)
 * values copied over.
 */
template <class node_type>
class MeshNatural : public MeshInterface<MeshNatural<node_type> >
{
  friend class MeshInterface<MeshNatural<node_type> >;
  public:
    INJECT_MESH_TRAITS_TYPEDEFS(MeshNatural<node_type>)

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
template <class TNode>
struct MeshImpl_traits<MeshNatural<TNode> > {
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
