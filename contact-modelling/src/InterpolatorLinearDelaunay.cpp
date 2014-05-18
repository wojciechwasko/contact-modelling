#include "cm/interpolator/linear_delaunay.hpp"

#include "cm/details/delaunay.hpp"
#include "cm/grid/grid.hpp"

namespace cm {
using details::Delaunay;

InterpolatorLinearDelaunay::InterpolatorLinearDelaunay(NIPP policy)
  : InterpolatorInterface(policy)
{

}

std::vector<size_t>
InterpolatorLinearDelaunay::impl_offline(const Grid& from, Grid& to)
{
  Delaunay dt(from);
  std::vector<size_t> nonInterpolableCells;

  for (size_t n = 0; n < to.num_cells(); ++n) {
    auto meta = dt.getTriangleInfoForPoint(to.cell(n));
    if (std::get<Delaunay::FAIL>(meta)) {
      nonInterpolableCells.push_back(n);
    }
    to.setMetadata(n, meta);
  }
  return nonInterpolableCells;
}

void InterpolatorLinearDelaunay::impl_interpolate(
  const Grid& from,
  Grid& to,
  const size_t n
)
{
  // this line might throw a boost::bad_any_cast in case of failure
  // TODO recover from failure and do something meaningful?
  const typename Delaunay::PointInTriangleMeta& meta =
      boost::any_cast<const typename Delaunay::PointInTriangleMeta&>(to.getMetadata(n));

  if (std::get<Delaunay::FAIL>(meta)) {
    throw std::runtime_error("This is not supposed to happen! Interpolator implementation "
      "cannot be asked to interpolate a non-interpolable point. Contact the authors.");
  }

  const size_t n0  = std::get<Delaunay::N0>(meta);
  const size_t n1  = std::get<Delaunay::N1>(meta);
  const size_t n2  = std::get<Delaunay::N2>(meta);

  const double ksi0 = std::get<Delaunay::KSI0>(meta);
  const double ksi1 = std::get<Delaunay::KSI1>(meta);
  const double ksi2 = std::get<Delaunay::KSI2>(meta);

  for (size_t dim = 0; dim < to.dim(); ++dim) {
    to.setValue(n, dim, 
      ksi0 * from.getValue(n0, dim) +
      ksi1 * from.getValue(n1, dim) +
      ksi2 * from.getValue(n2, dim)
    );
  }
}

} /* namespace cm */
