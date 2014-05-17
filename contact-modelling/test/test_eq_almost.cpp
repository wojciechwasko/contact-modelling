#define BOOST_TEST_MODULE eq_almost
#include <boost/test/unit_test.hpp>
#include "cm/details/math.hpp"

using cm::details::eq_almost;

BOOST_AUTO_TEST_CASE(eq_almost_larger)
{
  BOOST_CHECK_EQUAL(
    false,
    eq_almost(0.4, 0.4+1e-2, 1e-5)
  );
};

BOOST_AUTO_TEST_CASE(eq_almost_larger_negtol)
{
  BOOST_CHECK_EQUAL(
    false,
    eq_almost(0.4, 0.4+1e-2, -1e-5)
  );
};

BOOST_AUTO_TEST_CASE(eq_almost_equal)
{
  BOOST_CHECK_EQUAL(
    false,
    eq_almost(0.4, 0.4+1e-2, 1e-2)
  );
};

BOOST_AUTO_TEST_CASE(eq_almost_equal_negtol)
{
  BOOST_CHECK_EQUAL(
    false,
    eq_almost(0.4, 0.4+1e-2, -1e-2)
  );
};

BOOST_AUTO_TEST_CASE(eq_almost_smaller)
{
  BOOST_CHECK_EQUAL(
    true,
    eq_almost(0.4, 0.4+1e-2, 1e-1)
  );
};

BOOST_AUTO_TEST_CASE(eq_almost_smaller_negtol)
{
  BOOST_CHECK_EQUAL(
    true,
    eq_almost(0.4, 0.4+1e-2, -1e-1)
  );
};
