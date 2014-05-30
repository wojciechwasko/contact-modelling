#include <boost/test/unit_test.hpp>
#include "custom_test_macros.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <boost/filesystem.hpp>

#include "cm/grid/grid.hpp"
#include "cm/grid/plot.hpp"
#include "cm/grid/cell_shapes.hpp"

struct MockCell {
  MockCell(double x, double y) : relative_position{x,y} {};
  std::array<double, 2> relative_position;
};

struct MockGrid {
  std::vector<MockCell> cells;

  std::unique_ptr<cm::Grid> create(size_t dim, const cm::GridCellShape& cell_shape)
  {
    return std::unique_ptr<cm::Grid>(cm::Grid::fromSensors(dim, cell_shape, cells.cbegin(), cells.cend()));
  }
};

std::unique_ptr<cm::Grid> generateGrid()
{
  MockGrid source;
  source.cells.push_back({0.1, 0.2});
  source.cells.push_back({0.3, 0.4});
  auto ret = source.create(1, cm::Rectangle(0.002, 0.001));
  ret->setValue(0, 0, 1.23);
  ret->setValue(1, 0, 5.67);
  return ret;
}

BOOST_AUTO_TEST_SUITE(dump_grid)

BOOST_AUTO_TEST_CASE(dump_to_ss)
{
  auto m = generateGrid();
  std::stringstream ss;
  cm::dumpForPlot(*m, ss);

  // comparing strings would be pretty useless due to the differences in precision between stream
  // output values on various platforms. We'll read the values from the ss and compare floats by
  // BOOST_CHECK_CLOSE
  std::vector<double> result(std::istream_iterator<double>(ss), {});
  std::vector<double> expected({0.002,0.001,0.1,0.3,0.2,0.4,1.23,5.67});
  CHECK_CLOSE_COLLECTION(result,expected, 0.1);
};

struct DumpToFile
{
  boost::filesystem::path tmp;
  DumpToFile()
    : tmp(boost::filesystem::unique_path("test-dump-grid-%%%%-%%%%-%%%%-%%%%"))
  {}

  ~DumpToFile()
  {
    boost::filesystem::remove_all(tmp);
  }
};

BOOST_FIXTURE_TEST_CASE(dump_to_f, DumpToFile)
{
  const std::string tmpstr    = tmp.native();  // optional

  auto m = generateGrid();
  dumpForPlot(*m, tmpstr);

  // comparing strings would be pretty useless due to the differences in precision between stream
  // output values on various platforms. We'll read the values from the ss and compare floats by
  // BOOST_CHECK_CLOSE
  std::ifstream i(tmpstr);
  std::vector<double> result(std::istream_iterator<double>(i), {});
  std::vector<double> expected({0.002,0.001,0.1,0.3,0.2,0.4,1.23,5.67});
  CHECK_CLOSE_COLLECTION(result,expected, 0.1);
};

BOOST_AUTO_TEST_SUITE_END()
