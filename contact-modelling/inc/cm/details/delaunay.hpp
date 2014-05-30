#ifndef DELAUNAY_HPP
#define DELAUNAY_HPP

/**
 * \cond DEV
 */

/**
 * \file
 * \brief   Adapter for the triangle library.
 */

#include <iterator>
#include <algorithm>
#include <cstring>
#include <tuple>
#include <vector>
#include <array>

#include "cm/grid/cell.hpp"

namespace cm {
class Grid;

namespace details {

/**
 * \brief   Adapter for the delaunay triangulation.
 *
 * Wraps around the [Shewchuk's triangle library][triangle] and provides a nice,
 * C++-oriented interface.
 *
 * [triangle]: http://www.cs.cmu.edu/~quake/triangle.html
 */
class Delaunay
{
  /**
   * \brief   Copy of the cells from the grid the triangulation is performed
   * upon.
   */
  std::vector<GridCell> cells_;
  /**
   * \brief   Type used for single triangle -- indices of three nodes.
   * \note    Technically, it'd be more sound to use size_t, but let's stick with the convention of
   * the Triangle library, which uses ints for that purpose.
   */
  typedef std::array<int,3>   triangle_type;
  /**
   * \brief   All triangles created through Delaunay triangulation.
   */
  std::vector<triangle_type>  triangles_;

public:
  /**
   * \brief   Default constructor, performs the actual triangulation.
   * \param   grid  The grid over which to perform the triangulation.
   *
   * The constructor performs the actual triangulation (in this aspect, it is similar to CGAL's
   * interface.)
   */
  Delaunay(const Grid& grid);

  Delaunay& operator=(const Delaunay&) = default;
  Delaunay(const Delaunay&)            = default;
  Delaunay& operator=(Delaunay&&)      = default;
  Delaunay(Delaunay&&)                 = default;
  ~Delaunay()                          = default;

  /**
   * \brief   How many triangles are there in the triangulation?
   */
  size_t getNumTriangles() const;

  /// for meaning of fields, refer to PointInTriangleMetaIndex
  /**
   * \brief   Metadata type for a point.
   *
   * \todo  A tuple with 7 elements? It's a code smell, that's for sure. Rather,
   * create a custom return type and return it (like I've written on my
   * [blog][blogpost]).
   * [blogpost]: http://blog.merlotatnight.com/2014/05/c-do-not-fear-custom-return-types-for.html
   *
   *
   */
  typedef std::tuple<bool, int, int, int, double, double, double> PointInTriangleMeta;

  /**
   * \brief   Easy-to-read indices for the PointInTriangleMeta tuple
   *
   * Will be deleted once PointInTriangleMeta becomes a regular struct.
   */
  enum PointInTriangleMetaIndex {
    /// whether the point is outside ALL triangles. true if the point is not inside the triangulation
    FAIL,  
    /// ID of 0th cell of the triangle
    N0,    
    /// ID of 1st cell of the triangle
    N1,    
    /// ID of 2nd cell of the triangle
    N2,    
    /// barycentric coordinate w.r.t. 0th cell
    KSI0,  
    /// barycentric coordinate w.r.t. 1st cell
    KSI1,  
    /// barycentric coordinate w.r.t. 2nd cell
    KSI2   
  };

  /**
   * \brief Check which triangle a point is inside of (if any).
   * \param p   query point 
   * \return  metadata on the relative position.
   *
   * If the point is not inside any of the triangles, true is returned as std::get<FAIL>(return).
   * Metadata on the relative position include:
   * - ids of the cells of the triangle
   * - barycentric coordinates to each of the triangle's cells, in the same order as the cells of
   *   the triangle
   *
   * If the point lies on the edge of the triangulation or coincides with a vertex of the
   * triangulation, only one triangle will be reported; the triangle will be chosen
   * deterministically from call to call;
   *
   * Complexity: Worst-case O(n) where n is the number of triangles. We'll traverse the triangles
   * one-by-one, with no shortcuts. There are sneaky ways to speed things up, but they rely on the
   * triangulation to be convex and such. I figured since this is not an online step, there's not
   * much point in spending too much time optimising this method. Obviously, it'd be more than
   * welcome, but there are priorities.
   */
  PointInTriangleMeta getTriangleInfoForPoint(const GridCell& p);
};

} /* namespace details */
} /* namespace cm */

/**
 * \endcond
 */

#endif /* DELAUNAY_HPP */
