#include "cm/details/delaunay.hpp"

#include "cm/grid/grid.hpp"
#include "cm/details/memory.hpp"
#include "cm/details/geometry.hpp"

#include "triangle.h"

namespace cm {
namespace details {

Delaunay::Delaunay(const Grid& grid)
{
  // copy cells over for our internal use
  cells_.assign(grid.cells_cbegin(), grid.cells_cend());

  size_t no_points = grid.num_cells();
  struct triangulateio in, out;
  // z - number stuff from 0, not from 1
  // B - no boundary markers in the output
  // c - output convex hull of the triangulation
  // n - output neighbouring triangles (will probably be nice for quickly finding which triangle
  //     a point lies in, if we can say that the hull is convex)
  // N - don't copy pointlist into output struct; we don't use jettisoning vertices so it
  //     doesn't change anything. We're also using just Delaunay (no Steiner points), so again,
  //     no need for it.
  // Q : quiet, no output on the command line
  const char*  triswitches_const = "zBcnNQ";
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
  std::for_each(grid.cells_cbegin(), grid.cells_cend(), [&](const GridCell& p) {
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
    throw std::runtime_error("Triangles have more than 3 cells! Verify if -o2 is not used!");
  }
  if (1 > out.numberoftriangles) {
    throw std::runtime_error("Need at least one triangle in the triangulation!");
  }

  const size_t no_triangles = out.numberoftriangles;
  int *n0, *n1, *n2;
  n0 = out.trianglelist;
  n1 = out.trianglelist + 1;
  n2 = out.trianglelist + 2;

  triangles_.reserve(no_triangles);
  for (size_t t = 0; t < no_triangles; ++t) {
    triangles_.push_back(triangle_type{*n0, *n1, *n2});
    n0 += 3; n1 += 3; n2 += 3;
  }
}

size_t Delaunay::getNumTriangles() const
{
  return triangles_.size(); 
}

Delaunay::PointInTriangleMeta Delaunay::getTriangleInfoForPoint(const GridCell& p)
{
  PointInTriangleMeta ret;
  std::get<FAIL>(ret)  = true;
  std::get<N0>(ret) = -1;
  std::get<N1>(ret) = -1;
  std::get<N2>(ret) = -1;
  std::get<KSI0>(ret)  = -1;
  std::get<KSI1>(ret)  = -1;
  std::get<KSI2>(ret)  = -1;

  int n0,n1,n2;
  double area_whole,area0,area1,area2;
  for (size_t i = 0; i < triangles_.size(); ++i) {
    n0 = triangles_[i][0];
    n1 = triangles_[i][1];
    n2 = triangles_[i][2];
    area_whole = area_triangle(cells_[n0], cells_[n1], cells_[n2]);
    area0 = area_triangle(p, cells_[n1], cells_[n2]);
    area1 = area_triangle(cells_[n0], p, cells_[n2]);
    area2 = area_triangle(cells_[n0], cells_[n1], p);

    // hack to verify if the problems with interpolation stem from roundoff error.
    // In fact, they do. Now I have to find a more robust solution, since this 10e-10 is UGLY.
    const double eps = 10e-10;
    const double sum_area = area0+area1+area2;
    if (sum_area > area_whole-eps && sum_area < area_whole + eps) {
      // the point is inside the triangle, on its edge or coincident with one of its vertices
      std::get<FAIL>(ret)  = false;
      std::get<N0>(ret) = n0;
      std::get<N1>(ret) = n1;
      std::get<N2>(ret) = n2;
      std::get<KSI0>(ret)  = area0 / area_whole;
      std::get<KSI1>(ret)  = area1 / area_whole;
      std::get<KSI2>(ret)  = area2 / area_whole;
      break;
    }
  }

  return ret;
}

} /* namespace details */
} /* namespace cm */

