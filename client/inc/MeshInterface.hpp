#ifndef MESHINTERFACE_HPP
#define MESHINTERFACE_HPP

#include <vector>
#include <cstddef>
#include <algorithm>

/**
 * \def COMMA
 * A nifty hack to enable processing of multiple templated classes in macros.
 * Inspired by http://stackoverflow.com/a/19841470/312650
 */
#ifndef COMMA
#define COMMA ,
#endif
/**
 * Injects common typedefs from MeshImpl_traits
 */
#define INJECT_MESH_TRAITS_TYPEDEFS(name) \
  typedef typename MeshImpl_traits< name >::node_type                                   node_type; \
  typedef typename MeshImpl_traits< name >::reference                                   reference; \
  typedef typename MeshImpl_traits< name >::const_reference                       const_reference; \
  typedef typename MeshImpl_traits< name >::iterator                                     iterator; \
  typedef typename MeshImpl_traits< name >::const_iterator                         const_iterator; \
  typedef typename MeshImpl_traits< name >::container_type                         container_type; \
  typedef typename MeshImpl_traits< name >::container_reference               container_reference; \
  typedef typename MeshImpl_traits< name >::container_const_reference   container_const_reference; 


template <class derived_t>
struct MeshImpl_traits;

/**
 * \brief   A mixin class to provide a common interface and various helper functions
 *
 * While it doesn't seem crucial right now, it'll come in handy when
 * we have to implement functions like extractROI and such.
 */
template <class Derived>
class MeshInterface {
  public:
    INJECT_MESH_TRAITS_TYPEDEFS(Derived)

  protected:
    /**
     * \brief   Values in the mesh'es nodes. Node's val/ (val_x, val_y, val_z) will
     * point to elements of this vector.
     * TODO   Think of enhancing encapsulation. The way I'm thinking to do it now is to 
     *        both write and read the values (vals) from Node stuct's pointers; hence,
     *        there's no granularity in access. A much better solution would allow the
     *        Interpolators/Algorithms to access the values rw, while the rest of the world,
     *        by ro. A solution somewhere in the middle would be to have additional ro pointers
     *        in the Node struct.
     *
     * TODO   Aproppriately initialize the size of this container, based on type of Node (1D or 3D)
     */
    std::vector<double> vals;
    /**
     * \brief   Implementation for random-access no bounds-checking access.
     */
    reference       impl_ra_nobounds(size_t n);
    /**
     * \brief   Implementation for random-access no bounds-checking access. (const version).
     */
    const_reference impl_ra_nobounds(size_t n) const;

  public:
    /**
     * \brief Get number of nodes in the mesh.
     */
    std::ptrdiff_t size() const {
      return std::distance(
        static_cast<Derived*>(this)->cbegin(),
        static_cast<Derived*>(this)->cend()
      );
    };

    /**
     * \brief Iterate over node in the mesh, executing a function. Const version.
     *
     * \param   f   Function to be called for each node. Takes node Derived::node as argument
     */
    template <class F>
    void for_each_node(F f) const {
      std::for_each(
        static_cast<Derived*>(this)->cbegin(),
        static_cast<Derived*>(this)->cend(),
        f
      );
    };

    /**
     * \brief Iterate over node in the mesh, executing a function.
     *
     * \param   f   Function to be called for each node. Takes node Derived::node as argument
     */
    template <class F>
    void for_each_node(F f) {
      std::for_each(
        static_cast<Derived*>(this)->begin(),
        static_cast<Derived*>(this)->end(),
        f
      );
    };

    /**
     * \brief Random access operator []
     * \note  This is not bound-checked.
     */
    reference       operator[](size_t n)       {
      return static_cast<Derived*>(this)->impl_ra_nobounds(n);
    };

    /**
     * \brief Random access operator []. Const version.
     * \note  This is not bound-checked.
     */
    const_reference operator[](size_t n) const {
      return static_cast<Derived*>(this)->impl_ra_nobounds(n);
    };

    /**
     * \brief Extracts the underlying container
     */
    container_reference       getRawValues()       {
      return static_cast<Derived*>(this)->nodes;
    };

    /**
     * \brief Extracts the underlying container
     */
    container_const_reference getRawValues() const {
      return static_cast<Derived*>(this)->nodes;
    };
};


#endif /* MESHINTERFACE_HPP */
