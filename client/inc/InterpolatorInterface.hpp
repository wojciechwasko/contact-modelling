#ifndef INTERPOLATORINTERFACE_HPP
#define INTERPOLATORINTERFACE_HPP

#include <cstddef>
#include <stdexcept>
#include <algorithm>

/**
 * Non-Interpolable Point Policy
 */
namespace NIPP {
  /**
   * Non-Interpolable Point Policy
   */
  enum NIPP {
    InterpolateToZero,
    RemoveFromMesh
  };
}

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
    source_mesh_type::D == target_mesh_type::D,
    "You can't use interpolation over two meshes with different dimensionality"
  );

  static_assert(
    target_mesh_type::D == 1,
    "Interpolation is not (yet) implemented for >1D"
  );

  protected:
    InterpolatorInterface(const source_mesh_type* source_mesh, target_mesh_type* target_mesh)
      : source_mesh_(source_mesh), target_mesh_(target_mesh)
    {
    }

    /**
     * \brief   Internal use. Called by child classes after metadata has been gathered, to apply
     *          NonInterpolablePointPolicy
     * \param   nonInterpolableNodes  a vector of nodes which cannot be interpolated
     */
    void applyNIPP(const std::vector<size_t>& nonInterpolableNodes)
    {
      bad_points_ = nonInterpolableNodes;
      // sort
      std::sort(bad_points_.begin(), bad_points_.end());
      // make unique
      bad_points_.erase(std::unique(bad_points_.begin(), bad_points_.end()), bad_points_.end());

      if (policy_ == NIPP::RemoveFromMesh) {
        this->getTargetMesh()->erase(bad_points_);
        bad_points_.clear();
      } else if (policy_ == NIPP::InterpolateToZero) {
        // do nothing, essentially. This will be applied online 
      } else {
        throw std::runtime_error("Unrecognized Non-interpolable Point Policy.");
      }
    }

    /**
     * \brief   Actual interpolation function
     * \param   n   node in the target mesh to interpolate in
     * \returns     Interpolated value
     *
     * \note    This method does *NOT* modify the actual values in the target mesh.
     *          Rather, it returns the value.
     */
    double impl_interpolate(size_t n);

    /**
     * \brief Internal use. Get a reference to the target mesh.
     */
    target_mesh_type* getTargetMesh() { return target_mesh_; }

    /**
     * \brief Internal use. Get a reference to the source mesh.
     */
    const source_mesh_type* getSourceMesh() const { return source_mesh_; }

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
    double interpolateSingle(size_t n)
    {
      if (
        policy_ == NIPP::InterpolateToZero &&
        std::binary_search(bad_points_.begin(), bad_points_.end(), n)
      ) {
        return 0;
      }

      return static_cast<Implementation*>(this)
        ->impl_interpolate(n); 
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
      target_mesh_type* targetMesh = getTargetMesh();
      for (size_t n = 0; n < targetMesh->size(); ++n) {
        targetMesh->setValue(n, 0, interpolateSingle(n));
      }
    }

  private:
    constexpr static NIPP::NIPP policy_ = InterpolatorImpl_traits<Implementation>::policy;
    const source_mesh_type *const source_mesh_;
          target_mesh_type *const target_mesh_;
    /**
     * \brief   A vector of nodes' IDs in the target mesh which cannot be interpolated.
     * \note    This vector can be assumed to be sorted. (so that we can use binary_search)
     */
    std::vector<size_t> bad_points_;
};


#endif /* INTERPOLATORINTERFACE_HPP */
