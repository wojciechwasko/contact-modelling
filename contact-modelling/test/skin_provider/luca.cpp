#include <stdexcept>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "cm/skin_provider/luca.hpp"

#include "custom_test_macros.hpp"
#include "luca_fixtures.hpp"

namespace bfs = boost::filesystem;
auto always_true = [](const std::runtime_error& e) {
//  std::cout << e.what() << std::endl;
  return true; 
};

BOOST_AUTO_TEST_SUITE(bad_luca_provider)

BOOST_FIXTURE_TEST_CASE(bad_luca_empty_dir, BadLucaEmptyDir)
{
  BOOST_CHECK_EXCEPTION(cm::SkinProviderLuca(luca_path.native()), std::runtime_error, always_true);
}

BOOST_AUTO_TEST_CASE(bad_luca_nonexistent_dir)
{
  bfs::path nonexistent = bfs::unique_path("luca_empty-%%%%-%%%%-%%%%-%%%%");
  BOOST_CHECK_EXCEPTION(cm::SkinProviderLuca(nonexistent.native()), std::runtime_error, always_true);
}

BOOST_FIXTURE_TEST_CASE(bad_luca_only_report_txt, BadLucaOnlyReportTxt)
{
  BOOST_CHECK_EXCEPTION(cm::SkinProviderLuca(luca_path.native()), std::runtime_error, always_true);
}

BOOST_FIXTURE_TEST_CASE(bad_luca_some_missing, BadLucaSomeMissing)
{
  BOOST_CHECK_EXCEPTION(cm::SkinProviderLuca(luca_path.native()), std::runtime_error, always_true);
}

BOOST_FIXTURE_TEST_CASE(good_luca, GoodLuca)
{
  BOOST_CHECK_NO_THROW(cm::SkinProviderLuca(luca_path.native()));
}

BOOST_AUTO_TEST_SUITE_END()
