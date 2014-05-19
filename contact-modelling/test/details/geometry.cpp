#include <boost/test/unit_test.hpp>

#include "cm/details/geometry.hpp"

BOOST_AUTO_TEST_SUITE(details__geometry)

BOOST_AUTO_TEST_CASE(triangle_area_coordinates)
{
  using cm::details::area_triangle;
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
  using cm::details::area_triangle;
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

BOOST_AUTO_TEST_CASE(place_segments_exact)
{
  using cm::details::place_segments;
  const double t0 = 1;
  const double t1 = 4;
  const double dt = 1;
  const auto pl = place_segments(t0,t1,dt);

  BOOST_CHECK_EQUAL(
    3,
    pl.num
  );

  BOOST_CHECK_CLOSE(
    1,
    pl.origin,
    1e-6
  );

  BOOST_CHECK_CLOSE(
    1.5,
    pl.seg0middle,
    1e-6
  );
}

BOOST_AUTO_TEST_CASE(place_segments_not_exact)
{
  using cm::details::place_segments;
  const double t0 = 1;
  const double t1 = 3.5;
  const double dt = 1;
  const auto pl = place_segments(t0,t1,dt);

  BOOST_CHECK_EQUAL(
    2,
    pl.num
  );

  BOOST_CHECK_CLOSE(
    1.25,
    pl.origin,
    1e-6
  );

  BOOST_CHECK_CLOSE(
    1.75,
    pl.seg0middle,
    1e-6
  );
}

BOOST_AUTO_TEST_SUITE_END()
