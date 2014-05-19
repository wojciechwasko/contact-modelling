#include "cm/grid/cell_shapes.hpp"
#include <cmath>

#include "cm/log/log.hpp"

namespace cm {

GridCellShape::~GridCellShape()
{}

Rectangle::Rectangle(const double dx, const double dy)
  : dx_(dx), dy_(dy), r_approx_(sqrt(dx*dy/M_PI))
{}

double Rectangle::dx() const { return dx_; }
double Rectangle::dy() const { return dy_; }

double Rectangle::r() const
{
  LOG(WARN) << "Rectangle base shape for a grid: returning approximation of radius r";
  return r_approx_;
}

double Rectangle::area() const { return dx_ * dy_; }

GridCellShape* Rectangle::clone() const { return new Rectangle(dx_,dy_); }

Square::Square(const double d)
  : Rectangle(d,d)
{}

GridCellShape* Square::clone() const { return new Square(this->dx()); }

Circle::Circle(const double r)
  : dx_approx_(r * sqrt(M_PI)), dy_approx_(r * sqrt(M_PI)), r_(r)
{}

double Circle::dx() const
{
  LOG(WARN) << "Circle base shape for a grid: returning approximation of dx";
  return dx_approx_;
}

double Circle::dy() const
{
  LOG(WARN) << "Circle base shape for a grid: returning approximation of dy";
  return dy_approx_;
}

double Circle::r() const { return r_; }

double Circle::area() const { return M_PI * r_ * r_; }

GridCellShape* Circle::clone() const { return new Circle(r_); }

} /* namespace cm */
