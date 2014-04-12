#include "MeshRegularSquare.hpp"

#include <stdexcept>
#include <cmath>

#include "helpers/string.hpp"

using helpers::string::sb;

MeshRegularSquare::MeshRegularSquare(const MeshInterface& base_mesh, const double d)
  : MeshRegularSquare(
      base_mesh.D,
      base_mesh.minX(), 
      base_mesh.minY(),
      base_mesh.maxX(),
      base_mesh.maxY(),
      d
    )
{
}

MeshRegularSquare::MeshRegularSquare(const MeshInterface& base_mesh, const double d, const size_t dim)
  : MeshRegularSquare(
      dim,
      base_mesh.minX(), 
      base_mesh.minY(),
      base_mesh.maxX(),
      base_mesh.maxY(),
      d
    )
{
}

MeshRegularSquare::MeshRegularSquare(
  const size_t dim,
  const double x0,
  const double y0,
  const double x1,
  const double y1,
  const double d
)
  : MeshInterface(dim, calculate_no_nodes(x0, y0, x1, y1, d)), d_(d)
{
  const size_t nx       = calculate_no_nodes_1D(x0, x1, d);
  const double diff_x   = nx * d - (x1 - x0);
  const double x_origin = x0 - diff_x / 2.;

  const size_t ny       = calculate_no_nodes_1D(y0, y1, d);
  const double diff_y   = ny * d - (y1 - y0);
  const double y_origin = y0 - diff_y / 2.;

  size_t in = 0;
  double x = x_origin;
  for (size_t ix = 0; ix < nx; ++ix) {
    double y = y_origin;
    for (size_t iy = 0; iy < ny; ++iy) {
      this->node(in).x = x;
      this->node(in).y = y;
      y += d;
      ++in;
    }
    x += d;
  }
  generateMinMax();
}

double 
MeshRegularSquare::impl_node_area(size_t i) const
{
  return d_*d_;
}

size_t 
MeshRegularSquare::calculate_no_nodes(double x0, double y0, double x1, double y1, double d)
{
  if (!(x1 > x0)) {
    throw std::runtime_error(
      sb()  << "Error constructing regular square mesh, !(x1 > x0); "
            << "x1 == " << x1 << "x0 == " << x0
    );
  }
  if (!(y1 > y0)) {
    throw std::runtime_error(
      sb()  << "Error constructing regular square mesh, !(y1 > y0); "
            << "y1 == " << y1 << "y0 == " << y0
    );
  }

  return calculate_no_nodes_1D(x0, x1, d) * calculate_no_nodes_1D(y0, y1, d);
}

size_t 
MeshRegularSquare::calculate_no_nodes_1D(double t0, double t1, double d)
{
  if (!(t1 > t0)) {
    throw std::runtime_error(
      sb()  << "Error constructing regular square mesh, !(t1 > t0); "
            << "t1 == " << t1 << "t0 == " << t0
    );
  }
  if (!(d > 0)) {
    throw std::runtime_error(
      sb()  << "Error constructing regular square mesh, !(d > 0); "
            << "d == " << d
    );
  }
  return std::ceil((t1-t0)/d);
}
