#include <boost/test/unit_test.hpp>

#include "cm/grid/cell_shapes.hpp"

BOOST_AUTO_TEST_SUITE(cell_shapes)

BOOST_AUTO_TEST_CASE(square)
{
  cm::Square s(1.25);
  BOOST_CHECK_CLOSE(s.dx() , 1.25              , 1e-5);
  BOOST_CHECK_CLOSE(s.dy() , 1.25              , 1e-5);
  BOOST_CHECK_CLOSE(s.r()  , 0.705236979434695 , 1e-5);
  BOOST_CHECK_EQUAL(s.isSquare(), true);
  BOOST_CHECK_EQUAL(s.isRectangular(), true);
  BOOST_CHECK_EQUAL(s.isCircular(), false);
};

BOOST_AUTO_TEST_CASE(rectangle)
{
  cm::Rectangle s(1.15, 1.25);
  BOOST_CHECK_CLOSE(s.dx() , 1.15              , 1e-5);
  BOOST_CHECK_CLOSE(s.dy() , 1.25              , 1e-5);
  BOOST_CHECK_CLOSE(s.r()  , 0.676439547475751 , 1e-5);
  BOOST_CHECK_EQUAL(s.isSquare(), false);
  BOOST_CHECK_EQUAL(s.isRectangular(), true);
  BOOST_CHECK_EQUAL(s.isCircular(), false);
};

BOOST_AUTO_TEST_CASE(circle)
{
  cm::Circle s(1.25);
  BOOST_CHECK_CLOSE(s.dx() , 2.2155673136319 , 1e-5);
  BOOST_CHECK_CLOSE(s.dy() , 2.2155673136319 , 1e-5);
  BOOST_CHECK_CLOSE(s.r()  , 1.25            , 1e-5);
  BOOST_CHECK_EQUAL(s.isSquare(), false);
  BOOST_CHECK_EQUAL(s.isRectangular(), false);
  BOOST_CHECK_EQUAL(s.isCircular(), true);
};

BOOST_AUTO_TEST_SUITE_END()

