#define BOOST_TEST_MODULE log
#include <boost/test/unit_test.hpp>
#include "inc/custom_test_macros.hpp"

#include <sstream>

#include "helpers/log.hpp"

BOOST_AUTO_TEST_CASE(log_simple)
{
  std::stringstream ss;
  LOG_SET_STREAM(ss)
  LOG_SET_LEVEL(INFO)
  LOG(ERROR) << "whatever";
  BOOST_CHECK_EQUAL(ss.str(), "ERROR: whatever\n");
}

BOOST_AUTO_TEST_CASE(log_no_newline)
{
  std::stringstream ss;
  LOG_SET_STREAM(ss)
  LOG_SET_LEVEL(INFO)
  LOG_DISABLE_NEWLINE()
  LOG(ERROR) << "whatever";
  LOG_ENABLE_NEWLINE()
  LOG(ERROR) << "whatever2";
  BOOST_CHECK_EQUAL(ss.str(), "ERROR: whateverERROR: whatever2\n");
}

BOOST_AUTO_TEST_CASE(log_above)
{
  std::stringstream ss;
  LOG_SET_STREAM(ss)
  LOG_SET_LEVEL(INFO)
  LOG(INFO) << "whateverinfo";
  LOG_SET_LEVEL(ERROR)
  LOG(DEBUG4) << "whatevedebug4";
  BOOST_CHECK_EQUAL(ss.str(), "INFO: whateverinfo\n");
}

BOOST_AUTO_TEST_CASE(iflog)
{
  int i = 1;
  LOG_SET_LEVEL(INFO)
  IFLOG(DEBUG) {
    i = 2;
  }
  BOOST_CHECK_EQUAL(i,1);
  IFLOG(INFO) {
    i = 2;
  }
  BOOST_CHECK_EQUAL(i,2);
}

BOOST_AUTO_TEST_CASE(iflog_nobraces)
{
  // Note this *is* evil. Please do not use no-braces notation.
  // It's enough that the macros don't use it!
  int i = 1;
  LOG_SET_LEVEL(INFO)
  IFLOG(DEBUG) i = 2;
  BOOST_CHECK_EQUAL(i,1);
  IFLOG(INFO) i = 2;
  BOOST_CHECK_EQUAL(i,2);
}
