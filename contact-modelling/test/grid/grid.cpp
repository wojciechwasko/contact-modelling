#include <boost/test/unit_test.hpp>
#include "custom_test_macros.hpp"
#include <algorithm>
#include <memory>
#include <array>
#include <vector>

#include "cm/grid/grid.hpp"
#include "cm/grid/cell_shapes.hpp"

BOOST_AUTO_TEST_SUITE(grid_construction)

BOOST_AUTO_TEST_CASE(grid_fromSensors)
{
  struct MockCell { std::array<double, 2> relative_position; };
  std::vector<MockCell> mock_source;
  mock_source.push_back({1.23, 4.56});
  mock_source.push_back({2.34, 5.67});

  typedef cm::Square shape_type;
  auto shape = shape_type(0.001);

  std::unique_ptr<cm::Grid> ptr(
    cm::Grid::fromSensors(3, shape, mock_source.begin(), mock_source.end())
  );

  BOOST_CHECK_EQUAL(2, ptr->num_cells());
  BOOST_CHECK_CLOSE(1.23, ptr->cell(0).x, 1e-5);
  BOOST_CHECK_CLOSE(4.56, ptr->cell(0).y, 1e-5);
  BOOST_CHECK_CLOSE(2.34, ptr->cell(1).x, 1e-5);
  BOOST_CHECK_CLOSE(5.67, ptr->cell(1).y, 1e-5);

  BOOST_CHECK_CLOSE(1.2295, ptr->minX(), 1e-5);
  BOOST_CHECK_CLOSE(2.3405, ptr->maxX(), 1e-5);

  BOOST_CHECK_CLOSE(4.5595, ptr->minY(), 1e-5);
  BOOST_CHECK_CLOSE(5.6705, ptr->maxY(), 1e-5);

  BOOST_CHECK_EQUAL(6, ptr->getRawValues().size());

  CHECK_DOWNCAST(ptr->getCellShape(), const shape_type&);
  BOOST_CHECK_EQUAL(shape.dx(), ptr->getCellShape().dx());
};

BOOST_AUTO_TEST_CASE(grid_fromEmpty)
{
  typedef cm::Square shape_type;
  auto shape = shape_type(0.001);
  std::unique_ptr<cm::Grid> ptr(
    cm::Grid::fromEmpty(1, shape)
  );

  BOOST_CHECK_EQUAL(0, ptr->num_cells());

  BOOST_CHECK_EQUAL(0, ptr->getRawValues().size());
  
  CHECK_DOWNCAST(ptr->getCellShape(), const shape_type&);
  BOOST_CHECK_EQUAL(shape.dx(), ptr->getCellShape().dx());
}

BOOST_AUTO_TEST_CASE(grid_fromFill_coords_perfect_fit)
{
  typedef cm::Rectangle shape_type;
  auto shape = shape_type(0.001, 0.002);
  std::unique_ptr<cm::Grid> ptr(
    cm::Grid::fromFill(3, shape, 0, 0, 0.002, 0.006)
  );

  // we need to do it this way so that this test is independent of whether nodes
  // are constructed row- or column-wise

  std::vector<std::array<double, 2>> expected_cells;
  std::vector<std::array<double, 2>> actual_cells;

  expected_cells.push_back({0.0005, 0.0010});
  expected_cells.push_back({0.0005, 0.0030});
  expected_cells.push_back({0.0005, 0.0050});

  expected_cells.push_back({0.0015, 0.0010});
  expected_cells.push_back({0.0015, 0.0030});
  expected_cells.push_back({0.0015, 0.0050});

  std::for_each(ptr->cells_cbegin(), ptr->cells_cend(), [&](const cm::GridCell& c) {
    actual_cells.push_back({c.x, c.y}); 
  });

  std::sort(expected_cells.begin(), expected_cells.end());
  std::sort(actual_cells.begin(), actual_cells.end());

  const size_t num = expected_cells.size();
  BOOST_CHECK_EQUAL(num, ptr->num_cells());
  BOOST_CHECK_EQUAL(num*3, ptr->getRawValues().size());

  for (size_t i = 0; i < num; ++i) {
    BOOST_CHECK_CLOSE(expected_cells[i][0], actual_cells[i][0], 1e-5);
    BOOST_CHECK_CLOSE(expected_cells[i][1], actual_cells[i][1], 1e-5);
  }

  BOOST_CHECK_CLOSE(ptr->minX(), 0, 1e-5);
  BOOST_CHECK_CLOSE(ptr->maxX(), 0.002, 1e-5);

  BOOST_CHECK_CLOSE(ptr->minY(), 0, 1e-5);
  BOOST_CHECK_CLOSE(ptr->maxY(), 0.006, 1e-5);

  CHECK_DOWNCAST(ptr->getCellShape(), const shape_type&);
  BOOST_CHECK_EQUAL(shape.dx(), ptr->getCellShape().dx());
}

