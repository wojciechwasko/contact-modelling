#include "MeshRegularSquare.hpp"

MeshRegularSquare::MeshRegularSquare(const MeshInterface& base_mesh, const double d)
  : MeshRegularSquare(base_mesh, d, base_mesh.D)
{}

MeshRegularSquare::MeshRegularSquare(const MeshInterface& base_mesh, const double d, const size_t dim)
  : MeshRegularRectangular(base_mesh, d, d, dim)
{}

MeshRegularSquare::MeshRegularSquare(
  const size_t dim,
  const double x0,
  const double y0,
  const double x1,
  const double y1,
  const double d
)
  : MeshRegularRectangular(dim, x0, y0, x1, y1, d, d)
{}
