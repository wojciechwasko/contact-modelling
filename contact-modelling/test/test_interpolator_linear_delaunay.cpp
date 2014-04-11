#define BOOST_TEST_MODULE interpolator_linear_delaunay
#include <boost/test/unit_test.hpp>

#include <cmath>
#include <vector>
#include <array>
#include <memory>

#include "MeshInterface.hpp"
#include "MeshNatural.hpp"
#include "InterpolatorLinearDelaunay.hpp"

struct MockNode {
  std::array<double,2> relative_position;
};

// base it off equilateral triangle with center of gravity at (0,0)
MeshInterface* createMockSourceMesh(size_t dim)
{
  MockNode temp_node;
  std::vector<MockNode> mock_source;

  const double side = 3;
  const double height = side * sqrt(3)/2.0;

  // lower left
  temp_node.relative_position[0] = -0.5 * side;
  temp_node.relative_position[1] = -height / 3.0;
  mock_source.push_back(temp_node);

  // lower right
  temp_node.relative_position[0] = 0.5 * side;
  temp_node.relative_position[1] = -height / 3.0;
  mock_source.push_back(temp_node);

  // top (centre)
  temp_node.relative_position[0] = 0;
  temp_node.relative_position[1] = 2 * height / 3.0;
  mock_source.push_back(temp_node);

  return new MeshNatural(dim, mock_source.cbegin(), mock_source.cend());
}

// smack in the middle (0,0)
MeshInterface* createMockTargetMeshInside(size_t dim)
{
  MockNode temp_node;
  std::vector<MockNode> mock_source;

  temp_node.relative_position[0] = 0;
  temp_node.relative_position[1] = 0;
  mock_source.push_back(temp_node);

  return new MeshNatural(dim, mock_source.cbegin(), mock_source.cend());
}

// some crazy point (100,100)
MeshInterface* createMockTargetMeshOutside(size_t dim)
{
  MockNode temp_node;
  std::vector<MockNode> mock_source;

  temp_node.relative_position[0] = 100;
  temp_node.relative_position[1] = 100;
  mock_source.push_back(temp_node);

  return new MeshNatural(dim, mock_source.cbegin(), mock_source.cend());
}

BOOST_AUTO_TEST_CASE(simple_test_1D_inside)
{
  std::vector<double> values = {-1.23, 0.25, 4.23};
  std::unique_ptr<MeshInterface> m_source(createMockSourceMesh(1));  
  std::unique_ptr<MeshInterface> m_target(createMockTargetMeshInside(1));  

  m_source->getRawValues().assign(values.begin(), values.end());

  InterpolatorLinearDelaunay interpolator(NIPP::InterpolateToZero);
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
  std::unique_ptr<MeshInterface> m_source(createMockSourceMesh(1));  
  std::unique_ptr<MeshInterface> m_target(createMockTargetMeshOutside(1));  

  m_source->getRawValues().assign(values.begin(), values.end());

  InterpolatorLinearDelaunay interpolator(NIPP::InterpolateToZero);
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
  std::unique_ptr<MeshInterface> m_source(createMockSourceMesh(1));  
  std::unique_ptr<MeshInterface> m_target(createMockTargetMeshOutside(1));  

  m_source->getRawValues().assign(values.begin(), values.end());

  InterpolatorLinearDelaunay interpolator(NIPP::RemoveFromMesh);
  interpolator.offline(*m_source, *m_target);
  interpolator.interpolate(*m_source, *m_target);

  BOOST_CHECK_EQUAL(
    m_target->no_nodes(),
    0
  );
};
