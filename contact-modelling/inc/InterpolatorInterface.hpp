#ifndef INTERPOLATORINTERFACE_HPP
#define INTERPOLATORINTERFACE_HPP

#include <vector>
#include <cstddef>

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

class MeshInterface;

/**
 * \brief   Interface for interpolating values given over one mesh to another mesh.
 *
 */
class InterpolatorInterface {
  public:
    const NIPP::NIPP bad_node_policy;
    virtual ~InterpolatorInterface() = default;

    /**
     * \brief   Perform offline step of the interpolation, storing the result in "to".
     */
    void offline(const MeshInterface& from, MeshInterface& to);

    /**
     * \brief   Perform actual interpolation, updating the values of "to" with interpolated values
     *          of "from"
     */
    void interpolate(const MeshInterface& from, MeshInterface& to);

  protected:
    InterpolatorInterface(NIPP::NIPP policy);

    InterpolatorInterface& operator=(const InterpolatorInterface&) = default;
    InterpolatorInterface(const InterpolatorInterface&)            = default;
    InterpolatorInterface& operator=(InterpolatorInterface&&)      = default;
    InterpolatorInterface(InterpolatorInterface&&)                 = default;

  private:
    // Possible performance degradation, since we'll call this (virtual) method for each point in
    // the "to" mesh
    /**
     * \returns   A vector of non-interpolable (bad) nodes' indices.
     */
    virtual std::vector<size_t> impl_offline(const MeshInterface& from, MeshInterface& to) = 0;

    virtual void impl_interpolate(
      const MeshInterface& from,
      MeshInterface& to,
      const size_t n
    ) = 0;

    /**
     * \brief   Internal use.
     * \param   nonInterpolableNodes  a vector of nodes which cannot be interpolated
     */
    void applyNippOffline(MeshInterface& to, std::vector<size_t>& bad_points);
    
    /**
     * \brief   Apply online-phase NIPP;
     * \return  true if policy was applied to the point, so it we should not forward the
     *          interpolation to the implementation, false otherwise.
     */
    bool applyNippOnline(MeshInterface& to, const std::vector<size_t>& bad_points, const size_t n);
};


#endif /* INTERPOLATORINTERFACE_HPP */
