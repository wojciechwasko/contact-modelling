#include "cm/grid/grid.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <tuple>

#include "cm/details/geometry.hpp"
#include "cm/details/container_algorithms.hpp"
#include "cm/log/log.hpp"

namespace cm {

Grid::Grid(const size_t dim, const GridCellShape& cell_shape)
  : dim_(dim), cell_shape_(cell_shape.clone())
{}

Grid* Grid::fromEmpty(
  const size_t dim,
  const GridCellShape& cell_shape
)
{
  return new Grid(dim, cell_shape);
}

// hide template specialisations from doxygen

/**
 * \cond
 */
template <class CellShape>
Grid* Grid::fromFill(
  const size_t dim,
  const CellShape& cell_shape,
  const Grid& other
)
{
  return Grid::fromFill(dim, cell_shape, other.minX(), other.minY(), other.maxX(), other.maxY());
}

// specialisations for fromFill (other Grid version)
template Grid* Grid::fromFill(const size_t, const Circle&,    const Grid&);
template Grid* Grid::fromFill(const size_t, const Square&,    const Grid&);
template Grid* Grid::fromFill(const size_t, const Rectangle&, const Grid&);

template <>
Grid* Grid::fromFill(
  const size_t dim,
  const Circle& cell_shape,
  const double x0,
  const double y0,
  const double x1,
  const double y1
)
{
  // fill with circles
  throw std::runtime_error("Filling with circles is not implemented (yet). TODO: hexagonal packing");
}

template <>
Grid* Grid::fromFill(
  const size_t dim,
  const Rectangle& cell_shape,
  const double x0,
  const double y0,
  const double x1,
  const double y1
)
{
  Grid* ret = new Grid(dim, cell_shape);

  // fill with rectangles
  const double dx = cell_shape.dx();
  const double dy = cell_shape.dy();

  const auto segm_x = details::place_segments(x0, x1, dx);
  const auto segm_y = details::place_segments(y0, y1, dy);

  const size_t num_cells = segm_x.num * segm_y.num;

  ret->cells_.reserve(num_cells);
  double x = segm_x.seg0middle;
  for (size_t ind_x = 0; ind_x < segm_x.num; ++ind_x, x += dx) {
    double y = segm_y.seg0middle;
    for (size_t ind_y = 0; ind_y < segm_y.num; ++ind_y, y += dy) {
      ret->cells_.push_back({x,y});
    }
  }
  ret->metadata_.resize(num_cells);
  ret->values_.resize(ret->dim_ * num_cells);

  ret->generateMinMax();
  return ret;
}

template <>
Grid* Grid::fromFill(
  const size_t dim,
  const Square& cell_shape,
  const double x0,
  const double y0,
  const double x1,
  const double y1
)
{
  // this is none different from filling with rectangles:
  return Grid::fromFill(dim, (const Rectangle&) cell_shape, x0, y0, x1, y1);
}

/**
 * \endcond
 */

void Grid::clone_structure(const Grid& clone_from)
{
  // remove existing cells
  cells_.clear(); 
  metadata_.clear();
  values_.clear();

  cells_.reserve(clone_from.num_cells());
  
  std::for_each(clone_from.cells_cbegin(), clone_from.cells_cend(), [&] (const GridCell& n) {
    cells_.push_back(n);
  });

  metadata_.resize(clone_from.num_cells());
  values_.resize(dim_ * clone_from.num_cells());

  generateMinMax();
}

size_t Grid::num_cells() const
{
  return cells_.size(); 
}

Grid::cells_iterator Grid::cells_begin() 
{ 
  return cells_.begin(); 
}

Grid::cells_const_iterator Grid::cells_cbegin() const
{
  return cells_.cbegin();
}

Grid::cells_iterator Grid::cells_end()
{
  return cells_.end();
}

Grid::cells_const_iterator Grid::cells_cend() const
{
  return cells_.cend();
}

const Grid::cell_type& Grid::cell(size_t i) const
{
  return cells_.at(i);
}

Grid::cell_type& Grid::cell(size_t i)
{
  return cells_.at(i);
}

const Grid::values_container& Grid::getRawValues() const
{
  return values_;
}

Grid::values_container& Grid::getRawValues() 
{
  return values_;
}

std::vector<Grid::value_type> Grid::getValues(size_t i) const
{
  std::vector<value_type> ret;
  ret.resize(dim_);
  for (size_t it = 0; it < dim_; ++it)
    ret.at(it) = values_.at(i*dim_ + it);
  return ret;
}

Grid::value_type Grid::getValue(size_t i, size_t vi) const
{
  return values_.at(i*dim_ + vi);
}

void Grid::setValue(size_t i, size_t vi, value_type v)
{
  values_.at(i*dim_ + vi) = v;
}

const Grid::metadata_type& Grid::getMetadata(const size_t i) const
{
  return metadata_.at(i);
}

void Grid::setMetadata(const size_t i, const metadata_type& rhs)
{
  metadata_.at(i) = rhs;
}

void Grid::setBadCells(const Grid::bad_cells_type& rhs)
{
  bad_cells_.assign(rhs.cbegin(), rhs.cend());
}

const Grid::bad_cells_type& Grid::getBadCells() const
{
  return bad_cells_;
}

Grid::bad_cells_type& Grid::getBadCells()
{
  return bad_cells_;
}

void Grid::erase(const std::vector<size_t>& indices)
{
  if (indices.size() == 0)
    return;
  using details::erase_by_indices;
  erase_by_indices(cells_,      indices);
  erase_by_indices(metadata_,   indices);
  erase_by_indices(values_,     indices, dim_);
  generateMinMax();
}

void Grid::generateMinMax()
{
  const double extra_x = cell_shape_->isCircular() ? cell_shape_->r() : 0.5*cell_shape_->dx();
  const double extra_y = cell_shape_->isCircular() ? cell_shape_->r() : 0.5*cell_shape_->dy();

  double min_x = std::numeric_limits<double>::max();
  double min_y = std::numeric_limits<double>::max();
  double max_x = std::numeric_limits<double>::lowest();
  double max_y = std::numeric_limits<double>::lowest();

  std::for_each(this->cells_cbegin(), this->cells_cend(), [&](const cell_type& n) {
    if (n.x - extra_x < min_x) min_x = n.x - extra_x;
    if (n.x + extra_x > max_x) max_x = n.x + extra_x;
    if (n.y - extra_y < min_y) min_y = n.y - extra_y;
    if (n.y + extra_y > max_y) max_y = n.y + extra_y;
  });
  // save min/max to member variables
  min_x_ = min_x;
  min_y_ = min_y;
  max_x_ = max_x;
  max_y_ = max_y;
}

double Grid::minX() const
{
  return min_x_;
}

double Grid::minY() const
{
  return min_y_;
}

double Grid::maxX() const
{
  return max_x_;
}

double Grid::maxY() const
{
  return max_y_;
}

const GridCellShape& Grid::getCellShape() const
{
  if (!cell_shape_)
    throw std::runtime_error("I don't know how that's possible but I have nullptr for cell shape!");
  return *cell_shape_;
}

size_t Grid::dim() const
{
  return dim_;
}

} /* namespace cm */
