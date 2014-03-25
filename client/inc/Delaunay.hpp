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

#include "helpers/memory.hpp"
#include "helpers/geometry.hpp"

#include "triangle.h"

/**
 * \brief   Adapter for the delaunay triangulation.
 * \tparam  point_iterator_type   iterators of this type will be used for accessing the points.
 *
 * Four things are assumed about the iterators:
 * 1. they employ operator==
 * 2. they are deterministic, iterating by ++-ing the begin iterator till ==end will result in the
 * same sequence every time.
 * 3. they are unique for a given sequence
 *
 * For perfomance, it would be quite nice to have them be random-access, since we're using
 * std::distance(begin, iterator) as the hash for our internal representations and using non-random
 * access iterators will result in a loooot of unnecessary iterations over the container. Anyway,
 * the alternative would be to use the address of the element the iterator refers to as the hash but
 * that's not robust w.r.t. relocations. bugzzz.
 *
 * TODO maybe issue a warning (compile-time?) about non-random access iterators being used, by using
 * typeid(iterator_traits::iterator_category) != typeid(std::random_access_iterator_tag)
 *
 * About the points, their type (decltype(*begin), where begin is of point_iterator_type type), must
 * merely have .x and .y members. (Will probably think of supporting .x() .y() callable members
 * later on) 
 */
template <class point_iterator_type>
class Delaunay
{
    typedef typename std::iterator_traits<point_iterator_type>::value_type point_type;
    std::vector<point_type> nodes_;
    // technically, it'd be more sound to use size_t, but let's stick with the convention of
    // Triangle library
    typedef std::array<int,3> triangle_type;
    std::vector<triangle_type> triangles_;
    std::vector<double> triangles_areas_;
    size_t no_triangles_;

  public:
    /**
     * \brief   Default constructor, performs the actual triangulation.
     * \param   begin   iterator to the beginning of the points sequence.
     * \param   end     invalid iterator indicating the end of iteration.
     *
     * The constructor performs the actual triangulation (in this aspect, it is similar to CGAL's
     * interface.)
     */
    Delaunay(point_iterator_type begin, point_iterator_type end)
    {
      using helpers::memory::delguard;
      using helpers::geometry::area_triangle;

      // copy nodes over for our internal use
      nodes_.insert(nodes_.begin(), begin, end);

      size_t no_points = std::distance(begin, end);
      struct triangulateio in, out;
      // z - number stuff from 0, not from 1
      // B - no boundary markers in the output
      // c - output convex hull of the triangulation
      // n - output neighbouring triangles (will probably be nice for quickly finding which triangle
      //     a point lies in, if we can say that the hull is convex)
      // N - don't copy pointlist into output struct; we don't use jettisoning vertices so it
      //     doesn't change anything. We're also using just Delaunay (no Steiner points), so again,
      //     no need for it.
      // FUTURE:
      // Q : quiet, no output on the command line
      const char*  triswitches_const = "zBcnN";
      char* triswitches = new char[strlen(triswitches_const)+1]; // +1 for \0
      strncpy(triswitches, triswitches_const, strlen(triswitches_const)+1);

      // initialize input
      in.numberofpoints          = no_points;
      in.pointlist               = new double[in.numberofpoints * 2];
        delguard<double,double[]> d_inpointlist(in.pointlist);
      in.numberofpointattributes = 0;
      in.pointattributelist      = (double *) NULL;
      in.pointmarkerlist         = (int *) NULL;
      in.numberofsegments        = 0;
      in.segmentlist             = (int *) NULL;
      in.numberofholes           = 0;
      in.holelist                = (double *) NULL;
      in.numberofregions         = 0;
      in.regionlist              = (double *) NULL;
      // fill points
      auto point_from_array = in.pointlist;
      std::for_each(begin, end, [&](const point_type& p) {
        *point_from_array = p.x;
        ++point_from_array;
        *point_from_array = p.y;
        ++point_from_array;
      });

      // initialize output
      out.trianglelist = (int *) NULL; // because -E NOT used
        delguard<int,int[]> d_outtrianglelist(out.trianglelist);
      out.neighborlist = (int *) NULL; // because -n used
        delguard<int,int[]> d_outneighborlist(out.neighborlist);
      out.segmentlist  = (int *) NULL; // because -c used
        delguard<int,int[]> d_outsegmentlist(out.segmentlist);

      // triangulate, duh
      triangulate(triswitches, &in, &out, NULL);

      if (3 != out.numberofcorners) {
        throw std::runtime_error("Triangles have more than 3 nodes! Verify if -o2 is not used!");
      }
      if (1 > out.numberoftriangles) {
        throw std::runtime_error("Need at least one triangle in the triangulation!");
      }

      no_triangles_ = out.numberoftriangles;
      int *n0, *n1, *n2;
      n0 = out.trianglelist;
      n1 = out.trianglelist + 1;
      n2 = out.trianglelist + 2;

      triangles_.reserve(no_triangles_);
      triangles_areas_.reserve(no_triangles_);
      for (size_t t = 0; t < no_triangles_; ++t) {
        triangles_.push_back(triangle_type{*n0, *n1, *n2});
        triangles_areas_.push_back(
          area_triangle(nodes_[*n0], nodes_[*n1], nodes_[*n2])
        );
        n0 += 3; n1 += 3; n2 += 3;
      }

    }

