#define BOOST_TEST_MODULE all_tests
#include <boost/test/unit_test.hpp>

#include "cm/log/log.hpp"

/**
 * Turns off logging
 */
struct F {
  F()
  {
    LOG_SET_STREAM(cm::log::onull)
  }

  ~F()
  {

  }
};


BOOST_GLOBAL_FIXTURE(F);
