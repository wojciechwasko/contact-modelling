#ifndef INTERPOLATORINTERFACE_HPP
#define INTERPOLATORINTERFACE_HPP

#include <cstddef>

#include "helpers/iterate.hpp"
#include "Point.hpp"

/**
 * \def COMMA
 * A nifty hack to enable processing of multiple templated classes in macros.
 * Inspired by http://stackoverflow.com/a/19841470/312650
 */
#ifndef COMMA
#define COMMA ,
#endif
/**
 * Injects common typedefs from InterpolatorImpl_traits
 */
#define INJECT_INTERPOLATOR_TRAITS_TYPEDEFS(name) \
  typedef typename InterpolatorImpl_traits< name >::source_mesh_type source_mesh_type; \
  typedef typename InterpolatorImpl_traits< name >::target_mesh_type target_mesh_type;

template <class derived_t>
struct InterpolatorImpl_traits;

/**
 * \brief   Interface for interpolating values given over one mesh to another mesh.
 * \tparam  Implementation Actual implementation of the interpolating algorithm
 *
 * Common mixin interface for interpolation methods. User will not instantiate this directly
 * but will use this interface for interacting with the interpolation algorithm
 */
template <class Implementation>
class InterpolatorInterface {
  public:
    INJECT_INTERPOLATOR_TRAITS_TYPEDEFS(Implementation)

  static_assert(
    std::is_same<
      typename MeshImpl_traits<source_mesh_type>::node_type,
      typename MeshImpl_traits<target_mesh_type>::node_type
    >::value,
    "You can't use interpolation over two meshes with different underlying node type"
  );

  static_assert(
    MeshImpl_traits<target_mesh_type>::node_type::val_dimensionality == 1,
    "Interpolation is not (yet) implemented for >1D"
  );

  protected:
    InterpolatorInterface() {}
    /**
     * \brief   Actual interpolation function
     * \param   targetNode  node in the target mesh to interpolate in
     * \returns             Interpolated value
     *
     * \note    This method does *NOT* modify the actual values in the target mesh.
     *          Rather, it returns the value.
     */
    double impl_interpolate(
      typename target_mesh_type::const_iterator node
    );

  public:
    /**
     * \brief   interface method for performing the interpolation in one node
     * \param   sourceMesh  source mesh with current values
     * \param   targetMesh  target mesh
     * \param   targetNode  node in the target mesh to interpolate in
     *
     * \note  This method does *NOT* modify the targetMesh in any way; if you want to save the returned
     *        value to the mesh, do it manually. 
     */
    double interpolateSingle(
      typename target_mesh_type::const_iterator node
    )
    {
      return static_cast<Implementation*>(this)
        ->impl_interpolate(node); 
    }

    /**
     * \brief   interface method for performing the interpolation over the whole target mesh
     * \param   sourceMesh  source mesh with current values
     * \param   targetMesh  target mesh
     *
     * Under the hood, it simply iterates over the target mesh and interpolates for each node, saving
     * the value to the node's 'val'. TODO what to do in 1d and 3d cases?
     */
    void interpolateBulk()
    {
      using helpers::iterate::for_each_it;
      target_mesh_type & targetMesh = static_cast<Implementation*>(this)->getTargetMesh();
      for_each_it(
        targetMesh.begin(),
        targetMesh.end(),
        [&] (typename target_mesh_type::iterator& n_it) {
          *(n_it->vals[0]) = interpolateSingle(n_it); 
        });
    }
};


#endif /* INTERPOLATORINTERFACE_HPP */