    /// for meaning of fields, refer to PointInTriangleMetaIndex
    typedef std::tuple<bool, int, int, int, double, double, double> PointInTriangleMeta;
    /**
     * \brief   Easy-to-read indices for the PointInTriangleMeta tuple
     */
    enum PointInTriangleMetaIndex {
      FAIL,  /// whether the point is outside ALL triangles. true if the point is not inside the triangulation
      N0_ID, /// ID of 0th node of the triangle
      N1_ID, /// ID of 1st node of the triangle
      N2_ID, /// ID of 2nd node of the triangle
      KSI0,  /// barycentric coordinate w.r.t. 0th node
      KSI1,  /// barycentric coordinate w.r.t. 1st node
      KSI2   /// barycentric coordinate w.r.t. 2nd node
    };

    /**
     * \brief Check which triangle a point is inside of (if any).
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
     * much point in spending too much time optimising this method.
     */
    PointInTriangleMeta get_triangle_for_point(const point_type& p)
    {
      using helpers::geometry::area_triangle;
      PointInTriangleMeta ret;
      std::get<FAIL>(ret)  = true;
      std::get<N0_ID>(ret) = -1;
      std::get<N1_ID>(ret) = -1;
      std::get<N2_ID>(ret) = -1;
      std::get<KSI0>(ret)  = -1;
      std::get<KSI1>(ret)  = -1;
      std::get<KSI2>(ret)  = -1;

      int n0,n1,n2;
      double area_whole,ksi0,ksi1,ksi2;
      for (size_t i = 0; i < triangles_.size(); ++i) {
        n0 = triangles_[i][0];
        n1 = triangles_[i][1];
        n2 = triangles_[i][2];
        area_whole = triangles_areas_[i];
        ksi0 = area_triangle(p, nodes_[n1], nodes_[n2]) / area_whole;
        ksi1 = area_triangle(nodes_[n0], p, nodes_[n2]) / area_whole;
        ksi2 = area_triangle(nodes_[n0], nodes_[n1], p) / area_whole;

        if (ksi0 >= 0 && ksi1 >= 0 && ksi2 >= 0) {
          // the point is inside the triangle, on its edge or coincident with one of its vertices
          std::get<FAIL>(ret)  = false;
          std::get<N0_ID>(ret) = n0;
          std::get<N1_ID>(ret) = n1;
          std::get<N2_ID>(ret) = n2;
          std::get<KSI0>(ret)  = ksi0;
          std::get<KSI1>(ret)  = ksi1;
          std::get<KSI2>(ret)  = ksi2;
          break;
        }
      }

      return ret;
    }
  private:

};

#endif /* DELAUNAY_HPP */
