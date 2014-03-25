#ifndef INTERPOLATORLINEARDELAUNAY_HPP
#define INTERPOLATORLINEARDELAUNAY_HPP

#include <type_traits>

#include "InterpolatorInterface.hpp"
#include "Delaunay.hpp"

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
  public:
    INJECT_INTERPOLATOR_TRAITS_TYPEDEFS(InterpolatorLinearDelaunay<sourceMeshType COMMA targetMeshType>)

  InterpolatorLinearDelaunay(const sourceMeshType& source_mesh, const targetMeshType& target_mesh)
  {
    triangulate(source_mesh);
  }

  protected:
    /**
     * \brief Actual implementation of online phase of Linear Delaunay interpolation
     */
    double impl_interpolate(
      const sourceMeshType& sourceMesh,
      const targetMeshType& targetMesh,
      typename targetMeshType::node_type& node
    )
    {
      return 1.4;
    }

  private:
    /**
     * \brief   Implementation of the triangulation, to be performed offline.
     */
    void triangulate(const sourceMeshType& source_mesh)
    {
      Delaunay<typename sourceMeshType::const_iterator> dt(source_mesh.cbegin(), source_mesh.cend());
    }
};

template <class sourceMeshType, class targetMeshType>
struct InterpolatorImpl_traits<InterpolatorLinearDelaunay<sourceMeshType, targetMeshType> > {
  typedef sourceMeshType source_mesh_type;
  typedef targetMeshType target_mesh_type;
};
#endif /* INTERPOLATORLINEARDELAUNAY_HPP */
