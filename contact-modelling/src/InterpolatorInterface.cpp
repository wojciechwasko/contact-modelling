#include "InterpolatorInterface.hpp"

#include <stdexcept>
#include <algorithm>

#include "MeshInterface.hpp"
#include "helpers/string.hpp"

using helpers::string::sb;

InterpolatorInterface::InterpolatorInterface(
  NIPP::NIPP policy
) : bad_node_policy(policy)
{

}

void
InterpolatorInterface::offline(const MeshInterface& from, MeshInterface& to)
{
  if (from.D !=  to.D) {
    throw std::runtime_error(
      sb()  << "Incompatible dimensionality of from and to mesh: "
            << from.D << " vs. " << to.D
    );
  }

  std::vector<size_t> bad_points = impl_offline(from, to);
  applyNippOffline(to, bad_points);
  to.setBadNodes(bad_points);
}

void 
InterpolatorInterface::applyNippOffline(
  MeshInterface& to,
  std::vector<size_t>& bad_points
)
{
  // sort
  std::sort(bad_points.begin(), bad_points.end());
  // make unique
  bad_points.erase(std::unique(bad_points.begin(), bad_points.end()), bad_points.end());

  if (bad_node_policy == NIPP::RemoveFromMesh) {
    to.erase(bad_points);
    bad_points.clear();
  } else if (bad_node_policy == NIPP::InterpolateToZero) {
    // do nothing, essentially. This will be applied online 
  } else {
    throw std::runtime_error(
      sb()  << "Unrecognized Non-interpolable Point Policy: "
            << bad_node_policy
    );
  }
}

void
InterpolatorInterface::interpolate(
  const MeshInterface& from,
        MeshInterface& to
)
{
  if (from.D !=  to.D) {
    throw std::runtime_error(
      sb()  << "Incompatible dimensionality of from and to mesh: "
            << from.D << " vs. " << to.D
    );
  }

  const std::vector<size_t>& bad_nodes = to.getBadNodes();
  for (size_t n = 0; n < to.no_nodes(); ++n) {
    if (applyNippOnline(to, bad_nodes, n))
      continue;
    impl_interpolate(from, to, n);
  }
}

bool
InterpolatorInterface::applyNippOnline(
  MeshInterface& to,
  const std::vector<size_t>& bad_nodes, 
  const size_t n
)
{
  if (bad_node_policy == NIPP::RemoveFromMesh)
    return false;

  if (
    bad_node_policy == NIPP::InterpolateToZero &&
    std::binary_search(bad_nodes.cbegin(), bad_nodes.cend(), n)
  ) {
    for (size_t vi = 0; vi < to.D; ++vi)
      to.setValue(n, vi, 0);
    return true;
  }
  
  return false;
}
