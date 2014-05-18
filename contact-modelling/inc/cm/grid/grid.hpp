#ifndef GRID_HPP
#define GRID_HPP

#include <algorithm>
#include <memory>
#include <cstdint>
#include <vector>
#include <boost/any.hpp>

#include "cm/grid/cell.hpp"
#include "cm/grid/cell_shapes.hpp"

/**
 * \file
 * \brief   Definition of the Grid class
 */

namespace cm {

/**
 * \brief   Structure of 'cells' at which forces/pressure/displacements can be
 * determined.
 *
 * Naming
 * ------
 *
 * I'm not a person to be spending whole days fixing naming conventions and
 * stuff, but I did spend some quality time on this one.
 *
 * A gist of it is that this class is a collection of *nodes* which have some
 * area that they control (GridCellShape). The reason why I don't call the nodes
 * nodes is that it might be a little bit misleading. Before, this class was
 * named `MeshInterface` and it gave the impression that these were nodes of a
 * mesh. In fact, they are independent nodes, there is no tesselation of the
 * space, you can't travel from one node to it's neighbours, it's what it is, a
 * collection of ... cells.
 *
 * But why Grid? Well, Mesh was a no-go and other names like Tesselation were
 * not truthful to what it really is, either. The name *grid*, on the other
 * hand, stresses the independent nature of the nodes (in my opinion). Of
 * course, a *grid* can be thought of as a tesselation of Euclidean space, but
 * that's of secondary concern.
 *
 * Actual docs
 * -----------
 *
 *  Now that the naming is out of the way, what is a Grid, actually? As
 *  mentioned above, it is a collection of cells, all of which are of the same
 *  shape. It may, but doesn't have to be regular in any way.
 *
 *  This class also stores values (no specific meaning) in those cells,
 *  accessible through getValues(), getValue() and setValue(). It also stores
 *  metadata about each cell, accessible through getMetadata() and
 *  setMetadata(). Note however that the metadata is of primary use for the
 *  algorithms. I don't believe a user should try to access/set any of the
 *  metadata themselves.
 *
 *  This class follows the [Named Constructor Idiom][isocpp_named], so the
 *  regular constructor is not user-accessible; rather there are several
 *  "constructing" static methods, namely fromSensors(), fromEmpty() and
 *  fromFill().
 *
 *  There's no direct way for the user to construct the mesh explicitly, cell by
 *  cell (not until something like add_cell() is implemented). A workaround
 *  would be to create a cell source and use the fromSensors() constructing
 *  method.
 *
 *  [isocpp_named]: http://isocpp.org/wiki/faq/ctors#named-ctor-idiom
 */
class Grid {
public:
  /**
   * \name Methods implementing the Named Constructor Idiom.
   */
  /**\{*/

  /**
   * \brief   Construct grid from a collection of sensors.
   * \param   dim           dimensionality of values in the grid
   * \param   cell_shape    shape of the base element
   * \param   sensors_begin beginning of the collection of sensors
   * \param   sensors_end   end of the collection of sensors
   *
   * Items iterated over must have `.relative_position` attributes with
   * array-like extraction of x and y (`.relative_position[0] == x`,
   * `.relative_position[1] == y`)
   *
   * \note  This method can be used for constructing user-defined meshes, e.g.:
   *
   *     #include <array>
   *     #include <vector>
   *
   *     ...
   *
   *     struct MockCell { std::array<double, 2> relative_position; };
   *     std::vector<MockCell> mock_source;
   *     mock_source.push_back( {1.23, 4.56} ); // x == 1.23, y == 4.56
   * \par
   * \note  `std::distance` is used to get the number of sensors and thus
   * preallocate memory for the cells
   */
  template <class SensorIt>
  static Grid* fromSensors(
    const size_t dim,
    const GridCellShape& cell_shape,
    SensorIt sensors_begin,
    SensorIt sensors_end
  ) 
  {
    Grid* ret = new Grid(dim, cell_shape);

    const size_t num_sensors = std::distance(sensors_begin, sensors_end);
    ret->cells_.reserve(num_sensors);

    using sensor_type = decltype(*sensors_begin);
    std::for_each(sensors_begin, sensors_end, [&] (const sensor_type& sensor) {
        ret->cells_.push_back({sensor.relative_position[0], sensor.relative_position[1]});
    });

    ret->metadata_.resize(num_sensors);
    ret->values_.resize(dim * num_sensors);

    ret->generateMinMax();
    return ret;
  }