BOOST_AUTO_TEST_CASE(grid_fromFill_coords_not_perfect_fit)
{
  typedef cm::Rectangle shape_type;
  auto shape = shape_type(0.001, 0.002);
  std::unique_ptr<cm::Grid> ptr(
    cm::Grid::fromFill(3, shape, 0, 0, 0.0021, 0.0021)
  );

  // we need to do it this way so that this test is independent of whether nodes
  // are constructed row- or column-wise

  std::vector<std::array<double, 2>> expected_cells;
  std::vector<std::array<double, 2>> actual_cells;

  expected_cells.push_back({0.00055, 0.00105});
  expected_cells.push_back({0.00155, 0.00105});

  std::for_each(ptr->cells_cbegin(), ptr->cells_cend(), [&](const cm::GridCell& c) {
    actual_cells.push_back({c.x, c.y}); 
  });

  std::sort(expected_cells.begin(), expected_cells.end());
  std::sort(actual_cells.begin(), actual_cells.end());

  const size_t num = expected_cells.size();
  BOOST_CHECK_EQUAL(num, ptr->num_cells());
  BOOST_CHECK_EQUAL(num*3, ptr->getRawValues().size());

  for (size_t i = 0; i < num; ++i) {
    BOOST_CHECK_CLOSE(expected_cells[i][0], actual_cells[i][0], 1e-5);
    BOOST_CHECK_CLOSE(expected_cells[i][1], actual_cells[i][1], 1e-5);
  }

  BOOST_CHECK_CLOSE(ptr->minX(), 0.00005, 1e-5);
  BOOST_CHECK_CLOSE(ptr->maxX(), 0.00205, 1e-5);

  BOOST_CHECK_CLOSE(ptr->minY(), 0.00005, 1e-5);
  BOOST_CHECK_CLOSE(ptr->maxY(), 0.00205, 1e-5);

  CHECK_DOWNCAST(ptr->getCellShape(), const shape_type&);
  BOOST_CHECK_EQUAL(shape.dx(), ptr->getCellShape().dx());
}

BOOST_AUTO_TEST_CASE(grid_fromFill_other)
{
  typedef cm::Rectangle shape_type;
  auto shape = shape_type(0.001, 0.002);
  std::unique_ptr<cm::Grid> src(
    cm::Grid::fromFill(1, shape, 0, 0, 0.0021, 0.0021)
  );

  std::unique_ptr<cm::Grid> ptr(
    cm::Grid::fromFill(3, shape, *src)
  );

  // we need to do it this way so that this test is independent of whether nodes
  // are constructed row- or column-wise

  std::vector<std::array<double, 2>> expected_cells;
  std::vector<std::array<double, 2>> actual_cells;

  expected_cells.push_back({0.00055, 0.00105});
  expected_cells.push_back({0.00155, 0.00105});

  std::for_each(ptr->cells_cbegin(), ptr->cells_cend(), [&](const cm::GridCell& c) {
    actual_cells.push_back({c.x, c.y}); 
  });

  std::sort(expected_cells.begin(), expected_cells.end());
  std::sort(actual_cells.begin(), actual_cells.end());


  const size_t num = expected_cells.size();
  BOOST_CHECK_EQUAL(num, ptr->num_cells());
  BOOST_CHECK_EQUAL(num*3, ptr->getRawValues().size());

  for (size_t i = 0; i < num; ++i) {
    BOOST_CHECK_CLOSE(expected_cells[i][0], actual_cells[i][0], 1e-5);
    BOOST_CHECK_CLOSE(expected_cells[i][1], actual_cells[i][1], 1e-5);
  }

  BOOST_CHECK_CLOSE(ptr->minX(), 0.00005, 1e-5);
  BOOST_CHECK_CLOSE(ptr->maxX(), 0.00205, 1e-5);

  BOOST_CHECK_CLOSE(ptr->minY(), 0.00005, 1e-5);
  BOOST_CHECK_CLOSE(ptr->maxY(), 0.00205, 1e-5);

  CHECK_DOWNCAST(ptr->getCellShape(), const shape_type&);
  BOOST_CHECK_EQUAL(shape.dx(), ptr->getCellShape().dx());
}

BOOST_AUTO_TEST_SUITE_END()

