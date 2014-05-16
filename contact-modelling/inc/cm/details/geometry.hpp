#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <cmath>

namespace cm {
namespace details {

/**
 * \brief   Calculate area of a triangle defined by nodes (x1,y1), (x2,y2), (x3,y3)
 */
double area_triangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
  return std::abs(
    0.5 * ( x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2) )
  ); 
}

/**
 * \brief   Templated version of area_triangle, delegates to the non-templated one.
 * \tparam  point_type  a type which has .x and .y attributes accessible
 */
template <class point_type>
double area_triangle(const point_type& p1, const point_type& p2, const point_type& p3)
{
  return area_triangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

} /* namespace details */
} /* namespace cm */


#endif /* GEOMETRY_HPP */
