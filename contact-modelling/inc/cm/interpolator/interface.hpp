#ifndef INTERPOLATORINTERFACE_HPP
#define INTERPOLATORINTERFACE_HPP

#include <vector>
#include <cstddef>

/**
 * \file
 * \brief   Interface all interpolators implement
 */

namespace cm {

class Grid;

/**
 * \brief Non-Interpolable Point Policy
 *
 * What to do with "bad" cells. "Bad" -- cells which cannot be interpolated
 * (e.g. are outside of the source mesh).
 */
enum class NIPP {
  /**
   * set the value at those cells to zero
   */
  InterpolateToZero,
  /**
   * remove the bad cells from the mesh
   */
  RemoveFromGrid
};

/**
 * \brief   Interface for interpolating values given over one grid to another
 * grid.
 *
 */
class InterpolatorInterface {
public:
  /**
   * \brief The policy for dealing with bad cells
   * \sa NIPP
   */
  const NIPP bad_cell_policy;
  virtual ~InterpolatorInterface() = default;
  /**
   * \brief   Perform offline step of the interpolation, storing the result in
   * "to"'s metadata.
   */
  void offline(const Grid& from, Grid& to);

  /**
   * \brief   Perform actual interpolation, updating the values of "to" with
   * interpolated values of "from"
   */
  void interpolate(const Grid& from, Grid& to);

protected:
  InterpolatorInterface(NIPP policy);

  InterpolatorInterface& operator=(const InterpolatorInterface&) = default;
  InterpolatorInterface(const InterpolatorInterface&)            = default;
  InterpolatorInterface& operator=(InterpolatorInterface&&)      = default;
  InterpolatorInterface(InterpolatorInterface&&)                 = default;

private:
  /**
   * \returns   A vector of non-interpolable (bad) cells' indices.
   */
  virtual std::vector<size_t> impl_offline(const Grid& from, Grid& to) = 0;

  /**
   * \brief   Perform actual interpolation at cell n
   *
   * \note  There might be a performance penalty from calling this (virtual)
   * method for each point in the target grid.
   */
  virtual void impl_interpolate(
    const Grid& from,
    Grid& to,
    const size_t n
  ) = 0;

  /**
   * \brief   Internal use.
   * \param   nonInterpolableCells  a vector of cells which cannot be interpolated
   */
  void applyNippOffline(Grid& to, std::vector<size_t>& bad_points);
  
  /**
   * \brief   Apply online-phase NIPP;
   * \return  true if policy was applied to the point, so it we should not forward the
   *          interpolation to the implementation, false otherwise.
   */
  bool applyNippOnline(Grid& to, const std::vector<size_t>& bad_points, const size_t n);
};

} /* namespace cm */


#endif /* INTERPOLATORINTERFACE_HPP */
