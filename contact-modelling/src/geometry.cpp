#include "cm/details/geometry.hpp"

#include <cmath>
#include <cstddef> // for size_t
#include <utility>
#include <stdexcept>

#include "cm/details/string.hpp"
#include "cm/details/math.hpp"

namespace cm {
namespace details {

double area_triangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
  return std::abs(
    0.5 * ( x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2) )
  ); 
}

segmentPlacement place_segments(const double t0, const double t1, const double dt)
{
  const double t_diff = t1-t0;
  if (t_diff < 0 || dt <= 0) {
    throw std::runtime_error(
        sb()  << "details::place_segments: Cannot place segments! "
              << "(t1-t0): " << t_diff << ", dt: " << dt
      );
  }
  const size_t num_segments = std::floor(t_diff/dt);
  const double covered_length = num_segments * dt;
  const double origin = t0 + ((eq_almost(covered_length,t_diff,1e-50)) ? 0 : 0.5 * (t_diff - covered_length));
  const double seg0mid = origin + dt/2.0;
  return {num_segments, origin, seg0mid};
}

} /* namespace details */
} /* namespace cm */

