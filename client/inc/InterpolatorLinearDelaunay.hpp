#ifndef INTERPOLATORLINEARDELAUNAY_HPP
#define INTERPOLATORLINEARDELAUNAY_HPP

#include <cmath>
#include <type_traits>
#include <vector>
#include <stdexcept>

#include "InterpolatorInterface.hpp"
#include "Delaunay.hpp"

namespace PointsOutsideSourceMeshPolicy {
  enum PointsOutsideSourceMeshPolicy {
    InterpolateToZero,
    RemoveFromMesh
  };
}

/**
 * \brief   Implementation of a linear Delaunay triangulation-based interpolation
 * \tparam  sourceMeshType  type of the "input", "real" mesh. Almost always MeshNatural. But still...
 * \tparam  targetMeshType  type of mesh we'd like to interpolate onto.
 * \tparam  POSMP           Policy - what to do with points in the target mesh which are outside of
 *                          the triangulation area of the source mesh. By default, their value will
 *                          be set to zero (InterpolateToZero); It is also possible to remove those
 *                          points from the target mesh.
 *
 * \note  We have to pass all those templates to the interpolators, but that's the price we hve to
 *        pay for static, compile-time polymorphism.
 *
 * Note that a particular instance of the Interpolator is "bound" to a particular source/target mesh
 * (there's a fair bit of offline computation involved in the constructor). Maybe later on, I'll add
 * a method to change the meshes we're using as source and target, but right now I don't see the
 * point in doing that right now. If you want to use a different mesh, you might just as well create
 * a new instance of the interpolator.
 */
template <
  class sourceMeshType,
  class targetMeshType,
  PointsOutsideSourceMeshPolicy::PointsOutsideSourceMeshPolicy POSMP = PointsOutsideSourceMeshPolicy::InterpolateToZero
>
class InterpolatorLinearDelaunay
  : public InterpolatorInterface<InterpolatorLinearDelaunay<sourceMeshType, targetMeshType, POSMP>>
{
  // ASSERTIONS
  static_assert(POSMP != PointsOutsideSourceMeshPolicy::RemoveFromMesh, "Removing points from target mesh is not yet implemented.");
  // ASSERTIONS -- end

  // FRIEND DECLARATIONS
  friend class InterpolatorInterface<InterpolatorLinearDelaunay<sourceMeshType, targetMeshType, POSMP>>;
  // FRIEND DECLARATIONS -- end
  
    // PRIVATE TYPEDEFS
    typedef Delaunay<typename sourceMeshType::const_iterator> DelaunayImpl;
    typedef typename DelaunayImpl::PointInTriangleMeta TargetPointMeta;

    // PRIVATE MEMBERS DECLARATIONS
    const sourceMeshType *source_mesh_;
    targetMeshType *target_mesh_;

    std::vector<TargetPointMeta> target_points_meta_;

  public:
    INJECT_INTERPOLATOR_TRAITS_TYPEDEFS(InterpolatorLinearDelaunay<sourceMeshType COMMA targetMeshType COMMA POSMP>)

    InterpolatorLinearDelaunay(const sourceMeshType& source_mesh, targetMeshType& target_mesh)
      : source_mesh_(&source_mesh), target_mesh_(&target_mesh)
    {
      typedef typename targetMeshType::node_type target_node_type;

      DelaunayImpl dt(source_mesh_->cbegin(), source_mesh_->cend());
      target_points_meta_.reserve(dt.getNoTriangles());

      std::for_each(target_mesh_->cbegin(), target_mesh_->cend(), [&] (target_node_type const& n) {
        target_points_meta_.push_back(dt.getTriangleInfoForPoint(n));        
      });
    }

  protected:
    /**
     * \brief Internal use. Get a reference to the target mesh.
     */
    targetMeshType & getTargetMesh() { return *target_mesh_; }

    /**
     * \brief Actual implementation of online phase of Linear Delaunay interpolation
     */
    double impl_interpolate(
      typename targetMeshType::const_iterator n_it
    )
    {
      size_t node_index = std::distance(target_mesh_->cbegin(), n_it);
      const TargetPointMeta& meta = target_points_meta_[node_index];

      if (std::get<DelaunayImpl::FAIL>(meta)) {
        if (POSMP == PointsOutsideSourceMeshPolicy::InterpolateToZero)
          return 0;
        else
          throw std::runtime_error("I was asked to interpolate a point outside of the mesh, but I don't have a policy for that");
      }

      const double val0 = *((*source_mesh_)[std::get<DelaunayImpl::N0>(meta)].vals[0]);
      const double val1 = *((*source_mesh_)[std::get<DelaunayImpl::N1>(meta)].vals[0]);
      const double val2 = *((*source_mesh_)[std::get<DelaunayImpl::N2>(meta)].vals[0]);

      const double ksi0 = std::get<DelaunayImpl::KSI0>(meta);
      const double ksi1 = std::get<DelaunayImpl::KSI1>(meta);
      const double ksi2 = std::get<DelaunayImpl::KSI2>(meta);
      return ksi0 * val0 + ksi1 * val1 + ksi2 * val2;
    }

  private:
};

template <class sourceMeshType, class targetMeshType>
struct InterpolatorImpl_traits<InterpolatorLinearDelaunay<sourceMeshType, targetMeshType> > {
  typedef sourceMeshType source_mesh_type;
  typedef targetMeshType target_mesh_type;
};
#endif /* INTERPOLATORLINEARDELAUNAY_HPP */
