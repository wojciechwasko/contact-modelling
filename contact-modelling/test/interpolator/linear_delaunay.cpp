#include <boost/test/unit_test.hpp>

#include <cmath>
#include <vector>
#include <array>
#include <memory>
#include <stdexcept>

#include "cm/grid/grid.hpp"
#include "cm/interpolator/linear_delaunay.hpp"

struct MockCell {
  std::array<double,2> relative_position;
};

// base it off equilateral triangle with center of gravity at (0,0)
cm::Grid* createMockSourceGrid(size_t dim)
{
  MockCell temp_cell;
  std::vector<MockCell> mock_source;

  const double side = 3;
  const double height = side * sqrt(3)/2.0;

  // lower left
  temp_cell.relative_position[0] = -0.5 * side;
  temp_cell.relative_position[1] = -height / 3.0;
  mock_source.push_back(temp_cell);

  // lower right
  temp_cell.relative_position[0] = 0.5 * side;
  temp_cell.relative_position[1] = -height / 3.0;
  mock_source.push_back(temp_cell);

  // top (centre)
  temp_cell.relative_position[0] = 0;
  temp_cell.relative_position[1] = 2 * height / 3.0;
  mock_source.push_back(temp_cell);

  return cm::Grid::fromSensors(dim, cm::Rectangle(0.001, 0.001), mock_source.cbegin(), mock_source.cend());
}

// smack in the middle (0,0)
cm::Grid* createMockTargetGridInside(size_t dim)
{
  MockCell temp_cell;
  std::vector<MockCell> mock_source;

  temp_cell.relative_position[0] = 0;
  temp_cell.relative_position[1] = 0;
  mock_source.push_back(temp_cell);

  return cm::Grid::fromSensors(dim, cm::Rectangle(0.001, 0.001), mock_source.cbegin(), mock_source.cend());
}

// some crazy point (100,100)
cm::Grid* createMockTargetGridOutside(size_t dim)
{
  MockCell temp_cell;
  std::vector<MockCell> mock_source;

  temp_cell.relative_position[0] = 100;
  temp_cell.relative_position[1] = 100;
  mock_source.push_back(temp_cell);

  return cm::Grid::fromSensors(dim, cm::Rectangle(0.001, 0.001), mock_source.cbegin(), mock_source.cend());
}

BOOST_AUTO_TEST_CASE(simple_test_1D_inside)
{
  std::vector<double> values = {-1.23, 0.25, 4.23};
  std::unique_ptr<cm::Grid> m_source(createMockSourceGrid(1));  
  std::unique_ptr<cm::Grid> m_target(createMockTargetGridInside(1));  

  m_source->getRawValues().assign(values.begin(), values.end());

  cm::InterpolatorLinearDelaunay interpolator(cm::NIPP::InterpolateToZero);
  interpolator.offline(*m_source, *m_target);
  interpolator.interpolate(*m_source, *m_target);

  BOOST_CHECK_EQUAL(
    m_target->getRawValues()[0],
    values[0]/3.0 + values[1]/3.0 + values[2]/3.0
  );
};

BOOST_AUTO_TEST_CASE(simple_test_1D_outside_NIPP_to_zero)
{
  std::vector<double> values = {-1.23, 0.25, 4.23};
  std::unique_ptr<cm::Grid> m_source(createMockSourceGrid(1));  
  std::unique_ptr<cm::Grid> m_target(createMockTargetGridOutside(1));  

  m_source->getRawValues().assign(values.begin(), values.end());

  cm::InterpolatorLinearDelaunay interpolator(cm::NIPP::InterpolateToZero);
  interpolator.offline(*m_source, *m_target);
  interpolator.interpolate(*m_source, *m_target);

  BOOST_CHECK_EQUAL(
    m_target->getRawValues()[0],
    0
  );
};

BOOST_AUTO_TEST_CASE(simple_test_1D_outside_NIPP_remove)
{
  std::vector<double> values = {-1.23, 0.25, 4.23};
  std::unique_ptr<cm::Grid> m_source(createMockSourceGrid(1));  
  std::unique_ptr<cm::Grid> m_target(createMockTargetGridOutside(1));  

  m_source->getRawValues().assign(values.begin(), values.end());

  cm::InterpolatorLinearDelaunay interpolator(cm::NIPP::RemoveFromGrid);
  interpolator.offline(*m_source, *m_target);
  interpolator.interpolate(*m_source, *m_target);

  BOOST_CHECK_EQUAL(
    m_target->num_cells(),
    0
  );
};

BOOST_AUTO_TEST_CASE(throw_1D_to_3D)
{
  std::unique_ptr<cm::Grid> m_source(createMockSourceGrid(1));  
  std::unique_ptr<cm::Grid> m_target(createMockTargetGridInside(3));  
  cm::InterpolatorLinearDelaunay interpolator(cm::NIPP::RemoveFromGrid);
  BOOST_CHECK_THROW(
    interpolator.offline(*m_source, *m_target),
    std::runtime_error
  );

  BOOST_CHECK_THROW(
    interpolator.interpolate(*m_source, *m_target),
    std::runtime_error
  );
}

BOOST_AUTO_TEST_CASE(throw_3D_to_1D)
{
  std::unique_ptr<cm::Grid> m_source(createMockSourceGrid(3));  
  std::unique_ptr<cm::Grid> m_target(createMockTargetGridInside(1));  
  cm::InterpolatorLinearDelaunay interpolator(cm::NIPP::RemoveFromGrid);
  BOOST_CHECK_THROW(
    interpolator.offline(*m_source, *m_target),
    std::runtime_error
  );

  BOOST_CHECK_THROW(
    interpolator.interpolate(*m_source, *m_target),
    std::runtime_error
  );
}
