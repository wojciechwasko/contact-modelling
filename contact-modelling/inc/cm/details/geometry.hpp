#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <cstddef> // for size_t
#include <utility>

#include "cm/details/string.hpp"

/**
 * \cond DEV
 */

/**
 * \file 
 * \brief   Geometrical helpers.
 */

namespace cm {
namespace details {

/**
 * \brief   Calculate area of a triangle defined by cells (x1,y1), (x2,y2), (x3,y3)
 */
double area_triangle(double x1, double y1, double x2, double y2, double x3, double y3);

/**
 * \brief   Templated version of area_triangle, delegates to the non-templated one.
 * \tparam  point_type  a type which has .x and .y attributes accessible
 */
template <class point_type>
inline double area_triangle(const point_type& p1, const point_type& p2, const point_type& p3)
{
  return area_triangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

/**
 * \brief   Return structure for place_segments
 */
struct segmentPlacement {
  /**
   * \brief   Number of segments
   */
  size_t num;
  /**
   * \brief   Value of the coordinate for the first segment
   */
  double origin;
  /**
   * \brief   Middle of the first segment
   */
  double seg0middle;
};

/**
 * \brief   How many segments of length dt can fit between t0 and t1 and where
 * they should start to be centred.
 *
 */
segmentPlacement place_segments(const double t0, const double t1, const double dt);

} /* namespace details */
} /* namespace cm */

/**
 * \endcond
 */

#endif /* GEOMETRY_HPP */
