#define BOOST_TEST_MODULE elastic_model
#include <boost/test/unit_test.hpp>
#include "inc/custom_test_macros.hpp"

#include <cstddef>
#include <array>
#include <vector>

#include "AlgForcesToDisplacements.hpp"

struct MockNode {
  double x;
  double y;
};

template <size_t dim>
class MockMesh {
public:
  typedef std::vector<double> values_container;

private:
  std::vector<MockNode> nodes;
  values_container values;

public:
  constexpr static size_t D = dim;
  size_t no_nodes() const { return nodes.size(); }
  const MockNode& node(size_t i) const { return nodes.at(i); }
  double node_area(size_t i) const { return 0.002; }
  const values_container& getRawValues() const { return values; }
  void add_node(MockNode n)
  {
    nodes.push_back(n);
    for (size_t i = 0; i < D; ++i)
      values.push_back(0);
  }
};

BOOST_AUTO_TEST_CASE(em_other_nodes_1_1_dim_3_3)
{
  MockMesh<3> forces_mesh;
  MockMesh<3> disps_mesh;

  // note: for generator dx = 0.0005, dy = 0.0007 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1005,0.1007});

  typedef AlgForcesToDisplacements<MockMesh<3>, MockMesh<3>> alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.elasticModulus = 300000;
  params_forces_to_disps.skin_props.skinThickness  = 0.002;
  alg_forces_to_disps::precomputed_type precomputed = alg_forces_to_disps::offline(forces_mesh, disps_mesh, params_forces_to_disps);
  std::vector<double> expected = {
    0.000284267757585335,
    0.000136847848007961,
    -2.57040388378672e-05,
    0.000136847848007961,
    0.000378106281933651,
    -3.59856543730141e-05,
    -2.57040388378672e-05,
    -3.59856543730141e-05,
    8.37031393710367e-05
  };
  CHECK_CLOSE_COLLECTION(precomputed, expected, 1e-5);

}
