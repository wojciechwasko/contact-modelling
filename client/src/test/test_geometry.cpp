#define BOOST_TEST_MODULE geometry
#include <boost/test/unit_test.hpp>

#include "helpers/geometry.hpp"

BOOST_AUTO_TEST_CASE(triangle_area_coordinates)
{
  using helpers::geometry::area_triangle;
  const double x0 = 16;
  const double y0 = 15;
  const double x1 = 23;
  const double y1 = 30;
  const double x2 = 50;
  const double y2 = 25;
  
  BOOST_CHECK_EQUAL(
    area_triangle(x0,y0,x1,y1,x2,y2),
    (double)220
  );
}

BOOST_AUTO_TEST_CASE(triangle_area_points)
{
  using helpers::geometry::area_triangle;
  struct P {
    double x;
    double y;
  };
  struct P p0{16, 15};
  struct P p1{23, 30};
  struct P p2{50, 25};
  
  BOOST_CHECK_EQUAL(
    area_triangle(p0,p1,p2),
    (double)220
  );
}
