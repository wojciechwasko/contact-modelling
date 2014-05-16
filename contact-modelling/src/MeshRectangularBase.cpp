#include "cm/mesh/rectangular_base.hpp"

#include <stdexcept>
#include <cmath>

#include "cm/details/string.hpp"

namespace cm {

using cm::details::sb;

MeshRectangularBase::MeshRectangularBase(const size_t dim, const double dx, const double dy)
  : MeshInterface(dim, 0), dx_(dx), dy_(dy)
{
}

MeshRectangularBase::MeshRectangularBase(const MeshInterface& base_mesh, const double dx, const double dy)
  : MeshRectangularBase(
      base_mesh,
      dx,
      dy,
      base_mesh.D
    )
{
}

MeshRectangularBase::MeshRectangularBase(const MeshInterface& base_mesh, const double dx, const double dy, const size_t dim)
  : MeshRectangularBase(
      dim,
      base_mesh.minX(), 
      base_mesh.minY(),
      base_mesh.maxX(),
      base_mesh.maxY(),
      dx,
      dy
    )
{
}

MeshRectangularBase::MeshRectangularBase(
  const size_t dim,
  const double x0,
  const double y0,
  const double x1,
  const double y1,
  const double dx,
  const double dy
)
  : MeshInterface(dim, calculate_no_nodes(x0, y0, x1, y1, dx, dy)), dx_(dx), dy_(dy)
{
  const size_t nx       = calculate_no_nodes_1D(x0, x1, dx);
  const double diff_x   = nx * dx - (x1 - x0);
  const double x_origin = x0;

  const size_t ny       = calculate_no_nodes_1D(y0, y1, dy);
  const double y_origin = y0;

  size_t in = 0;
  double x = x_origin;
  for (size_t ix = 0; ix < nx; ++ix) {
    double y = y_origin;
    for (size_t iy = 0; iy < ny; ++iy) {
      this->node(in).x = x;
      this->node(in).y = y;
      y += dy;
      ++in;
    }
    x += dx;
  }
  generateMinMax();
}

double 
MeshRectangularBase::impl_node_area(size_t i) const
{
  return dx_*dy_;
}

size_t 
MeshRectangularBase::calculate_no_nodes(double x0, double y0, double x1, double y1, double dx, double dy)
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

  return calculate_no_nodes_1D(x0, x1, dx) * calculate_no_nodes_1D(y0, y1, dy);
}

size_t 
MeshRectangularBase::calculate_no_nodes_1D(double t0, double t1, double d)
{
  if (!(t1 > t0)) {
    throw std::runtime_error(
      sb()  << "Error constructing regular rectangular mesh, !(t1 > t0); "
            << "t1 == " << t1 << "t0 == " << t0
    );
  }
  if (!(d > 0)) {
    throw std::runtime_error(
      sb()  << "Error constructing regular rectangular mesh, !(d > 0); "
            << "d == " << d
    );
  }
  return std::ceil((t1-t0)/d);
}

double 
MeshRectangularBase::impl_dx() const
{
  return dx_;
}

double 
MeshRectangularBase::impl_dy() const
{
  return dy_;
}

} /* namespace cm */
