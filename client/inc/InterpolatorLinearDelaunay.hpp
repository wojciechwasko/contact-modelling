#ifndef INTERPOLATORLINEARDELAUNAY_HPP
#define INTERPOLATORLINEARDELAUNAY_HPP

#include <cmath>
#include <type_traits>
#include <vector>
#include <stdexcept>

#include <boost/any.hpp>

#include "InterpolatorInterface.hpp"
#include "Delaunay.hpp"

template <class sourceMeshType, class targetMeshType, NIPP::NIPP policy>
class InterpolatorLinearDelaunay;

template <class sourceMeshType, class targetMeshType, NIPP::NIPP policy_val>
struct InterpolatorImpl_traits<InterpolatorLinearDelaunay<sourceMeshType, targetMeshType, policy_val> > {
  typedef sourceMeshType source_mesh_type;
  typedef targetMeshType target_mesh_type;
  constexpr static NIPP::NIPP policy = policy_val;
};

/**
 * \brief   Implementation of a linear Delaunay triangulation-based interpolation
 * \tparam  sourceMeshType  type of the "input", "real" mesh. Almost always MeshNatural. But still...
 * \tparam  targetMeshType  type of mesh we'd like to interpolate onto.
 * \tparam  NIPP            Policy - what to do with points in the target mesh which are outside of
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
  NIPP::NIPP policy = NIPP::InterpolateToZero
>
class InterpolatorLinearDelaunay
  : public InterpolatorInterface<InterpolatorLinearDelaunay<sourceMeshType, targetMeshType, policy>>
{
friend class InterpolatorInterface<InterpolatorLinearDelaunay<sourceMeshType, targetMeshType, policy>>;
  
    typedef InterpolatorInterface<InterpolatorLinearDelaunay<sourceMeshType, targetMeshType, policy>> interface_type;
    typedef Delaunay<sourceMeshType> DelaunayImpl;

  public:
    INJECT_INTERPOLATOR_TRAITS_TYPEDEFS(InterpolatorLinearDelaunay<sourceMeshType COMMA targetMeshType COMMA policy>)

    InterpolatorLinearDelaunay(const sourceMeshType* source_mesh, targetMeshType* target_mesh)
      : interface_type(source_mesh, target_mesh)
    {
      auto s_mesh = this->getSourceMesh();
      auto t_mesh = this->getTargetMesh();
      DelaunayImpl dt(*s_mesh);
      std::vector<size_t> nonInterpolableNodes;

      for (size_t n = 0; n < t_mesh->no_nodes(); ++n) {
        auto meta = dt.getTriangleInfoForPoint(t_mesh->node(n));
        t_mesh->setMetadata(n, meta);
        if (std::get<DelaunayImpl::FAIL>(meta)) {
          nonInterpolableNodes.push_back(n);
        }
      }

      this->applyNIPP(nonInterpolableNodes);
    }

    InterpolatorLinearDelaunay& operator=(const InterpolatorLinearDelaunay&) = default;
    InterpolatorLinearDelaunay(const InterpolatorLinearDelaunay&)            = default;
    InterpolatorLinearDelaunay& operator=(InterpolatorLinearDelaunay&&)      = default;
    InterpolatorLinearDelaunay(InterpolatorLinearDelaunay&&)                 = default;
    ~InterpolatorLinearDelaunay()                                            = default;

  protected:
    /**
     * \brief Actual implementation of online phase of Linear Delaunay interpolation
     */
    double impl_interpolate(size_t n)
    {
      // this line might throw a boost::bad_any_cast in case of failure
      // TODO recover from failure and do something meaningful?
      const typename DelaunayImpl::PointInTriangleMeta& meta =
          boost::any_cast<const typename DelaunayImpl::PointInTriangleMeta&>(this->getTargetMesh()->getMetadata(n));

      if (std::get<DelaunayImpl::FAIL>(meta)) {
        throw std::runtime_error("This is not supposed to happen! Interpolator implementation "
          "cannot be asked to interpolate a non-interpolable point. Contact the authors.");
      }

      const size_t n0  = std::get<DelaunayImpl::N0>(meta);
      const size_t n1  = std::get<DelaunayImpl::N1>(meta);
      const size_t n2  = std::get<DelaunayImpl::N2>(meta);

      const double val0 = this->getSourceMesh()->getValue(n0, 0);
      const double val1 = this->getSourceMesh()->getValue(n1, 0);
      const double val2 = this->getSourceMesh()->getValue(n2, 0);

      const double ksi0 = std::get<DelaunayImpl::KSI0>(meta);
      const double ksi1 = std::get<DelaunayImpl::KSI1>(meta);
      const double ksi2 = std::get<DelaunayImpl::KSI2>(meta);
      const double ret = ksi0 * val0 + ksi1 * val1 + ksi2 * val2;
      return ret;
    }
};
#endif /* INTERPOLATORLINEARDELAUNAY_HPP */
