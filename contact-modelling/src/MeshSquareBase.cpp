#include "cm/mesh/square_base.hpp"

namespace cm {

  MeshSquareBase::MeshSquareBase(const size_t dim, const double d)
  : MeshRectangularBase(dim, d, d)
{}

MeshSquareBase::MeshSquareBase(const MeshInterface& base_mesh, const double d)
  : MeshSquareBase(base_mesh, d, base_mesh.D)
{}

MeshSquareBase::MeshSquareBase(const MeshInterface& base_mesh, const double d, const size_t dim)
  : MeshRectangularBase(base_mesh, d, d, dim)
{}

MeshSquareBase::MeshSquareBase(
  const size_t dim,
  const double x0,
  const double y0,
  const double x1,
  const double y1,
  const double d
)
  : MeshRectangularBase(dim, x0, y0, x1, y1, d, d)
{}

} /* namespace cm */
