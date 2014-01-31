#ifndef MESHREGULARSQUARE_HPP
#define MESHREGULARSQUARE_HPP

#include <vector>

#include "MeshInterface.hpp"

/**
 * \brief   Regular mesh with square base element.
 *
 * \cond PRIVATE
 * Later on, it'd be nice to implement custom iterators instead
 * of having to populate some internal std::vector. After all,
 * (x,y) positions are very easy to calculate on-the-fly.
 */
template <class node_type>
class MeshRegularSquare : public MeshInterface<MeshRegularSquare<node_type> >
{
  friend class MeshInterface<MeshRegularSquare<node_type> >;
  public:
    INJECT_MESH_TRAITS_TYPEDEFS(MeshRegularSquare<node_type>)

  private:
    container_type nodes;
    iterator begin() { return nodes.begin(); };
    iterator end()   { return nodes.end();   };
    const_iterator cbegin() const { return nodes.cbegin(); };
    const_iterator cend()   const { return nodes.cend();   };
    reference       operator[](size_t n)       { return nodes[n]; };
    const_reference operator[](size_t n) const { return nodes[n]; };
    reference       at(size_t n)       { return nodes.at(n); };
    const_reference at(size_t n) const { return nodes.at(n); };
  public:
};

/**
 * \brief type traits for Regular Square mesh.
 *
 * \note Sorry for the weird naming, but 14.6.1/7:
 * > A template-parameter shall not be redeclared within its scope
 * > (including nested scopes). A template-parameter shall not have
 * > the same name as the template name.
 */
template <class TNode>
struct MeshImpl_traits<MeshRegularSquare<TNode> > {
  typedef TNode                                  node_type;
  typedef node_type      &                       reference;
  typedef node_type const&                 const_reference;
  typedef typename std::vector<node_type>            container_type;
  typedef container_type      &        container_reference;
  typedef container_type const&  container_const_reference;
  typedef typename container_type::iterator                iterator;
  typedef typename container_type::const_iterator    const_iterator;
};

#endif /* MESHREGULARSQUARE_HPP */
