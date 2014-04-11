#ifndef DELAUNAY_HPP
#define DELAUNAY_HPP

/**
 * This wraps around the actual triangulation library, to allow some modularity.
 */

#include <iterator>
#include <algorithm>
#include <cstring>
#include <tuple>
#include <vector>
#include <array>

#include "triangle.h"

#include "MeshNode.hpp"

class MeshInterface;

/**
 * \brief   Adapter for the delaunay triangulation.
 */
class Delaunay
{
  std::vector<MeshNode> nodes_;

  // technically, it'd be more sound to use size_t, but let's stick with the convention of
  // Triangle library
  typedef std::array<int,3>   triangle_type;
  std::vector<triangle_type>  triangles_;
  std::vector<double>         triangles_areas_;
  size_t                      no_triangles_;

public:
  /**
   * \brief   Default constructor, performs the actual triangulation.
   * \param   begin   iterator to the beginning of the points sequence.
   * \param   end     invalid iterator indicating the end of iteration.
   *
   * The constructor performs the actual triangulation (in this aspect, it is similar to CGAL's
   * interface.)
   */
  Delaunay(const MeshInterface& mesh);

  Delaunay& operator=(const Delaunay&) = default;
  Delaunay(const Delaunay&)            = default;
  Delaunay& operator=(Delaunay&&)      = default;
  Delaunay(Delaunay&&)                 = default;
  ~Delaunay()                          = default;

  size_t getNoTriangles() const;

  /// for meaning of fields, refer to PointInTriangleMetaIndex
  typedef std::tuple<bool, int, int, int, double, double, double> PointInTriangleMeta;
  /**
   * \brief   Easy-to-read indices for the PointInTriangleMeta tuple
   */
  enum PointInTriangleMetaIndex {
    FAIL,  /// whether the point is outside ALL triangles. true if the point is not inside the triangulation
    N0,    /// ID of 0th node of the triangle
    N1,    /// ID of 1st node of the triangle
    N2,    /// ID of 2nd node of the triangle
    KSI0,  /// barycentric coordinate w.r.t. 0th node
    KSI1,  /// barycentric coordinate w.r.t. 1st node
    KSI2   /// barycentric coordinate w.r.t. 2nd node
  };

  /**
   * \brief Check which triangle a point is inside of (if any).
   * \tparam MeshPoint  type for the query point. *NOTE* query points are from a different mesh!
   * \param p   query point 
   * \return  metadata on the relative position.
   *
   * If the point is not inside any of the triangles, true is returned as std::get<FAIL>(return).
   * Metadata on the relative position include:
   * - ids of the nodes of the triangle
   * - barycentric coordinates to each of the triangle's nodes, in the same order as the nodes of
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
  PointInTriangleMeta getTriangleInfoForPoint(const MeshNode& p);
};

#endif /* DELAUNAY_HPP */
