#include <stdexcept>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "cm/skin_provider/luca.hpp"

#include "custom_test_macros.hpp"
#include "luca_details_fixtures.hpp"

static const double eps = 1e-10;

BOOST_AUTO_TEST_SUITE(luca_details)

BOOST_FIXTURE_TEST_CASE(processReportTxt, ReportTxt)
{
  cm::details::ReportTxtData result;
  result.h = -1e200;
  result.E = -1e200;
  result.x_spacing = -1e200;
  result.y_spacing = -1e200;
  result.x_num = (size_t) -1;
  result.y_num = (size_t) -1;
  cm::details::processReportTxt(luca_path.native(), result);
  BOOST_CHECK_EQUAL(result.x_num, 40);
  BOOST_CHECK_EQUAL(result.y_num, 40);
  BOOST_CHECK_CLOSE(result.h, 0.002, eps);
  BOOST_CHECK_CLOSE(result.E, 177000, eps);
  BOOST_CHECK_CLOSE(result.x_spacing, 0.00165641, eps);
  BOOST_CHECK_CLOSE(result.y_spacing, 0.00143487, eps);
}

BOOST_FIXTURE_TEST_CASE(processIncompleteReportTxt, IncompleteReportTxt)
{
  auto always_true = [](const std::runtime_error& e) { return true; };
  cm::details::ReportTxtData result;
  using cm::details::processReportTxt;
  BOOST_CHECK_EXCEPTION(processReportTxt(luca_path.native(), result), std::runtime_error, always_true);
}

BOOST_AUTO_TEST_SUITE_END()
