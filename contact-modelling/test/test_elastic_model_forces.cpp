#define BOOST_TEST_MODULE elastic_model_forces
#include <boost/test/unit_test.hpp>
#include "inc/custom_test_macros.hpp"

#include <cstddef>
#include <array>
#include <vector>

#include "cm/algorithm/forces_to_displacements.hpp"

#include "cm/details/external/armadillo.hpp"
#include "cm/mesh/interface.hpp"

struct MockNode {
  double x;
  double y;
};

class MockMesh : public cm::MeshInterface {
private:
  size_t offset;

public:
  MockMesh(size_t dim, size_t no_nodes) : cm::MeshInterface(dim, no_nodes), offset(0) {}
  void add_node(MockNode n)
  {
    node(offset).x = n.x;
    node(offset).y = n.y;
    for (size_t i = 0; i < D; ++i)
      setValue(offset, i, 0);
    ++offset;
  }

private:
  double impl_node_area(size_t i) const
  {
    return 0.002;
  }
};

// generated with gen/gen.py
std::vector<double> expected_diff_3_3 = {4.77612499182e-06,3.32211154015e-06,-1.15517619829e-05, 3.32211154015e-06,7.05414433363e-06,-1.61724667761e-05, -1.15517619829e-05,-1.61724667761e-05,-2.21751661574e-06};
std::vector<double> expected_diff_1_3 = {-1.15517619829e-05,-1.61724667761e-05,-2.21751661574e-06};
std::vector<double> expected_diff_3_1 = {-1.15517619829e-05,-1.61724667761e-05,-2.21751661574e-06};
std::vector<double> expected_diff_1_1 = {-2.21751661574e-06};
std::vector<double> expected_same_3_3 = {1.93137101012e-05,0,0, 0,1.93137101012e-05,0, 0,0,3.86274202023e-05};
std::vector<double> expected_same_1_3 = {0,0,3.86274202023e-05};
std::vector<double> expected_same_3_1 = {0,0,3.86274202023e-05};
std::vector<double> expected_same_1_1 = {3.86274202023e-05};

// -- TEST CASES: mesh nodes "far away", i.e. dx > eps_samepoint && dy > eps_samepoint
// !begin

BOOST_AUTO_TEST_CASE(em_other_nodes_1_1_dim_3_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(3, 1);

  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.105,0.107});

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_diff_3_3, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_other_nodes_1_1_dim_1_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(1, 1);

  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.105,0.107});

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_diff_1_3, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_other_nodes_1_1_dim_3_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(3, 1);

  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.105,0.107});

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_diff_3_1, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_other_nodes_1_1_dim_1_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 0.0001, dy = 0.0004 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.105,0.107});

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_diff_1_1, 1e-5);
}
// !end


// -- TEST CASES: mesh nodes "very close", i.e. dx < eps_samepoint && dy < eps_samepoint
// !begin

BOOST_AUTO_TEST_CASE(em_same_nodes_1_1_dim_3_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1, 0.1});

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_same_3_3, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_same_nodes_1_1_dim_1_3)
{
  MockMesh forces_mesh(3, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1, 0.1});

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_same_1_3, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_same_nodes_1_1_dim_3_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(3, 1);

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1, 0.1});

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_same_3_1, 1e-5);
}

BOOST_AUTO_TEST_CASE(em_same_nodes_1_1_dim_1_1)
{
  MockMesh forces_mesh(1, 1);
  MockMesh disps_mesh(1, 1);

  // note: for generator dx = 1e-5, dy = 1e-6 (it'd disps.x/y - forces.x/y)
  forces_mesh.add_node({0.1,0.1});
  disps_mesh.add_node({0.1, 0.1});

  typedef cm::AlgForcesToDisplacements alg_forces_to_disps;
  alg_forces_to_disps::params_type                    params_forces_to_disps;
  params_forces_to_disps.skin_props.E = 300000;
  params_forces_to_disps.skin_props.h  = 0.002;
  arma::mat precomputed = boost::any_cast<arma::mat>(alg_forces_to_disps().offline(forces_mesh, disps_mesh, params_forces_to_disps));
  CHECK_CLOSE_COLLECTION(precomputed, expected_same_1_1, 1e-5);
}
// !end

