#ifndef INTERPOLATORINTERFACE_HPP
#define INTERPOLATORINTERFACE_HPP

#include <cstddef>

#include "Point.hpp"

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
  typedef typename InterpolatorImpl_traits<Implementation>::source_mesh_type
    source_mesh_type;
  typedef typename InterpolatorImpl_traits<Implementation>::target_mesh_type
    target_mesh_type;

  protected:
    /**
     * \brief   Actual interpolation function
     * \param   sourceMesh  mesh to take the "true" values from
     * \param   targetMesh  mesh to interpolate the function in
     * \param   targetNode  node in the target mesh to interpolate in
     * \returns             Interpolated value
     *
     * \note    This method does *NOT* modify the actual values in the target mesh.
     *          Rather, it returns the value.
     */
    double impl_interpolate(
      const source_mesh_type& sourceMesh,
      const target_mesh_type& targetMesh,
      size_t targetNode
    );

    /**
     * \brief   Optional offline step to be performed.
     *
     * \note    This method has a default empty implementation, so if there is
     *          no offline computation to be done, the implementation class does
     *          not have to override this method with empty impl_offline()
     */
    void   impl_offline(void) {}

  public:
    /**
     * \brief   interface method for doing any offline preparation
     */
    void offline(void) { static_cast<Implementation*>(this)->impl_offline(); };

    /**
     * \brief   interface method for performing the interpolation in one node
     * \param   sourceMesh  source mesh with current values
     * \param   targetMesh  target mesh
     * \param   p           2D point in which we'd like to know the value
     *
     * \note  This method does *NOT* modify the targetMesh in any way; if you want to save the returned
     *        value to the mesh, do it manually. 
     */
    double interpolateSingle(
      const source_mesh_type& sourceMesh,
      const target_mesh_type& targetMesh,
      size_t targetNode
    )
    {
      return static_cast<Implementation*>(this)
        ->impl_interpolate(sourceMesh, targetMesh, targetNode); 
    }

    /**
     * \brief   interface method for performing the interpolation over the whole target mesh
     * \param   sourceMesh  source mesh with current values
     * \param   targetMesh  target mesh
     *
     * Under the hood, it simply iterates over the target mesh and interpolates for each node, saving
     * the value to the node's 'val'. TODO what to do in 1d and 3d cases?
     */
    void interpolateBulk(
      const source_mesh_type& sourceMesh,
            target_mesh_type& targetMesh
    )
    {
      // TODO implement
    }
};


#endif /* INTERPOLATORINTERFACE_HPP */
