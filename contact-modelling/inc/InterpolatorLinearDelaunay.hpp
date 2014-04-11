#ifndef INTERPOLATORLINEARDELAUNAY_HPP
#define INTERPOLATORLINEARDELAUNAY_HPP

#include <cmath>
#include <vector>
#include <stdexcept>

#include <boost/any.hpp>

#include "InterpolatorInterface.hpp"


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
class InterpolatorLinearDelaunay : public InterpolatorInterface
{
  public:
    InterpolatorLinearDelaunay(NIPP::NIPP policy);

    InterpolatorLinearDelaunay& operator=(const InterpolatorLinearDelaunay&) = default;
    InterpolatorLinearDelaunay(const InterpolatorLinearDelaunay&)            = default;
    InterpolatorLinearDelaunay& operator=(InterpolatorLinearDelaunay&&)      = default;
    InterpolatorLinearDelaunay(InterpolatorLinearDelaunay&&)                 = default;
    ~InterpolatorLinearDelaunay()                                            = default;

  private:
    /**
     * \brief Actual implementation of offline phase of Linear Delaunay interpolation
     */
    std::vector<size_t> impl_offline(const MeshInterface& from, MeshInterface& to);

    /**
     * \brief Actual implementation of online phase of Linear Delaunay interpolation
     */
    void impl_interpolate(
      const MeshInterface& from,
      MeshInterface& to,
      const size_t n
    );

};
#endif /* INTERPOLATORLINEARDELAUNAY_HPP */
