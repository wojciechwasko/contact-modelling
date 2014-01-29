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
  private:
    typedef typename std::vector<node_type> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename MeshImpl_traits<MeshNatural<node_type> >::reference             reference;
    typedef typename MeshImpl_traits<MeshNatural<node_type> >::const_reference const_reference;
    container nodes;
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
 * \note Sorry for the weird naming, but 14.6.1/7:
 * > A template-parameter shall not be redeclared within its scope
 * > (including nested scopes). A template-parameter shall not have
 * > the same name as the template name.
 */
template <class TNode>
struct MeshImpl_traits<MeshNatural<TNode> > {
  typedef TNode node_type;
  typedef TNode      &       reference;
  typedef TNode const& const_reference;
};



#endif /* MESHNATURAL_HPP */
