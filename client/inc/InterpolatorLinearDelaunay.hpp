#ifndef INTERPOLATORLINEARDELAUNAY_HPP
#define INTERPOLATORLINEARDELAUNAY_HPP

#include "InterpolatorInterface.hpp"

/**
 * \brief   Implementation of a linear Delaunay triangulation-based interpolation
 * \tparam  sourceMeshType  type of the "input", "real" mesh. Almost always MeshNatural. But still...
 * \tparam  targetMeshType  type of mesh we'd like to interpolate onto.
 *
 * \note  We have to pass all those templates to the interpolators, but that's the price we hve to
 *        pay for static, compile-time polymorphism.
 */
template <class sourceMeshType, class targetMeshType>
class InterpolatorLinearDelaunay
  : public InterpolatorInterface<
    InterpolatorLinearDelaunay<sourceMeshType, targetMeshType>
  >
{
  // FIXME do we need this friend declaration?
  friend class InterpolatorInterface<
    InterpolatorLinearDelaunay<sourceMeshType, targetMeshType>
  >;
  protected:
    /**
     * \brief   Implementation of the offline step.
     *
     * Delaunay triangulation will be performed here.
     */
    void impl_offline(void)
    {

    }
    /**
     * \brief Actual implementation of online phase of Linear Delaunay interpolation
     */
    double impl_interpolate(
      const sourceMeshType& sourceMesh,
      const targetMeshType& targetMesh,
      size_t targetNode
    )
    {
      return 1.;
    }
};

template <class sourceMeshType, class targetMeshType>
struct InterpolatorImpl_traits<InterpolatorLinearDelaunay<sourceMeshType, targetMeshType> > {
  typedef sourceMeshType source_mesh_type;
  typedef targetMeshType target_mesh_type;
};
#endif /* INTERPOLATORLINEARDELAUNAY_HPP */
