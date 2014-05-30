#include <stdexcept>
#include <memory>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "cm/grid/grid.hpp"
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
  CHECK_STD_NO_THROW(cm::SkinProviderLuca(luca_path.native()));

  // construct it again
  auto provider = cm::SkinProviderLuca(luca_path.native());
  std::unique_ptr<cm::Grid> grid(provider.createGrid()); 
  provider.update(grid->getRawValues());

  BOOST_CHECK_EQUAL(100, grid->num_cells());

  double x_spacing = 0.001;
  double y_spacing = 0.002;

  // NOTE we want them to be constructed column-wise, because this is how Luca
  // puts the data in XX.txt
  size_t i_cell = 0;
  for (size_t iy = 0; iy < 10; ++iy) {
    for (size_t ix = 0; ix < 10; ++ix) {
      BOOST_CHECK_CLOSE(grid->cell(i_cell).x, ix*x_spacing, 1e-4);
      BOOST_CHECK_CLOSE(grid->cell(i_cell).y, iy*y_spacing, 1e-4);
      ++i_cell;
    }
  }

  auto expected = generateValues();
  CHECK_CLOSE_COLLECTION(grid->getRawValues(), expected, 1e-3);
}

BOOST_AUTO_TEST_SUITE_END()
