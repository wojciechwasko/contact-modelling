#ifndef MESHINTERFACE_HPP
#define MESHINTERFACE_HPP

#include <memory>
#include <cstddef>
#include <algorithm>

#include <Eigen/Dense>


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
    typedef Eigen::VectorXd vals_vec_type;

  protected:
    /**
     * \brief   Values in the mesh'es nodes. Node's val/ (val_x, val_y, val_z) will
     * point to elements of this vector.
     *
     * \note  Implementations may (and will) take advantage of the fact that this data (values) will
     *        remain at the same location in memory. Always. One major optimisation is that the
     *        Nodes will contain pointers directly to the values instead of having to look them up
     *        on each access. However, there is a downside - you CANNOT use move semantics or other
     *        tricks (e.g. raw buffer initialisation) to speed up copying of data. The simplest
     *        solution would be to simply get the new values and then iterate, copying data.
     *        However, I'm thinking of passing this vector (reference to it, actually) to the skin
     *        interface, which would write directly to this vector instead of returning a fresh one.
     *
     * TODO   Think of enhancing encapsulation. The way I'm thinking to do it now is to 
     *        both write and read the values (vals) from Node stuct's pointers; hence,
     *        there's no granularity in access. A much better solution would allow the
     *        Interpolators/Algorithms to access the values rw, while the rest of the world,
     *        by ro. A solution somewhere in the middle would be to have additional ro pointers
     *        in the Node struct.
     */
    vals_vec_type values_;
    /**
     * \brief   Implementation for random-access no bounds-checking access.
     */
    reference       impl_ra_nobounds(size_t n);
    /**
     * \brief   Implementation for random-access no bounds-checking access. (const version).
     */
    const_reference impl_ra_nobounds(size_t n) const;

    MeshInterface(size_t no_nodes)
      : values_(node_type::val_dimensionality * no_nodes)
    {}
  public:

    const_iterator cbegin() const { return static_cast<const Derived*>(this)->impl_cbegin(); }
    const_iterator   cend() const { return static_cast<const Derived*>(this)->impl_cend();   }

    iterator begin() { return static_cast<Derived*>(this)->impl_begin(); }
    iterator   end() { return static_cast<Derived*>(this)->impl_end();   }

    /**
     * \brief Get number of nodes in the mesh.
     */
    std::ptrdiff_t size() const { return std::distance(cbegin(), cend()); }

    /**
     * \brief Iterate over node in the mesh, executing a function. Const version.
     *
     * \param   f   Function to be called for each node. Takes node Derived::node as argument
     */
    template <class F>
    void for_each_node(F f) const { std::for_each(cbegin(), cend(), f); }

    /**
     * \brief Iterate over node in the mesh, executing a function.
     *
     * \param   f   Function to be called for each node. Takes node Derived::node as argument
     */
    template <class F>
    void for_each_node(F f) { std::for_each(begin(), end(), f); }

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
     * \brief   Get a constant reference to the values vector.
     */
    const vals_vec_type& getValues() const
    {
      return values_;
    };

    /**
     * \brief   Get a reference to the values vector.
     */
    vals_vec_type& getValues() 
    {
      return values_;
    };
};


#endif /* MESHINTERFACE_HPP */