  /**
   * \brief   Construct an empty grid.
   * \param   dim         dimensionality of the values
   * \param   cell_shape  shape of each cell
   */
  static Grid* fromEmpty(
    const size_t dim,
    const GridCellShape& cell_shape
  );

  /**
   * \brief   Construct the grid from packing as many cell shapes into a
   * rectangle described by (x0,y0), (x1,y1) as possible.
   * \param   dim         dimensionality of the values
   * \param   cell_shape  shape of each cell
   * \param   x0,y0       first corner of the rectangle
   * \param   x1,y1       second (opposite) corner of the rectangle
   *
   * \note  So far this is only specialised for Rectangle/Square cell shapes. For
   * Circle, it will compile but throw a std::runtime_error.
   *
   * \todo  For Circle, one should implement the Lagrange packing (hexagonal
   * arrangement)
   */
  template <class CellShape>
  static Grid* fromFill(
    const size_t dim,
    const CellShape& cell_shape,
    const double x0,
    const double y0,
    const double x1,
    const double y1
  );

  /**
   * \brief   Construct the grid from packing cells into a rectangle which
   * \param   dim         dimensionality of the values
   * \param   cell_shape  shape of each cell
   * \param   other       the grid we want to cover
   *
   */
  template <class CellShape>
  static Grid* fromFill(
    const size_t dim,
    const CellShape& cell_shape,
    const Grid& other
  );

  /**\}*/

  typedef GridCell                                   cell_type;
  typedef std::vector<cell_type>                     cells_container;
  typedef typename cells_container::iterator         cells_iterator;
  typedef typename cells_container::const_iterator   cells_const_iterator;
  typedef double                                     value_type;
  typedef std::vector<value_type>                    values_container;
  typedef boost::any                                 metadata_type;
  typedef std::vector<boost::any>                    metadata_container;
  typedef std::vector<size_t>                        bad_cells_type;

  /**
   * \brief   Copy the cells over from clone_from grid.
   *
   * Removes all the cells (and values and metadata) from this grid and adds cells with the
   * coordinates of the cells from the clone_from grid.
   *
   * \note  The shape of the cells remains unaltered!
   * \par
   * \warning  All values/metadata are erased!
   */
  void clone_structure(const Grid& clone_from);

  /**
   * \brief   Get number of cells in the grid.
   */
  size_t num_cells() const;

  /**
   * \brief   Get non-const iterator to the begin of cells.
   */
  cells_iterator cells_begin();

  /**
   * \brief   Get const iterator to the begin of cells.
   */
  cells_const_iterator cells_cbegin() const;

  /**
   * \brief   Get non-const iterator to the end of cells.
   */
  cells_iterator cells_end();

  /**
   * \brief   Get const iterator to the end of cells.
   */
  cells_const_iterator cells_cend() const;

  /**
   * \brief   Get a reference to the cell at position i, non-const version.
   */
  cell_type& cell(const size_t i);

  /**
   * \brief   Get a reference to the cell at position i, const version.
   */
  const cell_type& cell(const size_t i) const;

  /**
   * \brief   Get bulk access to the values vector (const version)
   */
  const values_container& getRawValues() const;

  /**
   * \brief   Get bulk access to the values vector (non-const version)
   */
  values_container& getRawValues();

  /**
   * \brief   Return a local copy of values for cell i
   * \param   i   ID (number of the cell)
   *
   * \note    This is not very efficient, since we're making a local copy and returning it.
   *
   * In future, if needed, it might be implement something resembling Eigen's ::block()
   * functionality to allow the returned types to be used as lvalues.
   */
  std::vector<value_type> getValues(const size_t i) const;

  /**
   * \brief   Get vi-th value for i-th cell.
   * \param   i   ID (number) of the cell
   * \param   vi  which value from the cell to get
   */
  value_type getValue(const size_t i, const size_t vi) const;

  /**
   * \brief   Set vi-th value for i-th cell.
   * \param   i   ID (number) of the cell
   * \param   vi  which value from the cell to get
   * \param   v   value to be set
   */
  void setValue(const size_t i, const size_t vi, const value_type v);

  /**
   * \brief   Get a const reference to the metadata of a cell at position i.
   */
  const metadata_type& getMetadata(const size_t i) const;

  /**
   * \brief   Set the metadata of a cell at position i.
   */
  void setMetadata(const size_t i, const metadata_type& rhs);

  /**
   * \brief   Set the information on what points are bad.
   */
  void setBadCells(const bad_cells_type& bad_points);

