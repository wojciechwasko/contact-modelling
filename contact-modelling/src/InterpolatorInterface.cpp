#include "cm/interpolator/interface.hpp"

#include <stdexcept>
#include <algorithm>

#include "cm/grid/grid.hpp"
#include "cm/details/string.hpp"

namespace cm {

using cm::details::sb;

InterpolatorInterface::InterpolatorInterface(
  NIPP policy
) : bad_cell_policy(policy)
{

}

void
InterpolatorInterface::offline(const Grid& from, Grid& to)
{
  if (from.dim() !=  to.dim()) {
    throw std::runtime_error(
      sb()  << "Incompatible dimensionality of from and to grid: "
            << from.dim() << " vs. " << to.dim()
    );
  }

  std::vector<size_t> bad_points = impl_offline(from, to);
  applyNippOffline(to, bad_points);
  to.setBadCells(bad_points);
}

void 
InterpolatorInterface::applyNippOffline(
  Grid& to,
  std::vector<size_t>& bad_points
)
{
  // sort
  std::sort(bad_points.begin(), bad_points.end());
  // make unique
  bad_points.erase(std::unique(bad_points.begin(), bad_points.end()), bad_points.end());

  if (bad_cell_policy == NIPP::RemoveFromGrid) {
    to.erase(bad_points);
    bad_points.clear();
  } else if (bad_cell_policy == NIPP::InterpolateToZero) {
    // do nothing, essentially. This will be applied online 
  } else {
    throw std::runtime_error(
      sb()  << "Unrecognized Non-interpolable Point Policy."
    );
  }
}

void
InterpolatorInterface::interpolate(
  const Grid& from,
        Grid& to
)
{
  if (from.dim() !=  to.dim()) {
    throw std::runtime_error(
      sb()  << "Incompatible dimensionality of from and to grid: "
            << from.dim() << " vs. " << to.dim()
    );
  }

  const std::vector<size_t>& bad_cells = to.getBadCells();
  for (size_t n = 0; n < to.num_cells(); ++n) {
    if (applyNippOnline(to, bad_cells, n))
      continue;
    impl_interpolate(from, to, n);
  }
}

bool
InterpolatorInterface::applyNippOnline(
  Grid& to,
  const std::vector<size_t>& bad_cells, 
  const size_t n
)
{
  if (bad_cell_policy == NIPP::RemoveFromGrid)
    return false;

  if (
    bad_cell_policy == NIPP::InterpolateToZero &&
    std::binary_search(bad_cells.cbegin(), bad_cells.cend(), n)
  ) {
    for (size_t vi = 0; vi < to.dim(); ++vi)
      to.setValue(n, vi, 0);
    return true;
  }
  
  return false;
}

} /* namespace cm */
