#define BOOST_TEST_MODULE log
#include <boost/test/unit_test.hpp>
#include "inc/custom_test_macros.hpp"

#include <sstream>

#define MAX_LOGGING_LEVEL INFO
#include "cm/log/log.hpp"

BOOST_AUTO_TEST_CASE(log_above_plateau)
{
  std::stringstream ss;
  LOG_SET_STREAM(ss)
  LOG(DEBUG) << "whatever";
  BOOST_CHECK_EQUAL(ss.str(), "");
}

BOOST_AUTO_TEST_CASE(iflog_above_plateau)
{
  int i = 1;
  IFLOG(DEBUG) {
    i = 2;
  }
  BOOST_CHECK_EQUAL(i,1);
  IFLOG(INFO) {
    i = 2;
  }
  BOOST_CHECK_EQUAL(i,2);
}