  /**
   * \brief   Get a vector of bad cells (e.g. non-interpolable)
   *
   * So far the only use of this method has been for interpolators. I don't
   * think user should call this method.
   *
   * \todo  Restrict the usage to interpolators only through the
   * ["passkey" idiom][passkey]. Or, alternatively, do not store bad points in
   * the grid at all. Maybe in the interpolator? Maybe the interpolator will get
   * Algorithm-like interface and the bad cells will be stored in the
   * precomputed object?
   * [passkey]: http://stackoverflow.com/q/3220009/312650
   */
  const bad_cells_type& getBadCells() const;
  /**
   * \sa  getBadCells() const
   */
        bad_cells_type& getBadCells();

  /**
   * \brief   Remove cells (and their associated values and metadata) from the grid
   * \param   indices  A sorted vector of unique IDs of points to be erased
   *
   * \note Metadata and values for the cells to be deleted will be erased as
   * well.
   */
  void erase(const std::vector<size_t>& indices);

  /**
   * \brief   underlying shape for each of the grid's cells
   */
  const GridCellShape& getCellShape() const;

  /**
   * \brief   dimensionality of the values in the grid
   */
  size_t dim() const;

  /**
   * \name These methods define the bounding rectangle of the grid.
   *
   * \note The bounding rectangle is determined by also including the areas of the
   * cells, not simply taking min/max of the centres of the cells.
   */
  /**\{*/

  /**
   * \brief   minimum x extent of the grid.
   */
  double minX() const;
  /**
   * \brief   minimum y extent of the grid.
   */
  double minY() const;
  /**
   * \brief   maximum x extent of the grid.
   */
  double maxX() const;
  /**
   * \brief   maximum y extent of the grid.
   */
  double maxY() const;

  /**\}*/

  virtual ~Grid()              = default;

protected:
  /**
   * \brief   Generate min/max values for x/y
   *
   * \note  Takes into account the shape of the cell, not only the coordinates
   * of the centre of the cell
   */
  void generateMinMax();

  Grid& operator=(const Grid&)  = default;
  Grid(const Grid&)             = default;
  Grid& operator=(Grid&&)       = default;
  Grid(Grid&&)                  = default;

  /**
   * \brief   Construct empty grid
   * \param   dim         dimensionality of the values
   * \param   cell_shape  shape of each cell
   * \sa      fromEmpty()
   *
   * Theoretically, this could be moved to public, but to keep the interface
   * consistent, fromEmpty() was created in place.
   */
  Grid(const size_t dim, const GridCellShape& cell_shape);

private:
  /**
   * \brief   Dimensionality of the values
   */
  size_t dim_;

  /**
   * \brief   Precomputed value for lowest x coordinate the grid covers
   */
  double min_x_;
  /**
   * \brief   Precomputed value for lowest y coordinate the grid covers
   */
  double min_y_;
  /**
   * \brief   Precomputed value for highest x coordinate the grid covers
   */
  double max_x_;
  /**
   * \brief   Precomputed value for highest y coordinate the grid covers
   */
  double max_y_;

  /**
   * \brief   Values in the grid's cells. 
   */
  values_container  values_;

  /**
   * \brief   A vector of cells in the grid
   */
  cells_container  cells_;
  
  /**
   * \brief   A vector of metadata. 
   *
   * \note    We're using boost::any for sake of generality. At time of instantiation of the grid,
   *          we don't know what is the type of the metadata to be stored here. This field is
   *          intended to be primarily used by interpolators, to store data such as the
   *          barycentric coordinates and IDs of the corresponding cells constituting e.g. a
   *          Delaunay triangle in the source grid.
   * \par
   * \note    Data will be removed from this vector if a cell is removed from the grid. On the
   *          other hand, if this is a target grid and a cell is removed from a source grid and
   *          the IDs of cells from the source grid are stored inside metadata, this class has (in
   *          principle) no way of knowing this and will not update accordingly. That is to say
   *          that if you store IDs of cells from some other grid, this will not be updated if
   *          you remove cells from the other grid. I guess this behaviour is to be expected, but
   *          I felt it needed to be pointed out explicitly.
   */
  metadata_container  metadata_;

  /**
   * \brief   Vector of indices of bad cells. Used by interpolators.
   */
  bad_cells_type      bad_cells_;  

  /**
   * \brief   Shape of each of the grid's cells.
   */
  std::unique_ptr<GridCellShape> cell_shape_;
};

} /* namespace cm */

#endif /* GRID_HPP */
